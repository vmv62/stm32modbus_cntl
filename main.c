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

//Объявляем структуры таблицы регистров и структуры ПДУ.
PDU_TypeDef PDU;
RegsTable_TypeDef REGS;



int main(void)
{
	//Инициализируем члены сервисной структуры.
	Serv.byte_count = 0;	//Считаем принятые байты
	Serv.end_transmission = 0; //Флаг конца приема ПДУ.
	hdw_init();			//Инициализация переферии.
	dma_usart_config(&PDU, sizeof(PDU));
    while(1)
    {
    	//Обработчик состояния выставленного флага конца передачи.
    	if(Serv.end_transmission){
    		Serv.end_transmission = 0;
    		pase_pdu(&PDU, &REGS);
    		dma_start_transsmit(&PDU, sizeof(PDU));
    	}
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
	if(!(USART1->ISR & USART_ISR_RXNE) & (USART1->ISR & USRT_ISR_IDLE)) //Если условие выполнено то приемный буфер пуст и линия в простое.
	{
		USART1->ICR &= ~USART_ICR_IDLECF; 	//Сбрасываем флаг холостой линии.
		Serv.end_transmission = 1;		//Ставим флаг окончания приема пакета.
		Serv.byte_count = DMA1_Channel3->CNDTR  //Считываем колличество непринятых байт для вычисления принятых.
		DMA1_Channel3->CNDTR = 256; 		//Перезаписываем счетчик байтов ДМА канала.
	}
}

//Старая функция обработчика прерывания.
/*
	if(USART1->ISR & USART_ISR_RXNE){	//Прерывание "регистр приема не пуст"
		BUFFER[Serv.byte_count] = (uint8_t)(USART1->RDR);
		USART1->CR1 |= USART_CR2_RTOEN;
		USART1->CR1 |= USART_CR1_RTOIE;	//Включение прерывания по простою линии приема.
		Serv.byte_count++;			//Почему то не инкрементируется? Видимо переменная не видна.
	}
	if(USART1->ISR & USART_ISR_RTOF){	//Прерывание по простою линии приема
		USART1->CR1 &= ~USART_CR1_RTOIE;
		USART1->ICR |= USART_ICR_RTOCF; //Сброс флага прерывания по простою линии приема.
//		USART1->CR1 &= ~USART_CR1_RTOIE; //Отключаем прерывание по простою, чтобы оно не мешало.
//		USART1->RQR &= ~USART_RQR_RXFRQ;
		Serv.end_transmission = 1;		//Переменная инициализируется это работает.
	}
*/
