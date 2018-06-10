#include "modbus.h"

void regs_filling(Regs_TypeDef *reg_t, uint32_t coil_adr, uint32_t input_adr ){
	reg_t->inputs |= GPIOA->ODR;
}

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
