#define TRUE 1
#define FALSE 0
#define FLASH_START 0x08000000

typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;


//Команды протокола
enum{
	READ_COIL_STATUS = 0x01,	//чтение битов регистров выходов.
	READ_DESCRET_INPUTS,		//чтение битов регистров входов.
	READ_HOLDING_REGISTERS,		//чтение регистров харнения (настройки) 16 бит
	READ_INPUT_REGISTERS,		//чтение рагистров 16 бит (показания разных датчиков)
	FORCE_SINGLE_COIL,			//Установка бит выходов
	PRESET_SINGLE_REGISTER,		//запись в регистры настроек.
};

//Ошибки протокола
enum {
    MODBUS_EXCEPTION_ILLEGAL_FUNCTION = 0x01,
    MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS,
    MODBUS_EXCEPTION_ILLEGAL_DATA_VALUE,
    MODBUS_EXCEPTION_SLAVE_OR_SERVER_FAILURE,
    MODBUS_EXCEPTION_ACKNOWLEDGE,
    MODBUS_EXCEPTION_SLAVE_OR_SERVER_BUSY,
    MODBUS_EXCEPTION_NEGATIVE_ACKNOWLEDGE,
    MODBUS_EXCEPTION_MEMORY_PARITY,
    MODBUS_EXCEPTION_NOT_DEFINED,
    MODBUS_EXCEPTION_GATEWAY_PATH,
    MODBUS_EXCEPTION_GATEWAY_TARGET,
    MODBUS_EXCEPTION_MAX
};

//Таблица регистров контроллера
static typedef struct{
	uint16_t coil;
	uint16_t inputs;
	uint16_t holding_reg[5];
	uint16_t input_reg[5];
}Regs_TypeDef;

//Структура сообщения протокола.
typedef struct{
	uint8_t slave_addres;
	uint8_t command;
	uint8_t body[500];
}PDU_TypeDef;

void regs_filling(Regs_TypeDef *reg_t);
uint16_t read_input_registers(uint16_t reg_addr, uint16_t count, uint16_t *dest);
uint16_t read_holding_registers(uint16_t reg_addr, uint16_t count, uint16_t *dest);
