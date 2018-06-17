#include "stm32f0xx.h"

//#define __interrupt_enable()	(asm("CPSIE i"))
//#define __interrupt_disable()	(asm("CPSID i"))
#define HDWSTATE_MRE	((uint32_t)0x00000001)	//message recieve start
#define HDWSTATE_MRS	((uint32_t)0x00000002)	//message recieve end


typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;

typedef struct{
	uint32_t STATE;
	uint8_t RECV_BYTE_CNT;
}HDWS_TypeDef;

void hdw_init();
void dma_usart_config(uint8_t *buffer, uint16_t buffer_len);
void dma_start_transsmit(uint8_t *buffer, uint16_t buffer_len);
void acp_init();
