#define TRUE 1
#define FALSE 0
#define FLASH_START 0x08000000

//Макросы
#define reg_swap(a)		((a >> 8) | (a << 8))

#define MAX_PDU_SIZE	254
#define MDB_ADDR	25
#define PDU_HEAD_SIZE	3
#define COIL_REG_COUNT	16
#define INP_REG_COUNT	5
#define CRC_BYTE_CNT	2
//Контрольные флаги(настройки)
#define COILS_HDW	((uint16_t)0x0001) 			//читаем флаги аппаратных регистров или програмных регистров.
#define BTEMP_EN	((uint16_t)0x0002)			//Включение чтения внутреннего датчика температуры.
#define LSENS_EN	((uint16_t)0x0004)			///Включение/отключение датчика света.


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
    MODBUS_EXCEPTION_ILLEGAL_FUNCTION = 0x01,	//Принятый код функции не может быть обработан
    MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS,		//Адрес данных, указанный в запросе, недоступен.
    MODBUS_EXCEPTION_ILLEGAL_DATA_VALUE,		//Значение, содержащееся в поле данных запроса, является недопустимой величиной.
    MODBUS_EXCEPTION_SLAVE_OR_SERVER_FAILURE,	//
    MODBUS_EXCEPTION_ACKNOWLEDGE,				// Ведомое устройство приняло запрос и обрабатывает его, но это требует много времени. Этот ответ предохраняет ведущее устройство от генерации ошибки тайм-аута
    MODBUS_EXCEPTION_SLAVE_OR_SERVER_BUSY,		//Ведомое устройство занято обработкой команды. Ведущее устройство должно повторить сообщение позже, когда ведомое освободится.
    MODBUS_EXCEPTION_NEGATIVE_ACKNOWLEDGE,		//Ведомое устройство не может выполнить программную функцию, заданную в запросе. Этот код возвращается для неуспешного программного запроса, использующего функции с номерами 13 или 14. Ведущее устройство должно запросить диагностическую информацию или информацию об ошибках от ведомого.
    MODBUS_EXCEPTION_MEMORY_PARITY,				//Ведомое устройство при чтении расширенной памяти обнаружило ошибку контроля четности.
    MODBUS_EXCEPTION_NOT_DEFINED,
    MODBUS_EXCEPTION_GATEWAY_PATH,
    MODBUS_EXCEPTION_GATEWAY_TARGET,
    MODBUS_EXCEPTION_MAX,
    MODBUS_ILLEGAL_SLAVE_ADDR
};

uint8_t BUFFER[256];

//Наполнение таблицы. часть касаемая содержания регистров хранения.
typedef struct{
	uint16_t CONT_FLAG;	//флаги настроек функций (включено 1/ выключено 0)
	uint16_t VAR_COIL;
	uint16_t SLAVE_ADRES;
}HoldingRegs_TypeDef;

//Таблица регистров контроллера
typedef struct{
	uint16_t COILS;						//выхода - читаем командой 01, пишем командой 05
	uint16_t INPUTS;					//дискретные входа, читаем командой 02
	HoldingRegs_TypeDef HOLD;			//регистры хранения, читаем через комманду 04, пишем командой 06
	uint32_t INP_REG[INP_REG_COUNT]; //только читаем через команду 04
}RegsTable_TypeDef;

//Структура сообщения протокола.
typedef struct{
	uint8_t slave_addres;
	uint8_t	command;
}PDU_QueryHead_TypeDef;

typedef struct{
	PDU_QueryHead_TypeDef head;
	uint16_t reg_addr;
	uint16_t reg_count;
	uint16_t crc;
}PDU_Query1_4_TypeDef;

uint16_t pase_pdu(uint8_t *buffer, RegsTable_TypeDef *REGS);
uint16_t regs_filling(RegsTable_TypeDef *REGS);
uint16_t read_coils(uint8_t *buffer, RegsTable_TypeDef *REGS);
uint16_t read_input_registers(uint8_t *buffer, RegsTable_TypeDef *REGS);
uint16_t read_holding_registers(uint8_t *buffer, RegsTable_TypeDef *REGS);
uint8_t error_handler(uint8_t error, uint8_t *buffer);
uint16_t crc16(uint8_t *adr_buffer, uint32_t byte_cnt);
