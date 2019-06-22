#ifndef EEPROM_MANAGER_H
#define EEPROM_MANAGER_H

#define DATA_STRUCT_LEN 6 				//data struct len in bytes
#define LOG_SIZE		160				//buffer of datastruct size

void EEPROM_write_data(DataStruct* data);

void EEPROM_read_data_block(DataStruct* data, int src_addr);

//void EEPROM_read_all(DataStruct* data);

#endif