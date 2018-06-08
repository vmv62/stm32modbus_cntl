#include "modbus.h"


uint16_t read_holding_registers(uint16_t reg_addr, uint16_t count, uint16_t *dest){
	uint32_t *contrl_addr;

	contrl_addr = FLASH_START + reg_addr;

	while(count){
		*dest = (*contrl_addr>>16);
		dest++;
		*dest = *contrl_addr;
		dest++;
		contrl_addr++;
		count--;
	}
	return count;
}
