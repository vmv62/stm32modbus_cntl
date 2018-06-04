#include "hw_init.c"

uint32_t DMA_USART_init(uint32_t *buffer, uint32_t buffer_size){
	//2 канал ДМА назначен на USART переферию.
	RCC->AHBENR |= RCC_AHBENR_DMA1EN; 			//Включаем тактирование ДМА переферии.
	USART1->CCR3 |= USART1_CCR3_DMAT; 			//Разрешаем использовать DMA на передачу.
	USART1->CCR3 |= USART1_CCR3_DMAR;			//Разрешаем использовать DMA  на прием.
	DMA1_Channel2->CPAR = (uint32_t) (&(USART1->TDR)); 	//Записываем адрес регистра данных регистра для отправки.
	DMA1_Channel2->CMAR = (uint32_t)(buffer);		//Записываем адрес памяти откуда будут читаться данные.
	DMA1_Channel2->CNDTR = buffer_size;				//Заносим размер данных для отправки.
	DMA1_Channel2->CCR |= DMA_CCR_MINC | DMA_CCR_MSIZE_0 | DMA_CCR_PSIZE_0 | DMA_CCR_TEIE | DMA_CCR_TCIE ;	//Настройка режима. (прерывания, размеры).

	DMA1_Chanel3->CPAR = (uint32_t)(bufer);
	DMA1_Chanel3->CMAR = (uint23_t)(&(USART1->TDR));
	DMA1)Chanel3->CNDTR = buffer_size;

	DMA1_Channel3->CCR |= DMA_CCR_EN;			//Включаем ДМА
	//Настройка перывания для ДМА.
	//NVIC_EnableIRQ(DMA1_Channel1_IRQn); /* (1) */
	//NVIC_SetPriority(DMA1_Channel1_IRQn,0); /* (2) */
}

uint32_t DMA_USART_tx(){
//	if(DMA_BASE->ISR && DMA_BASE_ISR_TCIF){			//Если установлен флаг завершения передачи.
//		DMA_BASE->ISR &=!
//	}

	USART1->ICR &=!USART_ICR_TCCF;
	DMA1_Channel2->CCR |= DMA_CCR_EN;			//Включаем ДМА

//В прерывании по завершению передачи снять все флаги
