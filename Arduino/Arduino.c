#include "Arduino.h"

uint8_t buf[MAX_BUF];
uint16_t sensor;
char toPClient[MAX_BUF];
char commandToProcess[MAX_BUF];
char output_result[MAX_BUF];
int checksum;
int checksum_received;
int EEPROM_index;
char aux_checksum[5];
char cSREG; //to store the Status Register during interrupts (must be handled by software)
DataStruct* w_data;
DataStruct* r_data;

/*------- EEPROM---------*/

DataStruct EEMEM data_log[LOG_SIZE];
int current_eeprom_index = 0;

void EEPROM_write_data(DataStruct* data){

    eeprom_busy_wait();
    
    eeprom_write_block((const void*)data, (DataStruct*)&data_log[current_eeprom_index], DATA_STRUCT_LEN);

    current_eeprom_index = (current_eeprom_index+1)%LOG_SIZE;
    
}

//reads block from eeprom at address src
void EEPROM_read_data_block(DataStruct* data, int src_addr){

    eeprom_busy_wait();

    eeprom_read_block(data, (DataStruct*)&data_log[src_addr], DATA_STRUCT_LEN);
    
}


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

                sensor = 0;
                memset(toPClient, 0, MAX_BUF);
                memset(output_result, 0, MAX_BUF);

                cSREG = SREG;
                cli(); //disable interrupts 
                sensor = hum_sensor_read();
                SREG = cSREG;
                sei(); //enable interrupts

                serialize_sensor(RESULT, output_result, sensor);

                checksum = calculateLRC(output_result, strlen(output_result));
                serialize(output_result, toPClient, checksum);
                UART_putString((uint8_t*) toPClient);

                //UART_putString((uint8_t*) "Output humidity funtion\n"); //for now (the client will go in segmentation fault)
            }
            else if (strcmp(commandToProcess, "Read temperature sensor\n") == 0){

                sensor = 0;
                memset(toPClient, 0, MAX_BUF);
                memset(output_result, 0, MAX_BUF);
                
                cSREG = SREG;
                cli();
                sensor = tmp_sensor_read();
                SREG = cSREG;
                sei();

                serialize_sensor(RESULT, output_result, sensor);

                checksum = calculateLRC(output_result, strlen(output_result));
                serialize(output_result, toPClient, checksum);
                UART_putString((uint8_t*) toPClient);

                //to debug: UART_putString((uint8_t*) "Started temperature function\n"); //for now (the client will go in segmentation fault)
            }
            else if(strcmp(commandToProcess, "Read photosensor\n") == 0){

                sensor = 0;
                memset(toPClient, 0, MAX_BUF);
                memset(output_result, 0, MAX_BUF);

                cSREG = SREG;
                cli();
                sensor = photo_sensor_read();
                SREG = cSREG;
                sei();
                serialize_sensor(RESULT, output_result, sensor);

                checksum = calculateLRC(output_result, strlen(output_result));
                serialize(output_result, toPClient, checksum);
                UART_putString((uint8_t*) toPClient);

                //to debug: UART_putString((uint8_t*) "Started photosensor function\n"); //for now (the client will go in segmentation fault)
            }
            else if (strcmp(commandToProcess, "Log\n") == 0){

                for (EEPROM_index = current_eeprom_index-5; EEPROM_index < current_eeprom_index; EEPROM_index++){
                    memset(r_data, 0, sizeof(*r_data));
                    memset(toPClient, 0, MAX_BUF);
                    memset(output_result, 0, MAX_BUF);

                    EEPROM_read_data_block(r_data, EEPROM_index);

                    serialize_EEPROM(EEPROM_STRING_1, EEPROM_STRING_2, EEPROM_STRING_3, output_result, (*r_data).tmp, (*r_data).hum, (*r_data).photo);
                    checksum = calculateLRC(output_result, strlen(output_result));
                    serialize(output_result, toPClient, checksum);
                    UART_putString((uint8_t*) toPClient);
                }
                EEPROM_index = 0;
            }
            else if (strcmp(commandToProcess, "quit\n") == 0){

                UART_putString(buf);
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

    w_data = (DataStruct*) malloc(sizeof(DataStruct));
    r_data = (DataStruct*) malloc(sizeof(DataStruct));

    UART_init();    //also enables interrupt RX
    sei();          //set Global Interrupt Enable to 1 so that interrupts can be handled

    //main loop where periodic measurements are taken (to be tested)
    while (1){

        memset(w_data, 0, sizeof(*w_data));

        (*w_data).hum = hum_sensor_read();
        (*w_data).photo = photo_sensor_read();
        (*w_data).tmp = tmp_sensor_read();

        //for testing
        //(*w_data).hum = 100;
        //(*w_data).tmp = 21;
        //(*w_data).photo = 6;

        EEPROM_write_data(w_data);

        _delay_ms(3000);

    };  

}