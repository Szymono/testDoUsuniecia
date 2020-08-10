/* < Szymon Ostrowski - 24.10.2018 >
 * < CW14.1_ADC_Injected - main.c >
 *
 * > ADC1 - Injected mode & Single conversion
 * > Pomiar z PA0 , PB0 co 100ms
 */

#include <stdio.h>
#include "stm32f10x.h"
#include "LIB/Interrupt/intpt.h"
#include "LIB/Useful/useful.h"
#include "LIB/GPIO/gpio.h"
#include "LIB/Delay/delay.h"


int main(void)
{
	RCC->APB2ENR = RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_ADC1EN; //w�. zegar�w ADC , GPIOA
	config_delay();
	ConfigPin(GPIOA,PA0,analog);
	ConfigPin(GPIOB,PB0,analog);

	//ADC Config
	ADC1->CR1 = ADC_CR1_JDISCEN;	//w�, wi�cej ni� 1 konwersji. Konwersje wykonywane pojedy�czo , pokolej
	ADC1->CR2 = ADC_CR2_JEXTSEL | ADC_CR2_JEXTTRIG | ADC_CR2_ADON;	//software trig enable , w�. ADC1
	delay_ms(100);
	ADC1->JSQR = 1<<20;	//set kana��w , MAX 4 kana�y (konwersje) w grupie injected
	ADC1->JSQR |= 0<<15;	//2 kana� PA0
	ADC1->JSQR |= 8<<10;	//1 kana� PB0

	while(1)
	{
		delay_ms(100);
		ADC1->CR2 |= ADC_CR2_JSWSTART;
	}

}

