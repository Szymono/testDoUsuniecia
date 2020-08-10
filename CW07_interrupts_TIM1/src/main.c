/*CW7 TIM1
 *
 * > PA3 - LED
 *
 */
#include <stdio.h>
#include "stm32f10x.h"
#include "LIB/Interrupt/intpt.h"
#include "LIB/Useful/useful.h"
#include "LIB/GPIO/gpio.h"

int main(void)
{
	RCC->APB2ENR = RCC_APB2ENR_IOPAEN | RCC_APB2ENR_TIM1EN;	//w³. portu GPIO A , enable TIM1

	ConfigPin(GPIOA , PA3 , output_PP_LS);
	BB(GPIOA->ODR, PA3) =0;

	TIM1->PSC = 7200;
	TIM1->ARR = 10000;				//Przepe³nienie licznika
	TIM1->DIER = TIM_DIER_UIE;		//Bit 0 UIE: Update interrupt enable
	TIM1->CR1 |= TIM_CR1_CEN;		//Bit 0 CEN: Counter enable

	NVIC_EnableIRQ(TIM1_UP_IRQn);		//Enable interrupt TIM1_UP_IRQn

	while (1);
}

intpt void TIM1_UP_IRQHandler(void)
{
 	if (TIM1->SR & TIM_SR_UIF)	//flaga przerwania zsetowana
	{
		TIM1->SR = ~TIM_SR_UIF;	//zerowanie flagi
		BB(GPIOA->ODR, PB3) ^=1;
	}
}

