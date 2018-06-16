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
  //  		dma_start_transsmit(&PDU, sizeof(PDU));
    	}
  //  	asm("CPSIE i");
 //  	regs_filling(&REGS);
    }
}

//Проблеммы работы: прерывание по приему байта происходит, но по простою линии никак не хочет.
//При приеме, когда на линии появляется стартовый бит происходит поднятие флага USART_ISR_RXNE это означает что можно читать данные из регистра.
//При работе с ДМА флаг сбрасывается при каждом чтении ДМА данных из регистра РДР.
//Флаг должен быть очищен пред окончанием приема иначе ошибка переполнения.
//При обнаружении холостого хода происходит тоже самое что и при приеме, плюс выставляется флаг ИДЛ.
// При поднятии флага USART_ISR_RXNE происходит прерывание.
//При настройке бит включения поднимается в самую последнюю очередь.
//Ошибка переполнения происходит когда не сборшен флаг USART_ISR_RXNE и происходит прием следующего бита.

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
//		USART1->ICR |= USART_ICR_RTOCF;	//Очистка флага по прерыванию простоя линии
		USART1->CR2 &= ~USART_CR2_RTOEN;	//Отключаем прерывание паузы приема
		HW.STATE |= HDWSTATE_MRE;
        asm("CPSID i");
	}
}


