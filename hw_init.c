#include "hw_init.h"


void dma_usart_config(uint8_t *buffer, uint16_t buffer_len){
	//Clock feeding enable
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN; //Enable clock to ADC
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN; //Enable clock to Usart
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;	//Включили тактирование ДМА
    RCC->APB2ENR |= RCC_APB2ENR_TIM17EN; //Включили тактирование таймера 6
//      while(!(ADC1->ISR && ADC_ISR_ADRDY)){} //wait while ADC calibrate
    asm("CPSIE i");				//Включение глобальных прерываний.
    NVIC->ISER |= 1<<USART1_IRQn;		//Включение прерываний от УСАРТА.
    NVIC->ISER |= 1<<DMA1_Channel2_3_IRQn;	//Включаем прерывание от ДМА
//        NVIC->ISER |= 1<<TIM17_IRQn;


 /*   //Калибровка преобразователя	ADCAL=1 по завершению калибровки бит снимается аппаратно.
    ADC1->CR |= ADC_CR_ADCAL;
    while(ADC1->CR & ADC_CR_ADCAL){;;}
    //Clear the ADRDY bit in ADC_ISR register by programming this bit to 1.
    //Выбираем канал для преобразования.
    ADC1->CHSELR |= ADC_CHSELR_CHSEL16;
    ADC1->CCR |= ADC_CCR_TSEN | ADC_CCR_VREFEN;
    //Set ADEN=1 in the ADC_CR register
    ADC1->CR |= ADC_CR_ADEN;
*/
    ADC1->CR |= ADC_CR_ADEN;

	//GPIO config
	GPIOA->MODER |= GPIO_MODER_MODER9_1 | GPIO_MODER_MODER10_1;  			//Назначение альтернативных функций для выводов (переназначение)
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR9 | GPIO_OSPEEDER_OSPEEDR10;  	//Максимальная скорость работы портов
	GPIOA->AFR[1] |= 0x110;													//Àëüòåðíàòèâíàÿ ôóíêöèÿ äëÿ GPIOA_9 GPIOA_10 ïîóìîë÷àíèþ íàñòðîåí íà ââîä/âûâîä
	//UART config
	USART1->BRR = 0x0341;	//скорость работы усарта (8000000/9600)
	USART1->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_UE | USART_CR1_RXNEIE | USART_CR1_RTOIE;//// ;  //Transmit enable, recive enable, usart enable
	USART1->CR3 |= USART_CR3_DMAT | USART_CR3_DMAR; 			//Включение ДМА на прием и передачу от усарта


	//DMA config
	DMA1_Channel2->CPAR = (uint32_t)(&(USART1->TDR)); 	//Адрес переферии для передачи
	DMA1_Channel2->CMAR = (uint32_t)buffer;		//Адрес буфера для чтения передаваемых данных
	DMA1_Channel2->CNDTR = buffer_len;				//Колличество передаваемых данных
	DMA1_Channel2->CCR |= DMA_CCR_DIR | DMA_CCR_MINC | DMA_CCR_TCIE;// DMA_CCR_TEIE ;	//Настройка канала ДМА(прерывания, направление передачи итд)

 // Включено после решения опять использовать ДМА.
//	DMA1_Channel3->CPAR = (uint32_t)(&(USART1->RDR)); 	//Канал 3 для приема данных (адрес переферии)
//	DMA1_Channel3->CMAR = (uint32_t)(buffer);			//Буфер для сохранения данных
//	DMA1_Channel3->CNDTR = buffer_len;				//Колличество сохраняемых данных
//	DMA1_Channel3->CCR |=  DMA_CCR_MINC;	//Включение инкрементирования адреса памяти
//	DMA1_Channel3->CCR |= DMA_CCR_EN;		//Включаем канал ДМА.

	//TIM6 configuration
//DMA1_Channel2_3_IRQHandler	TIM17->CR1 |= TIM_CR1_CEN; // Настраиваем делитель что таймер тикал 1000 раз в секунду
/*	TIM17->ARR = 57535 - 1;
	TIM17->PSC = 1000-1 ; // Чтоб прерывание случалось раз в секунду
	TIM17->CR1 |= TIM_CR1_OPM;
	TIM17->DIER |= TIM_DIER_UIE | TIM_CR1_ARPE ; //разрешаем прерывание от таймера
*/
}


void dma_start_transsmit(uint8_t *buffer, uint16_t buffer_len){
    DMA1_Channel2->CCR &= ~DMA_CCR_EN;
    DMA1_Channel2->CNDTR = buffer_len;
    DMA1_Channel2->CMAR = (uint32_t)buffer;
    DMA1_Channel2->CPAR = (uint32_t)(&(USART1->TDR));
    DMA1_Channel2->CCR |= DMA_CCR_EN;
}

uint32_t get_adc(uint32_t chanel){
	ADC1->CHSELR |= chanel; //ADC_CHSELR_CHSEL16;  //Select chanel where sensor connected
	ADC1->SMPR |= ADC_SMPR1_SMPR ;
	ADC->CCR |= ADC_CCR_TSEN;			//Enable temperature sensor
	ADC1->CR |= ADC_CR_ADSTART;			//starting adc converting
	while(!(ADC1->ISR && ADC_ISR_EOC));

//	int32_t temperature; /* will contain the temperature in degrees Celsius */
//	temperature = ((uint32_t) *TEMP30_CAL_ADDR
//	- ((uint32_t) ADC1->DR * VDD_APPLI / VDD_CALIB)) * 1000;
//	temperature = (temperature / AVG_SLOPE) + 30;

	return ADC1->DR;
}
