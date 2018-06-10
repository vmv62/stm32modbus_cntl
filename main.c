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
	uint32_t byte_count;
}Serv;


int main(void){

unsigned int a =0;
PDU_TypeDef PDU;
	hdw_init();
	dma_usart_config(&PDU, sizeof(PDU));
    while(1)
    {
    	//Обработчик состояния выставленного флага конца передачи.
    	if(Serv.end_transmission){
    		Serv.end_transmission = 0;
    		dma_start_transsmit(&PDU, sizeof(PDU));
    	}
    	a++;
    }
}



void USART1_IRQHandler(void){
//1. Определить прием первого байта.				Есть
//2. Включить прерывание по простою линии приема.		Есть
//3. Отключить прерывание по риему байта.	
//4. При прерывании по простою линии выставить флаг прием завершен.
//5. Вычислить колличество принятых данных (в дма есть регистр с количеством данных которые осталось принять)
//6. В основном цикле передать адрес буфера и колличество байт парсеру ПДУ.
	
	if(USART1->ISR & USART_ISR_RXNE){	//Прерывание "регистр приема не пуст"
		USART1->CR1 |= USART_CR1_RTOIE;	//Включение прерывания по простою линии приема.
		USART1->CR1 |= USART_CR1_RXNEIE; //отключаем прерывание приемный регистр не пуст, чтобы не оттягивала работу на себя. Флаг сбрасывается при чтении приемного регистра.
		Serv.byte_count++;			//Почему то не инкрементируется? Видимо переменная не видна.
	}
	if(USART1->ISR & USART_ISR_RTOF){	//Прерывание по простою линии приема
		USART1->ICR |= USART_ICR_RTOCF; //Сброс флага прерывания по простою линии приема.
		USART1->CR1 &= ~USART_CR1_RTOIE; //Отключаем прерывание по простою, чтобы оно не мешало.
		Serv.end_transmission = 1;		//Переменная инициализируется это работает.
	}
}
