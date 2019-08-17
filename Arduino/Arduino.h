#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "sensor.h"
#include "plant_guardian.h"

#define BAUD 9600
#define MYUBRR (F_CPU/16/BAUD-1)
#define MAX_BUF 256
#define ERROR "[PlantGuardian]: An error occurred in transmission, please try again\n"
#define INVALID_COMMAND "[PlantGuardian]: Sorry, the command you gave is invalid\n"
#define ACK "[PlantGuardian]: Message received\n"
#define NEW_COMMAND "[PlantGuardian]: Ready for a new command\n"
#define RESULT "[PlantGuardian]: Sensor read: \n"
#define EEPROM_STRING_1 "[Plant Guardian]: Temperature: \n"
#define EEPROM_STRING_2 " Humidity: \n"
#define EEPROM_STRING_3 " Light: \n"

//serial functions
void UART_init(void){
  // Set baud rate
  UBRR0H = (uint8_t)(MYUBRR>>8);
  UBRR0L = (uint8_t)MYUBRR;

  UCSR0C = (1<<UCSZ01) | (1<<UCSZ00); /* 8-bit data */ 
  UCSR0B = (1<<RXEN0) | (1<<TXEN0) | (1<<RXCIE0);   /* Enable RX and TX */  

}

void UART_putChar(uint8_t c){
  // wait for transmission completed, looping on status bit
  while ( !(UCSR0A & (1<<UDRE0)) ); 

  // Start transmission
  UDR0 = c;
}

uint8_t UART_getChar(void){
  // Wait for incoming data, looping on status bit
  while ( !(UCSR0A & (1<<RXC0)) ); 
  
  // Return the data
  return UDR0;
    
}

// reads a string until the first newline or 0
// returns the size read
uint8_t UART_getString(uint8_t* buf){
  uint8_t* b0=buf; //beginning of buffer
  while(1){
    uint8_t c=UART_getChar();
    *buf=c;
    ++buf;
    // reading a 0 terminates the string
    if (c==0)
      return buf-b0;
    // reading a \n  or a \r return results
    // in forcedly terminating the string
    if(c=='\n'||c=='\r'){
      *buf=0;
      ++buf;
      return buf-b0;
    }
  }
}

void UART_putString(uint8_t* buf){
  while(*buf){
    UART_putChar(*buf);
    ++buf;
  }
}

//checksum functions
unsigned char calculateLRC(char *buf, int lengh){
    unsigned char checksum = 0;
    while (lengh > 0){
        checksum += *buf++;
        lengh--;
    }
    return ((~checksum)+1);
}

void serialize(char* src, char* dest, int ch){      //dest will always be at least as long as src
    int i = 0;
    while(src[i] != '\n'){
        dest[i] = src[i];
        i++;
    }
    dest[i] = '$';  //it separates the string from the checksum
    i++;       
    sprintf(dest+i, "%d", ch);

    if (ch < 10)
        i++;
    else if (ch < 100)
        i += 2;
    else if (ch < 1000)
        i += 3;
    else 
        i += 4;

    dest[i] = '\n';
}

int deserialize(char* src, char* aux_s, char* aux_i){
    int i = 0;
    while(src[i] != '$'){
        aux_s[i] = src[i];
        //printf("aux_s[i] = %c ", aux_s[i]);
        i++;
    }
    aux_s[i] = '\n';
    //printf("aux_s = %s ", aux_s);
    i++;
    int k = 0;
    while(src[i] != '\n'){
        aux_i[k] = src[i];
        //printf("aux_i[k] = %c ", aux_i[k]);
        i++;
        k++;
    }
    aux_i[k] = '\0';
    //printf("aux_i = %s ", aux_i);
    return atoi(aux_i);
    //number =  atoi(aux_i); this doesn't work for some mysterious reason
}

void serialize_sensor(char* src, char* dest, uint16_t ch){      //dest will always be at least as long as src
    int i = 0;
    while(src[i] != '\n'){
        dest[i] = src[i];
        i++;
    }

    sprintf(dest+i, "%d", ch);

    if (ch < 10)
        i++;
    else if (ch < 100)
        i += 2;
    else if (ch < 1000)
        i += 3;
    else 
        i += 4;

    dest[i] = '\n';
}

void serialize_EEPROM(char* src1, char* src2, char* src3, char* dest, uint16_t tmp, uint16_t hum, uint16_t photo){      //dest will always be at least as long as src
    int i = 0;
    int j = 0;
    int k = 0; 

    while(src1[i] != '\n'){
        dest[i] = src1[i];
        i++;
    }
       
    sprintf(dest+i, "%d", tmp);

    if (tmp < 10)
        i++;
    else if (tmp < 100)
        i += 2;
    else if (tmp < 1000)
        i += 3;
    else 
        i += 4;

    dest[i] = ' ';      //it has to be written like this otherwise the compiler gives an internal error
    i++;
    dest[i] = 'C';
    i++;
    
    while(src2[j] != '\n'){
        dest[i] = src2[j];
        i++;
        j++;
    }
       
    sprintf(dest+i, "%d", hum);

    if (hum < 10)
        i++;
    else if (hum < 100)
        i += 2;
    else if (hum < 1000)
        i += 3;
    else 
        i += 4;

    dest[i] = ' ';
    i++;
    dest[i] = '%';
    i++;

    while(src3[k] != '\n'){
        dest[i] = src3[k];
        i++;
        k++;
    }
       
    sprintf(dest+i, "%d", photo);

    if (photo < 10)
        i++;
    else if (photo < 100)
        i += 2;
    else if (photo < 1000)
        i += 3;
    else 
        i += 4;

    dest[i] = ' ';
    i++;
    dest[i] = '%';
    i++;

    dest[i] = '\n';
}