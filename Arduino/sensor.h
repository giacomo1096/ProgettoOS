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

uint16_t tmp_sensor_read(){
	tmp_sensor_init();
	return tmp_sensor_read_();
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

uint16_t photo_sensor_read(){
	photo_sensor_init();
	return photo_sensor_read_();
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

uint16_t hum_sensor_read(){
	hum_sensor_init();
	return hum_sensor_read_();
}

/*------------- servo motor -----------*/

void servo_init(){

  	//servo settings	
  	PORTB |= (1<<SERVO_PIN);  							
  	DDRB |= (1<<SERVO_PIN);

  	//pwm settings
  	ICR1=20000;
  	OCR1A=1000; 								
  	TCCR1A=(1<<COM1A1);									

  	TCCR1B=(1<<WGM13)|(1<<CS11);  

}

void servo_open(){
	OCR1A = 550;
}

void servo_close(){
	OCR1A = 2200;
}


