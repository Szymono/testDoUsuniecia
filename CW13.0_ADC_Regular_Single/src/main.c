/* < Szymon Ostrowski - 03.10.2018 >
 * < CW13_ADC_Single_conversion - main.c >
 *
 * >
 * >
 */


#include <stdio.h>
#include "stm32f10x.h"
#include "LIB/Interrupt/intpt.h"
#include "LIB/Useful/useful.h"
#include "LIB/GPIO/gpio.h"



int main(void)
{
	RCC->APB2ENR = RCC_APB2ENR_IOPAEN | RCC_APB2ENR_ADC1EN;
	ConfigPin(GPIOA,PA0,analog);


		ADC1->CR2 = ADC_CR2_ADON;	//w³. przetwornika
		for(volatile uint32_t delay = 100000; delay; delay--);

		ADC1->SQR3 = 0;						//konfiguracja grupy regularnej - ustwienie kana³ów. W tym przypadku jeden kana³ ADC0 (na polu SQ1)
		ADC1->CR2 = ADC_CR2_ADON;			//programowe rozpoczêcie konwersji kana³ów grupy regularnej
		while( ! (ADC1->SR & ADC_SR_EOC) );	//oczekiwanie na 0 flagi EOC ("end of conversion")
}


