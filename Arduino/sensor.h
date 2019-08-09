#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <avr/io.h>
#include <string.h>
#include <avr/eeprom.h>
#include "plant_guardian.h"
#include "components.h"
#include "eeprom_manager.h"

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




/*-----------------------------------------FUNCTIONS TO SET AND USE SENSORS------------------------------------*/

/*--------- analog sensors (both temperature and photoresistance) -----------------*/

void adc_init(void) {
	ADMUX  = 0x00;
  	ADCSRA = 0x00;
  	ADCSRB = 0x00;
  
  	ADMUX  |= set_bit(REFS0) | set_bit(MUX3)  | set_bit(MUX2)  | set_bit(MUX1)  | set_bit(MUX0);
  	ADCSRA |= set_bit(ADEN)  | set_bit(ADPS2) | set_bit(ADPS1) | set_bit(ADPS0) | set_bit(ADSC);
  	loop_until_bit_is_clear(ADCSRA, ADSC);
}

/*-----------temperature sensor ----------*/

void tmp_sensor_init() {
  	DDRF  &= ~set_bit(TMP_SENSOR_PIN);
  	PORTF &= ~set_bit(TMP_SENSOR_PIN);
  	DIDR0 |= set_bit(TMP_SENSOR_PIN);
}

uint16_t tmp_sensor_read_() {
  	ADMUX = (ADMUX & 0xf0) | (TMP_SENSOR_PIN & 0x0f);

  	ADCSRA |= set_bit(ADSC);
  	loop_until_bit_is_clear(ADCSRA, ADSC);

  	uint8_t lb = ADCL;
  	uint8_t hb = ADCH;

  return (((uint16_t) hb) << 8) | lb;
}


 

/*--------- photoresistance ----------*/


void photo_sensor_init(){
    DDRF  &= ~set_bit(PHOTO_SENSOR_PIN);
    PORTF &= ~set_bit(PHOTO_SENSOR_PIN);
    DIDR0 |= set_bit(PHOTO_SENSOR_PIN);
}

uint16_t photo_sensor_read_(){

    ADMUX = (ADMUX & 0xf0) | (PHOTO_SENSOR_PIN & 0x0f);

    ADCSRA |= set_bit(ADSC);
    loop_until_bit_is_clear(ADCSRA, ADSC);

    uint8_t lb = ADCL;
    uint8_t hb = ADCH;

  return (((uint16_t) hb) << 8) | lb;

}


/*--------------humidity sensor ----------*/

void hum_sensor_init(){
	DDRF  &= ~set_bit(HUM_SENSOR_PIN);
    PORTF &= ~set_bit(HUM_SENSOR_PIN);
    DIDR0 |= set_bit(HUM_SENSOR_PIN);
}

uint16_t hum_sensor_read_(){ 

	ADMUX = (ADMUX & 0xf0) | (HUM_SENSOR_PIN & 0x0f);

    ADCSRA |= set_bit(ADSC);
    loop_until_bit_is_clear(ADCSRA, ADSC);

    uint8_t lb = ADCL;
    uint8_t hb = ADCH;

  return (((uint16_t) hb) << 8) | lb;
}