#ifndef PLANT_GUARDIAN_H
#define PLANT_GUARDIAN_H

/*-------- general parameters ---------*/
#define F_CFU 16000000
#define WORKING_PERIOD					//maybe we should define a new function which waits for n sec, instead of _delay_ms()


/*------ functions -------*/
#define set_bit(bit) (1 << (bit))

/*-------------------- data struct ---------------------*/

typedef struct {
	uint16_t tmp;
	uint16_t hum;
	uint16_t photo;
}DataStruct;

#endif