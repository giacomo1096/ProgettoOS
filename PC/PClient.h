#include <termios.h>
#include <errno.h>
#include <fcntl.h> //per settare dei flag in open()
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#define QUIT_COMMAND    "quit"
#define BUF_LEN 256
#define BAUD_RATE 9600
#define SERIAL_NAME "/dev/ttyACM0"

int serial_set_interface(int fd, int speed);    //This is the function to set correctly the serial attributes
int serial_open(const char* name);              //This is the function to open the serial port
int read_from_fd(int fd, char* buf);            //This is the function to read the serial port
int write_on_fd(int fd, const char* message);   //This is the function to write on the serial port
void clean_buffer(char* s);                     //This is the function to clean the buffer
int delay(long milliseconds);                   //This is a function to wait the time necessary for Arduino to write on the serial

void clean_buffer(char* s){
    int i = 0;
    while(i < strlen((char*) s)){
        s[i] = 0;
        i++;
    }
}

int delay(long milliseconds){
    usleep(milliseconds * 1000 ); // sleep milliseconds
}

int serial_set_interface_attribs(int fd, int speed){
    struct termios tty;
    memset(&tty, 0, sizeof tty);
    if(tcgetattr(fd, &tty) != 0){ //we get the attributes of the current structure
        fprintf(stderr, "[Error %d from tcgetattr\n]", errno);
        return -1;
    }

    //now we set the speed
    speed_t brate = speed;
    switch(speed) {
    case 4800:   brate=B4800;   
        break;
    case 9600:   brate=B9600;   
        break;
    case 38400:  brate=B38400;  
        break;
    case 57600:  brate=B57600;  
        break;
    case 115200: brate=B115200; 
        break;
    default:
        fprintf(stderr, "[Error: \"couldn't set baudrate %d\" in serial_set_interface_attribs]\n", speed);
        return -1;
    }

    //setting the same speed for input and output
    cfsetospeed(&tty, brate);
    cfsetispeed(&tty, brate);

    //here we set the data format: 8 bit with 1 stop bit, no parity (raw mode)
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;

    //no flow control
    tty.c_cflag &= ~CRTSCTS;

    tty.c_cflag |= CREAD | CLOCAL;  // turn on READ & ignore ctrl lines
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // turn off s/w flow ctrl

    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // make raw
    tty.c_oflag &= ~OPOST; // make raw

    tty.c_cc[VMIN]  = 0;
    tty.c_cc[VTIME] = 20;

    //now let's set the attributes
    if (tcsetattr(fd, TCSANOW, &tty) != 0){
        fprintf(stderr, "[Error %d from tcsetattr in serial_set_interface_attribs]\n", errno);
        return -1;
    }
    return 0;
}

int serial_open(const char* name){
    int fd = open(name, O_RDWR | O_NOCTTY | O_NDELAY);
    if(fd < 0){
        fprintf(stderr, "[Error %d opening serial in serial_open.\n fd = %d]\n", errno, fd);
        exit(EXIT_FAILURE);
    }
    return fd;
}

int read_from_fd(int fd, char* buf){
    char b[1];
    int i = 0;
    do {
        int things_read = read(fd, b, 1); //we read a char at a time
        if (things_read == -1){
            fprintf(stderr, "[Error while reading data from the serial port: returned %d]\n", errno);
            return -1;
        }
        if (things_read == 0){
            usleep(10 * 1000); //if Arduino needs more time to write he shall have it
            continue;
        }
        buf[i] = b[0];
        i++;
    } while(b[0] != '\n');
    buf[i] = 0; //terminating the string
    return 0;
}

int write_on_fd(int fd, const char* message){
    int message_len = strlen(message);
    int things_written = 0;
    while(things_written < message_len){
        things_written += write(fd, message, message_len-things_written); 
        if (things_written == -1){
            fprintf(stderr, "[Error while writing in the serial port: returned %d]\n", errno);
            exit(EXIT_FAILURE);
        }
    }
    return things_written;
}