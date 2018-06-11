#include "hw_init.h"

void hdw_init(){

//ADC config
//      RCC->APB2ENR |= RCC_APB2ENR_ADC1EN; //Enable clock to ADC
        RCC->APB2ENR |= RCC_APB2ENR_USART1EN; //Enable clock to Usart
        RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
        RCC->AHBENR |= RCC_AHBENR_DMA1EN;
//      while(!(ADC1->ISR && ADC_ISR_ADRDY)){} //wait while ADC calibrate
        asm("CPSIE i");				//Включение глобальных прерываний.
        NVIC->ISER |= 1<<USART1_IRQn;		//Включение прерываний от УСАРТА.
}



void dma_usart_config(uint8_t *buffer, uint16_t buffer_len){
	//GPIO config
	GPIOA->MODER |= GPIO_MODER_MODER9_1 | GPIO_MODER_MODER10_1;  			//Назначение альтернативных функций для выводов (переназначение)
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR9 | GPIO_OSPEEDER_OSPEEDR10;  	//Максимальная скорость работы портов
	GPIOA->AFR[1] |= 0x110;													//Àëüòåðíàòèâíàÿ ôóíêöèÿ äëÿ GPIOA_9 GPIOA_10 ïîóìîë÷àíèþ íàñòðîåí íà ââîä/âûâîä
	//UART config
	USART1->BRR = 0x0341;	//скорость работы усарта (8000000/9600)
	USART1->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_UE | USART_CR1_RXNEIE | USART_CR1_IDLEIE;  //Transmit enable, recive enable, usart enable
	//USART1->RTOR = 10;	//Длительность изсерения простоя линии приемника перед генерацией прерывания
	USART1->CR3 |= USART_CR3_DMAT; 			//Включение ДМА на прием и передачу от усарта


	//DMA config
	DMA1_Channel2->CPAR = (uint32_t)(&(USART1->TDR)); 	//Адрес переферии для передачи
	DMA1_Channel2->CMAR = (uint32_t)buffer;		//Адрес буфера для чтения передаваемых данных
	DMA1_Channel2->CNDTR = buffer_len;				//Колличество передаваемых данных
	DMA1_Channel2->CCR |= DMA_CCR_DIR | DMA_CCR_MINC;// DMA_CCR_TEIE | DMA_CCR_TCIE;	//Настройка канала ДМА(прерывания, направление передачи итд)

 // Включено после решения опять использовать ДМА.
	DMA1_Channel3->CPAR = (uint32_t)(&(USART1->RDR)); 	//Канал 3 для приема данных (адрес переферии)
	DMA1_Channel3->CMAR = (uint32_t)(buffer);			//Буфер для сохранения данных
	DMA1_Channel3->CNDTR = buffer_len;				//Колличество сохраняемых данных
	DMA1_Channel3->CCR |=  DMA_CCR_MINC;	//Включение инкрементирования адреса памяти
	DMA1_Channel3->CCR |= DMA_CCR_EN;		//Включаем канал ДМА.

}


void dma_start_transsmit(uint8_t *buffer, uint16_t buffer_len){
        if(DMA1->ISR && DMA_ISR_TCIF2 ){
                DMA1->IFCR |= DMA_IFCR_CGIF2;
                DMA1_Channel2->CCR &= ~DMA_CCR_EN;
                DMA1_Channel2->CNDTR = buffer_len;
        }
        DMA1_Channel2->CCR |= DMA_CCR_EN; 
}
