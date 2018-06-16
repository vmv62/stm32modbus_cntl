#define TRUE 1
#define FALSE 0
#define FLASH_START 0x08000000
#define MAX_PDU_SIZE	254
#define MDB_ADDR		25
#define PDU_HEAD_SIZE	3
#define COIL_REG_SIZE	16
//Контрольные флаги(настройки)
#define COILS_HDW	((uint16_t)0x0001)
#define INPUTS_HDW	((uint16_t)0x0002)
#define reg_swap(a)		((a >> 8) | (a << 8))


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
    MODBUS_EXCEPTION_MAX,
    MODBUS_ILLEGAL_SLAVE_ADDR
};

uint8_t BUFFER[256];

//Наполнение таблицы. часть касаемая содержания регистров хранения.
typedef struct{
	uint16_t CONT_FLAG;
	uint16_t VAR_COIL;
}HoldingRegs_TypeDef;

//Таблица регистров контроллера
typedef struct{
	uint16_t COILS;
	uint16_t INPUTS;
	HoldingRegs_TypeDef HOLD;
	uint16_t INP_REG[5];
}RegsTable_TypeDef;

//Структура сообщения протокола.
typedef struct{
	uint8_t slave_addres;
	uint8_t command;
	uint8_t RA_HI;
	uint8_t RA_LO;
	uint8_t DB_HI;
	uint8_t DB_LO;
	uint8_t body[MAX_PDU_SIZE];
}PDU_TypeDef;

typedef struct{
	uint8_t slave_addres;
	uint8_t	command;
	uint16_t reg_addr;
	uint16_t reg_count;
	uint16_t crc;
}PDU_Query_TypeDef;

uint16_t pase_pdu(PDU_TypeDef *PDU, RegsTable_TypeDef *REGS);
uint16_t regs_filling(RegsTable_TypeDef *REGS);
uint16_t read_coils(uint8_t *buffer, RegsTable_TypeDef *REGS, uint16_t adress, uint16_t num);
uint16_t read_input_registers(uint8_t *buffer, RegsTable_TypeDef *REGS, uint16_t adress, uint16_t num);
uint16_t read_holding_registers(uint8_t *buffer, RegsTable_TypeDef *REGS, uint16_t adress, uint16_t num);
uint16_t crc16(uint8_t *adr_buffer, uint32_t byte_cnt);
