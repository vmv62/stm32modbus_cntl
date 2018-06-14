#include "stm32f0xx.h"
#include "modbus.h"
#include "hw_init.h"

void hdw_init();
int get_adc();
void dma_start_transsmit(uint8_t *buffer, uint16_t buffer_len);
void dma_usart_config(uint8_t *buffer, uint16_t buffer_len);


//Структура для хранения сервиных регистров.
static struct {
	uint8_t end_transmission;
	uint8_t idle_simbol_count;
	uint32_t byte_count;
	uint32_t max_size_pdu;
	uint32_t current_size_pdu;
}Serv;

//Объявляем структуры таблицы регистров и структуры ПДУ.
PDU_TypeDef PDU;
RegsTable_TypeDef REGS;



int main(void)
{
	//Инициализируем члены сервисной структуры.
	Serv.byte_count = 0;	//Считаем принятые байты
	Serv.end_transmission = 0; //Флаг конца приема ПДУ.
	Serv.max_size_pdu = sizeof(PDU_TypeDef);
	hdw_init();			//Инициализация переферии.
	dma_usart_config(&PDU, sizeof(PDU));
    while(1)
    {
    	regs_filling(&REGS);
    	//Обработчик состояния выставленного флага конца передачи.
    	if(Serv.end_transmission){
    		Serv.end_transmission = 0;
    		pase_pdu(&PDU, &REGS);
  //  		dma_start_transsmit(&PDU, sizeof(PDU));
    	}
    	asm("CPSIE i");
    	regs_filling(&REGS);
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
		USART1->CR1 &= ~USART_CR1_RXNEIE;
		DMA1_Channel3->CPAR = (uint32_t)(&(USART1->RDR));
		DMA1_Channel3->CMAR = (uint32_t)(&PDU);
		DMA1_Channel3->CNDTR = 256;
		DMA1_Channel3->CCR |= DMA_CCR_EN;
		USART1->RTOR = 0x6;
		USART1->ICR |= USART_ICR_RTOCF;
		USART1->CR1 |= USART_CR1_RTOIE;
	}

	if(USART1->ISR & USART_ISR_RTOF)
	{
		USART1->ICR |= USART_ICR_RTOCF;
		USART1->CR1 |= USART_CR1_RXNEIE;
		DMA1_Channel3->CCR &= ~DMA_CCR_EN;
		USART1->CR1 &= ~USART_CR1_RTOIE;
		Serv.end_transmission = 1;
		asm("CPSID i");
	}
}
