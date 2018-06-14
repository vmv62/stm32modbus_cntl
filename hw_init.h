#include "stm32f0xx.h"

//#define __interrupt_enable()	(asm("CPSIE i"))
//#define __interrupt_disable()	(asm("CPSID i"))


typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;


void hdw_init();
void dma_usart_config(uint8_t *buffer, uint16_t buffer_len);
void dma_start_transsmit(uint8_t *buffer, uint16_t buffer_len);
void acp_init();
