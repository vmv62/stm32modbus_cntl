#include "stm32f0xx.h"
#include "modbus.h"
#include "hw_init.h"

void hdw_init();
int get_adc();
void dma_start_transsmit(uint8_t *buffer, uint16_t buffer_len);
void dma_usart_config(uint8_t *buffer, uint16_t buffer_len);




//Объявляем структуры таблицы регистров и структуры ПДУ.
RegsTable_TypeDef REGS;
HDWS_TypeDef HW;
uint32_t cnt=0;


int main(void)
{
	//Инициализируем регистр флагов
	HW.STATE = 0;


	hdw_init();			//Инициализация переферии.
	dma_usart_config(BUFFER, sizeof(BUFFER));

	while(1)
    {
    	regs_filling(&REGS);
    	//Обработчик состояния выставленного флага конца передачи.
    	if(HW.STATE & HDWSTATE_MRE){
    		pase_pdu(BUFFER, &REGS);
    		HW.STATE &= ~HDWSTATE_MRE;
    	}

    }
}


void USART1_IRQHandler(void){
	if(USART1->ISR & USART_ISR_RXNE)					//(USART1->ISR & USART_ISR_RTOF)
	{
		BUFFER[HW.RECV_BYTE_CNT] = (uint8_t)USART1->RDR;
		HW.RECV_BYTE_CNT++;
		USART1->ICR |= USART_ICR_RTOCF;	//Очистка флага прерывания по простою приемника
		USART1->RTOR = 0x3;					//Время до наступления прерывания при простое линии
		USART1->CR2 |= USART_CR2_RTOEN;	//Включение прерывания по паузе  приема
	}

	if(USART1->ISR & USART_ISR_RTOF)
	{
		USART1->ICR |= USART_ICR_RTOCF;	//Очистка флага по прерыванию простоя линии
		USART1->CR2 &= ~USART_CR2_RTOEN;	//Отключаем прерывание паузы приема
		USART1->CR1 &= ~USART_CR1_RXNEIE;	//Выключаем прерывание по приему байта
//		TIM17->ARR = 57535 - 1;
//		TIM17->CR1 |= TIM_CR1_CEN;
		HW.STATE |= HDWSTATE_MRE;
		HW.RECV_BYTE_CNT = 0;
	}
}

void DMA1_Channel2_3_IRQHandler(void)
{
//	TIM17->CR1 &= ~TIM_CR1_CEN;
	USART1->CR1 |= USART_CR1_RXNEIE;
	DMA1->IFCR |= DMA_IFCR_CTCIF2;
	DMA1->IFCR |= DMA_IFCR_CGIF2;
	DMA1->IFCR |= DMA_IFCR_CHTIF2;
}

/*
void TIM17_IRQHandler(void)
{
	USART1->CR1 |= USART_CR1_RXNEIE;
	TIM17->SR &= ~TIM_SR_UIF; //Сбрасываем флаг UIF
}
*/
