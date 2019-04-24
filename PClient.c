#include <termios.h>
#include <errno.h>
#include <fcntl.h> //per settare dei flag in open()
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define QUIT_COMMAND    "quit"

int fd; //Silvia: this is the file descriptor corresponding to the serial port
int c_fd; //Silvia: this is for serial_set_interface_attribs

//qua mettiamo delle funzioni che poi potranno essere usate nel main (da vedere come)

//Silvia: this function sets up attributes to open the serial port
int serial_set_interface_attribs(int fd, int speed, int parity){
    struct termios tty;
    memset(&tty, 0, sizeof tty);
    if(tcgetattr(fd, &tty) != 0){
        fprintf(stderr, "[Error %d from tcgetattr", errno);
        return -1;
    }
    switch (speed){
        case 57600:
            speed = B57600;
            break;
        case 115200:
            speed = B115200;
            break;
        default:
            fprintf(stderr, "[Error: \"couldn't set baudrate %d\" in serial_set_interface_attribs\n", speed);
            return -1;
    }
    cfsetospeed(&tty, speed);
    cfsetispeed(&tty, speed);
    cfmakeraw(&tty);

    //enable reading 
    tty.c_cflag &= ~(PARENB | PARODD); //shut off parity
    tty.c_cflag |= parity;
    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; //8-bit chars

    if (tcsetattr(fd, TCSANOW, &tty) != 0){
        fprintf(stderr, "[Error %d from tcsetattr in serial_set_interface_attribs]\n", errno);
        return -1;
    }
    return 0;
}

//Silvia: this function contains the operations to open the serial port
int serial_open(const char* name){
    int fd = open(name, O_RDWR | O_NOCTTY | O_SYNC);
    if(fd < 0){
        fprintf(stderr, "[Error %d opening serial in serial_open.\n fd = %d\n", errno, fd);
    }
    return fd;
}

//Silvia: this function is used to read from the serial port 
int read_from_fd(int fd, char* buf){
    int things_read = -1;
    while(things_read < 0){
        things_read = read(fd, buf, 250); //temporary values
    }
    return things_read;
}

//Silvia: this function is used to write on the serial port
int write_on_fd(int fd, char* message){
    int message_len = strlen(message);
    int things_written = 0;
    while(things_written < message_len){
        things_written += write(fd, message, message_len-things_written); 
    }
    return things_written;
}

//Silvia: client main
int main(int argc, char* argv[]){
    char buf[1024];

    //Silvia: setting up quit_command
    char* quit_command = QUIT_COMMAND;
    size_t quit_command_len = strlen(quit_command);

    //Silvia: opening corectly the serial port
    fd = serial_open("/dev/ttyACM0");
    c_fd = serial_set_interface_attribs(fd, 57600, 0); //temporary values

    //Silvia: client main loop
    while(1){
        printf("Insert command for PlantGuardian: "); //da vedere nel dettaglio come gestire il protocollo (nome provvisorio)

        //read a line from shell [stdin] (fgets stores in buffer also newline symbol '/n')
        if (fgets(buf, sizeof(buf), stdin) != (char*)buf){
            fprintf(stderr, "Sorry, an error occured while reading your command. I'm exiting...\n");
            exit(EXIT_FAILURE);
        }
        printf("%s", buf); //temporary, to debug

        //Exiting the loop after receiving quit_command
        if (!memcmp(buf, quit_command, quit_command_len)) break; 

    }

    //Silvia: terminating successfully
    exit(EXIT_SUCCESS);
}
