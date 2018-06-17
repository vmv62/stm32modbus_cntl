#include "stm32f0xx.h"
#include "modbus.h"
#include "hw_init.h"

//Функция заполнения таблицы
uint16_t pase_pdu(uint8_t *buffer, RegsTable_TypeDef *REGS){
	PDU_QueryHead_TypeDef *PDU = ((PDU_QueryHead_TypeDef *)buffer);
	uint8_t err_holder;

	if(PDU->slave_addres != MDB_ADDR)
	{
		return MODBUS_ILLEGAL_SLAVE_ADDR;
	}


	switch(PDU->command)
	{
		case READ_COIL_STATUS:	if((err_holder = read_coils((uint8_t *)(PDU), REGS))){
						error_handler(err_holder, buffer);
					}
								break;
		case READ_HOLDING_REGISTERS:	if((err_holder = read_holding_registers((uint8_t *)(PDU), REGS))){
							error_handler(err_holder, buffer);
						}
								break;
		default:	error_handler(MODBUS_EXCEPTION_ILLEGAL_FUNCTION, buffer);
	}

	return 0;
}

//Заполнение таблицы данных которая хранит регистры устройства.
uint16_t regs_filling(RegsTable_TypeDef *REGS)
{
// Если в настройках указано читать регистр порта(флаг выставлен) читаем порт, если нет, читаем регстр в памяти.
	if(REGS->HOLD.CONT_FLAG & COILS_HDW){
		REGS->COILS = ((uint16_t)GPIOA->ODR);		//Считываем регистры порта и помещаем их в таблицу.
	}else{
//		REGC->COIL = 	//заполняем регистр в памяти функцией возвращающей состояние выходов.
	}

// Аналогично записи выше, только заполлняем регистры входов.
	if(REGS->HOLD.CONT_FLAG & INPUTS_HDW){
		REGS->COILS |= ((uint16_t)GPIOA->IDR);
	}else{
		REGS->COILS = ((uint16_t)USART1->ISR);
	}

//Заполняем регистры содержащие 16 битные данные для теста.
	REGS->INP_REG[0] = 0x10;
	REGS->INP_REG[1] = 0x20;
	REGS->INP_REG[2] = 0x30;
	REGS->INP_REG[4] = 0x50;

	return 0;
}

//--------------Обработка команды 01----------------------------------

uint16_t read_coils(uint8_t *buffer, RegsTable_TypeDef *REGS)
{
	PDU_Query_TypeDef *QueryPDU = ((PDU_Query_TypeDef *)buffer);
	uint8_t byte_count = 2;
	uint16_t tm_crc = crc16(buffer, 6);
	if(reg_swap(QueryPDU->crc) != tm_crc)
	{
		return 0;
	}

	uint16_t adr = reg_swap(QueryPDU->reg_addr);
	uint16_t cnt = reg_swap(QueryPDU->reg_count);
//Еслм сумма адрес плюс количество превышают колличество регистров, выдаем ошибку.
	if((adr + cnt) > COIL_REG_COUNT){
		return MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS;
	}
//Непосредственно читаем заполняем тело ответа
	uint16_t REG_TMP = REGS->COILS >> adr;
	buffer[2] = 2;
	buffer[3]= (REG_TMP >> 8);
	buffer[4] = (uint8_t)REG_TMP;
	byte_count +=2;

	uint16_t CRC16 = crc16(BUFFER, 0x5);

	buffer[5] = CRC16;
	buffer[6] = CRC16 >> 8;
	byte_count += 2;

	dma_start_transsmit(buffer, 7);
	return 0;
}



uint16_t read_holding_registers(uint8_t *buffer, RegsTable_TypeDef *REGS)
{
	PDU_Query_TypeDef *QueryPDU = ((PDU_Query_TypeDef *)buffer);

	uint16_t adr = reg_swap(QueryPDU->reg_addr);
	uint16_t cnt = reg_swap(QueryPDU->reg_count);

	uint16_t tm_crc = reg_swap(crc16(buffer, 6));
	if(QueryPDU->crc != tm_crc)
	{
		return MODBUS_EXCEPTION_MEMORY_PARITY;
	}

//Еслм сумма адрес плюс количество превышают колличество регистров, выдаем ошибку.
	if((adr + cnt) > INP_REG_COUNT)
	{
		return MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS;
	}
//Непосредственно читаем заполняем тело ответа
	buffer[2] = cnt * (sizeof(REGS->INP_REG[adr]));	//Определяем колличество регистров в исходящем сообщении.
	
	for(uint8_t i=0; i < buffer[2], i++)
	{
		
	}
	
	
	
	buffer[3]= (uint8_t)(REGS->INP_REG[adr] >> 24);
	buffer[4]= (uint8_t)(REGS->INP_REG[adr] >> 16);
	buffer[5]= (uint8_t)(REGS->INP_REG[adr] >> 8);
	buffer[6] = (uint8_t)REGS->INP_REG[adr];
	uint16_t CRC16 = crc16(buffer, 0x7);

	buffer[7] = CRC16;
	buffer[8] = CRC16 >> 8;

	dma_start_transsmit(buffer, 9);
	return 0;
}

uint8_t error_handler(uint8_t error, uint8_t *buffer)
{
	buffer[1] = 0x8F;
	buffer[2] = error;

	uint16_t CRC16 = crc16(buffer, 0x3);

	buffer[3] = CRC16;
	buffer[4] = CRC16 >> 8;
	dma_start_transsmit(buffer, 5);
	return 0;
}


//Табличный подсчет контрольной суммы.
uint16_t crc16(uint8_t *adr_buffer, uint32_t byte_cnt)
{
    /////////////////////////////////////////////////
    static unsigned char auchCRCHi[]=
    {
        0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
        0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
        0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
        0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
        0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
        0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
        0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
        0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
        0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
        0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
        0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
        0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
        0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
        0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
        0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
        0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
    };
    /*Table of CRC values for lowвЂ“order byte*/
    static char auchCRCLo[]=
    {
        0x00,0xC0,0xC1,0x01,0xC3,0x03,0x02,0xC2,0xC6,0x06,0x07,0xC7,0x05,0xC5,0xC4,0x04,
        0xCC,0x0C,0x0D,0xCD,0x0F,0xCF,0xCE,0x0E,0x0A,0xCA,0xCB,0x0B,0xC9,0x09,0x08,0xC8,
        0xD8,0x18,0x19,0xD9,0x1B,0xDB,0xDA,0x1A,0x1E,0xDE,0xDF,0x1F,0xDD,0x1D,0x1C,0xDC,
        0x14,0xD4,0xD5,0x15,0xD7,0x17,0x16,0xD6,0xD2,0x12,0x13,0xD3,0x11,0xD1,0xD0,0x10,
        0xF0,0x30,0x31,0xF1,0x33,0xF3,0xF2,0x32,0x36,0xF6,0xF7,0x37,0xF5,0x35,0x34,0xF4,
        0x3C,0xFC,0xFD,0x3D,0xFF,0x3F,0x3E,0xFE,0xFA,0x3A,0x3B,0xFB,0x39,0xF9,0xF8,0x38,
        0x28,0xE8,0xE9,0x29,0xEB,0x2B,0x2A,0xEA,0xEE,0x2E,0x2F,0xEF,0x2D,0xED,0xEC,0x2C,
        0xE4,0x24,0x25,0xE5,0x27,0xE7,0xE6,0x26,0x22,0xE2,0xE3,0x23,0xE1,0x21,0x20,0xE0,
        0xA0,0x60,0x61,0xA1,0x63,0xA3,0xA2,0x62,0x66,0xA6,0xA7,0x67,0xA5,0x65,0x64,0xA4,
        0x6C,0xAC,0xAD,0x6D,0xAF,0x6F,0x6E,0xAE,0xAA,0x6A,0x6B,0xAB,0x69,0xA9,0xA8,0x68,
        0x78,0xB8,0xB9,0x79,0xBB,0x7B,0x7A,0xBA,0xBE,0x7E,0x7F,0xBF,0x7D,0xBD,0xBC,0x7C,
        0xB4,0x74,0x75,0xB5,0x77,0xB7,0xB6,0x76,0x72,0xB2,0xB3,0x73,0xB1,0x71,0x70,0xB0,
        0x50,0x90,0x91,0x51,0x93,0x53,0x52,0x92,0x96,0x56,0x57,0x97,0x55,0x95,0x94,0x54,
        0x9C,0x5C,0x5D,0x9D,0x5F,0x9F,0x9E,0x5E,0x5A,0x9A,0x9B,0x5B,0x99,0x59,0x58,0x98,
        0x88,0x48,0x49,0x89,0x4B,0x8B,0x8A,0x4A,0x4E,0x8E,0x8F,0x4F,0x8D,0x4D,0x4C,0x8C,
        0x44,0x84,0x85,0x45,0x87,0x47,0x46,0x86,0x82,0x42,0x43,0x83,0x41,0x81,0x80,0x40,
    };
    ///////////////////////////////////////////////////////////// 

    // РїРµСЂРµРјРµРЅРЅС‹Рµ РґР»СЏ СЂР°СЃС‡РµС‚Р° CRC16 -?*:?????
    unsigned char uchCRCHi = 0xFF;  
    unsigned char uchCRCLo = 0xFF; 
    unsigned char uIndex;
    
    /* CRC Generation Function */
    while( byte_cnt--) /* pass through message buffer */
    {
        uIndex = uchCRCHi ^ *adr_buffer++; /* calculate the CRC */
        uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex];
        uchCRCLo = auchCRCLo[uIndex];
    }

    return (uchCRCHi << 8  | uchCRCLo);
}
