#include "PClient.h"

int fd; //this is the file descriptor corresponding to the serial port
int c_fd; //this is for serial_set_interface_attribs
char buf[BUF_LEN];
char to_PlantGuardian[BUF_LEN];
int checksum;
char aux_checksum[5];

//client main
int main(int argc, char* argv[]){
    int ret;        //to see functions' return value
    int checksum_received;

    //setting up quit_command
    char* quit_command = QUIT_COMMAND;

    //opening corectly the serial port
    fd = serial_open("/dev/ttyACM0");
    c_fd = serial_set_interface_attribs(fd, BAUD_RATE);
    delay(1000);

    welcome_print();

    //client main loop
    while(1){

        white();
        printf("Please insert command for PlantGuardian: "); 

        //clean the buffers
        memset(buf, 0, BUF_LEN);
        memset(to_PlantGuardian, 0, BUF_LEN);

        //read a line from shell [stdin] (fgets stores in buffer also newline symbol '/n')
        if (fgets(buf, sizeof(buf), stdin) != (char*)buf){
            fprintf(stderr, "Sorry, an error occured while reading your command. I'm exiting...\n");
            exit(EXIT_FAILURE);
        }

        printf("\n");
        green();
        printf("This is the command I received: %s\n", buf); 

        checksum = calculateLRC(buf, strlen(buf));
        serialize(buf, to_PlantGuardian, checksum);

        printf("This is the command I'm going to send to Arduino: %s\n", to_PlantGuardian);
        ret = write_on_fd(fd, to_PlantGuardian);
        delay(1000);

        //verify I receive the checksum

        //clean the buffers
            memset(buf, 0, BUF_LEN);
            memset(to_PlantGuardian, 0, BUF_LEN);

            green();
            printf("Buffer clean: %s\n\n", buf);

            read_from_fd(fd, buf);
            delay(1000);

            blue();
            printf("Message received: %s",buf);

            checksum_received = deserialize(buf, to_PlantGuardian, aux_checksum);
            checksum = calculateLRC(to_PlantGuardian, strlen(to_PlantGuardian));
            printf("Deserialized struct: string = %s, checksum_received = %d, checksum calculated = %d\n", to_PlantGuardian, checksum_received, checksum);
            if (checksum_received == 142)   //we know 142 is che ACK's checksum
                printf("Trasmission occurred without errors!\nChecksum received!\n\n");
            else 
                printf("The checksum sent and received are not the same: an error occured durnig trasmission\n\n");

        //receive data according to what I asked
        do {
            //clean the buffers
            memset(buf, 0, BUF_LEN);
            memset(to_PlantGuardian, 0, BUF_LEN);

            green();
            printf("Buffer clean: %s\n\n", buf);

            read_from_fd(fd, buf);
            delay(1000);

            blue();
            printf("Message received: %s",buf);

            checksum_received = deserialize(buf, to_PlantGuardian, aux_checksum);
            checksum = calculateLRC(to_PlantGuardian, strlen(to_PlantGuardian));
            printf("Deserialized struct: string = %s, checksum_received = %d, checksum calculated = %d\n", to_PlantGuardian, checksum_received, checksum);
            if (checksum_received == checksum)
                printf("Trasmission occurred without errors!\n\n");
            else 
                printf("The checksum sent and received are not the same: an error occured durnig trasmission\n\n");
            
            cyan();
            printf("%s\n", to_PlantGuardian);

            //Exiting the loop after receiving quit_command
            if (strcmp(to_PlantGuardian, quit_command) == 0) 
            break; 
        } while (strcmp(to_PlantGuardian, NEW_COMMAND) != 0);

        //Exiting the second loop too after receiving quit_command
        if (strcmp(to_PlantGuardian, quit_command) == 0) 
            break; 
    }

    if (close(fd) < 0){
        fprintf(stderr, "[Error %d while closing the serial file descriptor %d", errno, fd);
        exit(EXIT_FAILURE);
    }

    white();
    printf("Thank you for using our service. Bye bye!\n");
    exit(EXIT_SUCCESS);
}
