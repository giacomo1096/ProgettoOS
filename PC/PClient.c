#include "PClient.h"

int fd; //Silvia: this is the file descriptor corresponding to the serial port
int c_fd; //Silvia: this is for serial_set_interface_attribs
char buf[BUF_LEN];

//client main
int main(int argc, char* argv[]){
    int ret;        //to see functions' return value

    //setting up quit_command
    char* quit_command = QUIT_COMMAND;
    size_t quit_command_len = strlen(quit_command);

    //opening corectly the serial port
    fd = serial_open("/dev/ttyACM0");
    c_fd = serial_set_interface_attribs(fd, BAUD_RATE);
    delay(1000);

    //client main loop
    while(1){
        printf("Insert command for PlantGuardian: "); //da vedere nel dettaglio come gestire il protocollo (nome provvisorio)

        //read a line from shell [stdin] (fgets stores in buffer also newline symbol '/n')
        if (fgets(buf, sizeof(buf), stdin) != (char*)buf){
            fprintf(stderr, "Sorry, an error occured while reading your command. I'm exiting...\n");
            exit(EXIT_FAILURE);
        }
        printf("This is the command I received: %s\n", buf); //temporary, to debug

        ret = write_on_fd(fd, buf);
        delay(1000);

        clean_buffer(buf);
        printf("Buffer clean: %s\n", buf);

        read_from_fd(fd, buf);
        printf("Message received: %s",buf);

        //Exiting the loop after receiving quit_command
        if (!memcmp(buf, quit_command, quit_command_len)) break; 
    }

    exit(EXIT_SUCCESS);
}
