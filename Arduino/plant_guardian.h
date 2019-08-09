#ifndef PLANT_GUARDIAN_H
#define PLANT_GUARDIAN_H

/*-------- general parameters ---------*/
#define F_CFU 16000000
#define WORKING_PERIOD					//maybe we should define a new function which waits for n sec, instead of _delay_ms()

/*----- several parameters which can be passed to sensor functions -------*/
#define GET_TEMP_INFO		0
#define GET_PHOTO_INFO 		1
#define GET_HUM_INFO		2
#define GET_ALL_INFO		3
#define GET_LOG				4

/*-------- range allowed for each sensor value -----------*/
#define MIN_TEMP			0
#define MAX_TEMP			0

#define MIN_LIGHT			0
#define MAX_LIGHT			0

#define MIN_HUMIDITY		0
#define MAX_HUMIDITY		0

/*------ functions -------*/
#define set_bit(bit) (1 << (bit))

/*-------------------- data struct ---------------------*/

typedef struct {
	uint16_t tmp;
	uint16_t hum;
	uint16_t photo;
}DataStruct;

#endif