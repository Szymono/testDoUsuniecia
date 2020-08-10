/* < Szymon Ostrowski - 26.10.2018 >
 * < TEST - main.c >
 *
 * >
 * >
 */

#include <stdio.h>
#include "stm32f10x.h"
#include "LIB/Interrupt/intpt.h"
#include "LIB/Useful/useful.h"
#include "LIB/GPIO/gpio.h"
#include "LIB/Delay/delay.h"

int main(void)
{
	//Zegary enable
	RCC->APB2ENR =
			RCC_APB2ENR_IOPAEN | 	//GPIOA
			RCC_APB2ENR_IOPBEN | 	//GPIOB
			RCC_APB2ENR_IOPCEN | 	//GPIOC
			RCC_APB2ENR_ADC1EN | 	//ADC1
			RCC_APB2ENR_AFIOEN;		//External interrupt

	//ConfigPin(GPIOA,PA0,analog);
	ConfigPin(GPIOA,PA0,3);
	BB(GPIOA->ODR,PA0) = 1;
	//External interrupt init.
	AFIO->EXTICR[0] = AFIO_EXTICR1_EXTI0_PA;
	EXTI ->IMR = (PA0);			//Interrupt mask
	EXTI ->FTSR = (PA0);		//Falling trigger
	NVIC_EnableIRQ(EXTI0_IRQn);

	while(1)
	{
		//NVIC_DisableIRQ(EXTI0_IRQn);

		EXTI ->IMR = 0;			//Interrupt mask

		BB(GPIOA->ODR,PA0) = 1;
		EXTI->PR = PA0;	//kasownie flagi przerwania na danym pinie A-G
		BB(GPIOA->ODR,PA0) = 0;

		EXTI ->IMR = (PA0);			//Interrupt mask

		//NVIC_ClearPendingIRQ(EXTI0_IRQn);
		//NVIC_EnableIRQ(EXTI0_IRQn);
	}
}

void EXTI0_IRQHandler(void)
{
	if ( EXTI->PR & PA0)	//je¿eli na pin 0 wystêpujê przerwanie

	{EXTI->PR = PA0;	//kasownie flagi przerwania na danym pinie A-G
		delay_ms(10);
		if(BB(GPIOA->IDR,PA0) == 0)
		{

		}
	}
}

