#include "Arduino.h"

uint8_t buf[MAX_BUF];
uint16_t sensor;
char toPClient[MAX_BUF];
char commandToProcess[MAX_BUF];
char output_result[MAX_BUF];
int checksum;
int checksum_received;
char aux_checksum[5];
char cSREG; //to store the Status Register during interrupts (must be handled by software)


//let's try receiving the serial interrupt
ISR(USART0_RX_vect, ISR_BLOCK){

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
            serialize(ERROR, toPClient, checksum_received);  //it sends the checksum_received to the client to let it see the error
            UART_putString((uint8_t*) toPClient);
            //receiving ERROR will trigger in the client the response: insert a new command
            //continue;

            //telling the client to send a new command because the one it sent wasn't processable
            memset(toPClient, 0, MAX_BUF);

            checksum = calculateLRC(NEW_COMMAND, strlen(NEW_COMMAND));
            serialize(NEW_COMMAND, toPClient, checksum);

            UART_putString((uint8_t*)toPClient);
        }
        else {
            
            memcpy(commandToProcess, toPClient, strlen(toPClient));

            memset(toPClient, 0, MAX_BUF);

            //sending ACK
            checksum = calculateLRC(ACK, strlen(ACK));
            serialize(ACK, toPClient, checksum);
            UART_putString((uint8_t*)toPClient);

            //the command received is correct, so let's process it 
            if (strcmp(commandToProcess, "Read humidity sensor\n") == 0){

                /*sensor = 0;
                memset(toPClient, 0, MAX_BUF);
                memset(output_result, 0, MAX_BUF);

                cSREG = SREG;
                cli(); //disable interrupts 
                sensor = hum_sensor_read_();
                SREG = cSREG;
                sei(); //enable interrupts

                serialize_sensor(result, output_result, sensor);

                checksum = calculateLRC(output_result, strlen(output_result));
                serialize(output_result, toPClient, checksum);
                UART_putString((uint8_t*) toPClient);*/

                UART_putString((uint8_t*) "Output humidity funtion\n"); //for now (the client will go in segmentation fault)
            }
            else if (strcmp(commandToProcess, "Read temperature sensor\n") == 0){

                /*sensor = 0;
                memset(toPClient, 0, MAX_BUF);
                memset(output_result, 0, MAX_BUF);
                
                cSREG = SREG;
                cli();
                sensor = tmp_sensor_read_();
                SREG = cSREG;
                sei();

                serialize_sensor(result, output_result, sensor);

                checksum = calculateLRC(output_result, strlen(output_result));
                serialize(output_result, toPClient, checksum);
                UART_putString((uint8_t*) toPClient);*/

                //to debug: UART_putString((uint8_t*) "Started temperature function\n"); //for now (the client will go in segmentation fault)
            }
            else if(strcmp(commandToProcess, "Read photosensor\n") == 0){

                /*sensor = 0;
                memset(toPClient, 0, MAX_BUF);
                memset(output_result, 0, MAX_BUF);

                cSREG = SREG;
                cli();
                sensor = photo_sensor_read_();
                SREG = cSREG;
                sei();
                serialize_sensor(result, output_result, sensor);

                checksum = calculateLRC(output_result, strlen(output_result));
                serialize(output_result, toPClient, checksum);
                UART_putString((uint8_t*) toPClient);*/

                //to debug: UART_putString((uint8_t*) "Started photosensor function\n"); //for now (the client will go in segmentation fault)
            }
            else if (strcmp(commandToProcess, "Log\n") == 0){

                memset(toPClient, 0, MAX_BUF);
                //funzione log
                UART_putString((uint8_t*) "Started log function\n"); //for now (the client will go in segmentation fault)
            }
            else if (strcmp(commandToProcess, "quit\n") == 0){
                UART_putString(buf);
                //break;
            } 
            else {
                
                memset(toPClient, 0, MAX_BUF);

                checksum = calculateLRC(INVALID_COMMAND, strlen(INVALID_COMMAND));
                serialize(INVALID_COMMAND, toPClient, checksum);
                UART_putString((uint8_t*) toPClient);
            }

            //telling the client to send a new command
            memset(toPClient, 0, MAX_BUF);

            checksum = calculateLRC(NEW_COMMAND, strlen(NEW_COMMAND));
            serialize(NEW_COMMAND, toPClient, checksum);

            UART_putString((uint8_t*)toPClient);
        }
}

int main(void){

    //let's prepare the sensors to do their job
    adc_init();
    tmp_sensor_init();
    hum_sensor_init();
    photo_sensor_init();

    //Questi dovranno essere cancellati se non sono utili
    /*//setting up ERROR
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

    //setting up RESULT    da fare ancora nel codice
    char* result = RESULT;
    //size_t result_len = strlen(RESULT); probably won't be needed*/

    UART_init();    //also enables interrupt RX
    sei();          //set Global Interrupt Enable to 1 so that interrupts can be handled

    while (1);  //necessary, otherwise the main is too short and the ISR isn't execute

}