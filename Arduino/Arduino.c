#include "Arduino.h"

uint8_t buf[MAX_BUF];
char toPClient[MAX_BUF];
char commandToProcess[MAX_BUF];
int checksum;
char aux_checksum[5];

int main(void){
    int checksum_received;

    //setting up ERROR
    char* error = ERROR;
    //size_t error_len = strlen(ERROR);

    //setting up ACK
    char* ack = ACK;
    size_t ack_len = strlen(ACK);

    //setting up INVALID_COMMAND
    char* invalid_command = INVALID_COMMAND;
    size_t invalid_command_len = strlen(invalid_command);

    //setting up NEW_COMMAND
    char* new_command = NEW_COMMAND;
    size_t new_command_len = strlen(NEW_COMMAND);

    UART_init();

    //main loop (dovrebbe partire ogni volta che arriva un interrupt dalla seriale)
    while(1) {
        //clean the buffers
        memset(buf, 0, MAX_BUF);
        memset(commandToProcess, 0, MAX_BUF);
        memset(toPClient, 0, MAX_BUF);

        UART_getString(buf);

        //UART_putString(buf); //to debug

        //first: control that the data reecived is correct
        checksum_received = deserialize((char*) buf, toPClient, aux_checksum);
        checksum = calculateLRC(toPClient, strlen(toPClient));

        //UART_putString((uint8_t*) toPClient); //to debug

        if (checksum != checksum_received){
            serialize(error, toPClient, checksum_received);  //it sends the checksum_received to the client to let it see the error
            UART_putString((uint8_t*) toPClient);
            //receiving ERROR will trigger in the client the response: insert a new command
            continue;
        }
        else {
            
            memcpy(commandToProcess, toPClient, strlen(toPClient));

            memset(toPClient, 0, MAX_BUF);

            //sending ACK
            checksum = calculateLRC(ack, ack_len);
            serialize(ack, toPClient, checksum);
            UART_putString((uint8_t*)toPClient);

            //the command received is correct, so let's process it 
            if (strcmp(commandToProcess, "Read humidity sensor\n") == 0){

                memset(toPClient, 0, MAX_BUF);
                //funzione umidità
                UART_putString((uint8_t*) "Output humidity funtion\n"); //for now (the client will go in segmentation fault)
            }
            else if (strcmp(commandToProcess, "Read temperature sensor\n") == 0){

                memset(toPClient, 0, MAX_BUF);
                //funzione temperatura
                UART_putString((uint8_t*) "Started temperature function\n"); //for now (the client will go in segmentation fault)
            }
            else if(strcmp(commandToProcess, "Read photosensor\n") == 0){

                memset(toPClient, 0, MAX_BUF);
                //funzione fotosensore
                UART_putString((uint8_t*) "Started photosensor function\n"); //for now (the client will go in segmentation fault)
            }
            else if (strcmp(commandToProcess, "Log\n") == 0){

                memset(toPClient, 0, MAX_BUF);
                //funzione log
                UART_putString((uint8_t*) "Started log function\n"); //for now (the client will go in segmentation fault)
            }
            else if (strcmp(commandToProcess, "quit\n") == 0){
                UART_putString(buf);
                break;
            } 
            else {
                
                memset(toPClient, 0, MAX_BUF);

                checksum = calculateLRC(invalid_command, invalid_command_len);
                serialize(invalid_command, toPClient, checksum);
                UART_putString((uint8_t*) toPClient);
            }

            //telling the client to send a new command
            memset(toPClient, 0, MAX_BUF);

            checksum = calculateLRC(new_command, new_command_len);
            serialize(new_command, toPClient, checksum);

            UART_putString((uint8_t*)toPClient);
        }
    }
}