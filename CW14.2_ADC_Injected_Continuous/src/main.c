/* < Szymon Ostrowski - 24.10.2018 >
 * < CW14.1_ADC_Injected - main.c >
 *
 * > ADC1 - Continuous mode & Injected mode
 * > Pomiar z PA1 ci¹g³y
 */


#include <stdio.h>
#include "stm32f10x.h"
#include "LIB/Interrupt/intpt.h"
#include "LIB/Useful/useful.h"
#include "LIB/GPIO/gpio.h"
#include "LIB/Delay/delay.h"


int main(void)
{
	config_delay();

	RCC->APB2ENR = RCC_APB2ENR_IOPAEN | RCC_APB2ENR_ADC1EN; //w³. zegarów ADC , GPIOA
	ConfigPin(GPIOA,PA0,analog);
	ConfigPin(GPIOA,PA1,analog);
	ConfigPin(GPIOA,PA2,analog);
	ConfigPin(GPIOA,PA3,analog);

	//ADC Config
	ADC1->CR1 |= ADC_CR1_JAUTO | ADC_CR1_JDISCEN;
	ADC1->CR2 =  ADC_CR2_ADON; //soft trig enable , w³. ADC1
	delay_ms(100);
	ADC1->JSQR = 3<<20;
	ADC1->JSQR |= 0<<15;		//set kana³ów - ustawiony 1 kana³ PA1 (MAX 4 kana³y , przy trybie Injected)
	ADC1->JSQR |= 1<<10;		//set kana³ów - ustawiony 1 kana³ PA1 (MAX 4 kana³y , przy trybie Injected)
	ADC1->JSQR |= 2<<5;		//set kana³ów - ustawiony 1 kana³ PA1 (MAX 4 kana³y , przy trybie Injected)
	ADC1->JSQR |= 3<<0;		//set kana³ów - ustawiony 1 kana³ PA1 (MAX 4 kana³y , przy trybie Injected)
	ADC1->CR2 |= ADC_CR2_ADON;
	while( ! (ADC1->SR & ADC_SR_EOC) );

	while(1)
	{
		delay_ms(100);
	}

}

