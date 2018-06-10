#include "stm32f0xx.h"
#include "modbus.h"

void hdw_init();
int get_adc();
void dma_start_transsmit(uint8_t *buffer, uint16_t buffer_len);
void dma_usart_config(uint8_t *buffer, uint16_t buffer_len);



int main(void){

unsigned int a =0;
PDU_TypeDef PDU;
	hdw_init();
	dma_usart_config(&PDU, sizeof(PDU));
	byte_count = 0;
    while(1)
    {
    	//dma_start_transsmit(buffer, sizeof(buffer));
    	if(transmission_end){
    		dma_start_transsmit(&PDU, sizeof(PDU));
    	}
    	a++;
    }
}

int get_adc(){
	ADC1->CHSELR |= ADC_CHSELR_CHSEL16;  //Select chanel where sensor connected
//	ADC1->SMPR |= ADC_SMPR_SMP_0 | ADC_SMPR_SMP_1 | ADC_SMPR_SMP_2;
	ADC->CCR |= ADC_CCR_TSEN;			//Enable temperature sensor
	ADC1->CR |= ADC_CR_ADSTART;			//starting adc converting
	while(!(ADC1->ISR && ADC_ISR_EOC));

	return ADC1->DR;

}

void hdw_init(){

//ADC config
//	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN; //Enable clock to ADC
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN; //Enable clock to Usart
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;
	//ADC setup
	ADC1->ISR |=(ADC_ISR_ADRDY); //Clear ADC ready flag
	ADC1->CR |=(ADC_CR_ADEN); //ADC on
//	while(!(ADC1->ISR && ADC_ISR_ADRDY)){} //wait while ADC calibrate
	asm("CPSIE i");
	NVIC->ISER |= 1<<USART1_IRQn;
}

void dma_usart_config(uint8_t *buffer, uint16_t buffer_len){
	//GPIO config
	GPIOA->MODER |= GPIO_MODER_MODER9_1 | GPIO_MODER_MODER10_1;  			//Íàñòðîéêà âõîä/âûõîä. Àëüòåðíàòèâíûå ôóíêöèè
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR9 | GPIO_OSPEEDER_OSPEEDR10;  	//Ðàáîòà â âûñîêîñêîðîñòíîì ðåæèìå
	GPIOA->AFR[1] |= 0x110;													//Àëüòåðíàòèâíàÿ ôóíêöèÿ äëÿ GPIOA_9 GPIOA_10 ïîóìîë÷àíèþ íàñòðîåí íà ââîä/âûâîä
	//UART config
	USART1->BRR = 0x0341;	//Íàñòðîéêà ñêîðîñòè ïåðåäà÷è. 0õ0341 - 9600
	USART1->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_UE | USART_CR1_RXNEIE;  //Transmit enable, recive enable, usart enable
	USART1->RTOR = 50;	//Âðåìÿ îæèäàíèÿ áèòîâ ïîñëå ïðèåìà ïîñëåäíåãî.

	//DMA config
	USART1->CR3 |= USART_CR3_DMAT | USART_CR3_DMAR; 			//Ðàçðåøàåì èñïîëüçîâàòü DMA íà ïåðåäà÷ó.
	DMA1_Channel2->CPAR = (uint32_t)(&(USART1->TDR)); 	//Çàïèñûâàåì àäðåñ ðåãèñòðà äàííûõ ðåãèñòðà äëÿ îòïðàâêè.
	DMA1_Channel2->CMAR = (uint32_t)buffer;		//Çàïèñûâàåì àäðåñ ïàìÿòè îòêóäà áóäóò ÷èòàòüñÿ äàííûå.
	DMA1_Channel2->CNDTR = buffer_len;				//Çàíîñèì ðàçìåð äàííûõ äëÿ îòïðàâêè.
	DMA1_Channel2->CCR |= DMA_CCR_DIR | DMA_CCR_MINC | DMA_CCR_TEIE | DMA_CCR_TCIE;	//Íàñòðîéêà ðåæèìà. (ïðåðûâàíèÿ, ðàçìåðû).

	DMA1_Channel3->CPAR = (uint32_t)(&(USART1->RDR)); 	//Àäðåñ ïåðåôåðèè
	DMA1_Channel3->CMAR = (uint32_t)(buffer);			//Àäðåñ áóôóðà äëÿ ïðèåìà.
	DMA1_Channel3->CNDTR = buffer_len;				//Ðàçìåð áóôåðà.
	DMA1_Channel3->CCR |=  DMA_CCR_MINC;	//Íàñòðîéêà ðåæèìà. (ïðåðûâàíèÿ, ðàçìåðû).
	DMA1_Channel3->CCR |= DMA_CCR_EN;		//Âêëþ÷àåì ÄÌÀ3
}

void dma_start_transsmit(uint8_t *buffer, uint16_t buffer_len){
	if(DMA1->ISR && DMA_ISR_TCIF2 ){
		DMA1->IFCR |= DMA_IFCR_CGIF2;
		DMA1_Channel2->CCR &= ~DMA_CCR_EN;
		DMA1_Channel2->CNDTR = buffer_len;
	}
	DMA1_Channel2->CCR |= DMA_CCR_EN; //Ïèíàåì ÄÌÀ äëÿ íà÷àëà ïåðåäà÷è.
}

void USART1_IRQHandler(void){
//1. Определить прием первого байта.				Есть
//2. Включить прерывание по простою линии приема.		Есть
//3. Отключить прерывание по риему байта.	
//4. При прерывании по простою линии выставить флаг прием завершен.
//5. Вычислить колличество принятых данных (в дма есть регистр с количеством данных которые осталось принять)
//6. В основном цикле передать адрес буфера и колличество байт парсеру ПДУ.
	
	if(USART1->ISR & USART_ISR_RXNE){	//Прерывание "регистр приема не пуст"
		USART1->ICR &= ~USART_ICR_
		USART1->CR1 |= USART_CR1_RTOIE;	//Включение прерывания по простою линии приема.
		USART1->CR1 |= USART_CR1_RXNEIE; //отключаем прерывание приемный регистр не пуст, чтобы не оттягивала работу на себя.
		byte_count++;			//Почему то не инкрементируется? Видимо переменная не видна.
	}
	if(USART1->ISR & USART_ISR_RTOIF{	//Прерывание по простою линии приема.
		USART1->ICR |= USART_ICR_RTOCF; //Сброс флага прерывания по простою линии приема.
		USART1->CR1 &= ~USART_CR1_RTOIE; //Отключаем прерывание по простою, чтобы оно не мешало.
		transmission_end = 1;		//Переменная инициализируется это работает.
	}
}

