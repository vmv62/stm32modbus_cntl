#include "stm32f030.h"
#include "modbus.h"

/*
Заполнение структурв
Данные:
	Выхода - физические(выходные регистры контроллера), виртуальные (регистр в котором хранится значение для вывода на сдвиговый регистр)
	Входа - физические(входные регистры контроллера), считанные в регистр хранения из расщирителя портов.
	Регитры хранения - память где хранятся настройки (энергонезависимая память). Должны читаться и обновлятся постоянно.
	Регистры входные - данные полученные с разных датчиков и регистров контроллера.

Представление данных в контроллере (в таблице регистров):
	Выхода - указатель на регистр с флагами выходов (uint16_t *coils)
	Входа - указатель на регистр с флагами входов.
	Регистры хранения - массив указателей хранения настроек. EEPROM размещение.

*/

void regs_filling(Regs_TypeDef *reg_t){
	ret_t->coils |= *coil_adr;		//Записываем в регистр состояние выходов, если входа виртуальные, пишем через функци.(GPIOA->IDR)
	reg_t->inputs |= *input_adr;		//Записываем состояние входов (для порта контроллера PIOA->ODR);
}

void read_coils(uint32_t *coil_adr){
	
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
