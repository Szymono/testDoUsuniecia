/* < Szymon Ostrowski - 04.10.2018 >
 * < CW14_ADC_Continuous - main.c >
 *
 * > ADC1 - Continuous mode (przez TIM1)  & Injected mode
 * > Pomiar z PA1 ci�g�y
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

	RCC->APB2ENR = RCC_APB2ENR_IOPAEN | RCC_APB2ENR_ADC1EN; //w�. zegar�w ADC, TIM1, GPIOA
	ConfigPin(GPIOA,PA9,analog);


	//ADC Config
	ADC1->CR2 = ADC_CR2_ADON | ADC_CR2_CONT; //w�. ADC1 , tryb ci�g�y

	delay_ms(100);
	ADC1->SQR3 = 9;				//set kana��w - ustawiony 1 kana� PA1 (MAX 4 kana�y , przy trybie Injected)




	ADC1->CR2 |= ADC_CR2_ADON;	//w�. konwersji
	while( ! (ADC1->SR & ADC_SR_EOC) );

	while(1)
	{
		delay_ms(100);
	}

}

