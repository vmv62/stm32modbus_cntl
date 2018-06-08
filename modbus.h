#define TRUE 1
#define FALSE 0
#define FLASH_START 0x08000000

typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;

uint8_t byte_count;
uint8_t transmission_end;

//Номера команд
enum{
	READ_COIL_STATUS = 0x01,	//Чтение флагов (один бит)
	READ_DESCRET_INPUTS,		//Чтение регистров входов (один бит)
	READ_HOLDING_REGISTERS,		//Чтение регистров хранения (настройки) (16 бит)
	READ_INPUT_REGISTERS,		//Чтение входных регистров(например значения датчик) (16 бит)
	FORCE_SINGLE_COIL,			//Запись значения одного флага
	PRESET_SINGLE_REGISTER,		//Запись в регистр хранения
};

//Ошибки
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

typedef struct{
	uint8_t slave_addres;
	uint8_t command;
	uint8_t body[500];
}PDU_TypeDef;

uint16_t read_input_registers(uint16_t reg_addr, uint16_t count, uint16_t *dest);
uint16_t read_holding_registers(uint16_t reg_addr, uint16_t count, uint16_t *dest);
