#ifndef COMPONENTS_H
#define COMPONENTS_H


/*--------- general settings -------------*/

#define TMP_SENSOR_PIN			(PORTF0)			//adc 0			
#define PHOTO_SENSOR_PIN	   	(PORTF4)			//adc 4
#define HUM_SENSOR_PIN			(PORTF2)			//adc 2
#define SERVO_PIN	   			(PORTB5)			//pin 11

/* -------- analog sensors -----------*/
void adc_init(void);							//initializes ADC avr settings

/*--------- temperature sensor ---------*/

void tmp_sensor_init(void);						//initializes temperature sensor settings
uint16_t tmp_sensor_read_(void);				//returns temperature read by temperature sensor (volts)


/*------ photoresistance --------------*/

void photo_sensor_init(void);					//initializes photo sensor settings
uint16_t photo_sensor_read_(void);				//returns photo sensor value (volts)


/*------- humidity sensor --------------*/

void hum_sensor_init(void);						//initializes photo sensor settings
uint16_t hum_sensor_read_(void);				//returns photo sensor value (volts)


/*-------- servo motor -----------------*/

void servo_init(void);							//initializes PWM and servo motor settings
void servo_open(void);							//puts servo at 180 degrees
void servo_close(void);							//puts servo at 0 degrees

#endif