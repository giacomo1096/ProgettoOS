#include "Arduino.h"

uint8_t buf[MAX_BUF];
char toPClient[MAX_BUF];
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

    UART_init();

    //main loop
    while(1) {
        //clean the buffers
        memset(buf, 0, MAX_BUF);
        memset(toPClient, 0, MAX_BUF);

        UART_getString(buf);

        //UART_putString(buf); //to debug
        checksum_received = deserialize((char*) buf, toPClient, aux_checksum);
        checksum = calculateLRC(toPClient, strlen(toPClient));

       // UART_putString((uint8_t*) toPClient); //to debug

        if (checksum != checksum_received){
            serialize(error, toPClient, checksum_received);
            UART_putString((uint8_t*) toPClient);
            continue;
        }
        else {
            //clean th buffers
            memset(buf, 0, MAX_BUF);
            memset(toPClient, 0, MAX_BUF);

            checksum = calculateLRC(ack, ack_len);
            serialize(ack, toPClient, checksum);

            UART_putString((uint8_t*)toPClient);
        }
    }
}