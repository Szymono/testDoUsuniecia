/*CW4 Zew. przerwania
 *
 * > PA0 - przycisk
 * > PA1 - przycisk
 * > PA2 - LED
 * > PA3 - LED
 *
 */
#include <stdio.h>
#include "stm32f10x.h"
#include "LIB/lib.h"

#define intpt __attribute__((interrupt))

int main(void)
{
	RCC->APB2ENR = (RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN);	//w³. portu GPIO A

	ConfigPin(GPIOA , PA0 , input_PP);
	ConfigPin(GPIOA , PA1 , input_PP);
	ConfigPin(GPIOA , PA2 , output);
	ConfigPin(GPIOA , PA3 , output);

	SetPin(GPIOA , PA0);		//pull-up
	SetPin(GPIOA , PA1);		//pull-up

	//definicje portu z kt. realizowane bêdzie przerwanie
	// 0 - port A
	// 1 - port A
	AFIO->EXTICR[0] = AFIO_EXTICR1_EXTI0_PA;
	AFIO->EXTICR[0] = AFIO_EXTICR1_EXTI1_PA;

	//External interrupt init.
	EXTI ->IMR = (PA0 | PA1);	//Interrupt mask
	EXTI ->FTSR = (PA0 | PA1);	//Falling trigger
	NVIC_EnableIRQ(EXTI0_IRQn);
	NVIC_EnableIRQ(EXTI1_IRQn);

	while(1)
	{

	}

}

intpt void EXTI0_IRQHandler(void)
{
	if ( EXTI->PR & PC0)	//je¿eli na pin 0 wystêpujê przerwanie
	{
		EXTI->PR = PC0;	//kasownie flagi przerwania na danym pinie A-G
		BB(GPIOA->ODR , PA2) ^= 1;
	}
}


intpt void EXTI1_IRQHandler(void)
{
	if ( EXTI->PR & PC1)	//je¿eli na pin 0 wystêpujê przerwanie
	{
		EXTI->PR = PC1;	//kasownie flagi przerwania na danym pinie A-G
		BB(GPIOA->ODR , PA3) ^= 1;
	}
}
