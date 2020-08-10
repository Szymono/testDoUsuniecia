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
#include "LIB/Interrupt/ex.h"

#define intpt __attribute__((interrupt))

int main(void)
{
	RCC->APB2ENR = (RCC_APB2ENR_IOPAEN );	//w³. portu GPIO A

	ConfigPin(GPIOA , PA0 , input_PUD);
	ConfigPin(GPIOA , PA1 , input_PUD);
	ConfigPin(GPIOA , PA2 , output_PP_LS);
	ConfigPin(GPIOA , PA3 , output_PP_LS);

	SetPin(GPIOA , PA0);	//pull-up (dla przycisków)
	SetPin(GPIOA , PA1);	//pull-up

	ConfigExtInt(GPIOA , B0 , AFIO_EXTICR1_EXTI0_PA , EXTI_mode_RFT);
	ConfigExtInt(GPIOA , B1 , AFIO_EXTICR1_EXTI1_PA , EXTI_mode_RFT);

	while(1);
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
