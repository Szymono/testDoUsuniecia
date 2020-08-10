/*CW8 TIM1 , liczy zbocza opadaj¹ce przycisku
 *
 * > PA8 - przycisk
 * > PA2 - LED
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
	ConfigPin(GPIOA , PA8 , input_PUD);

	BB(GPIOA->ODR,PA8) = 1;

	TIM1->CCER = TIM_CCER_CC1P;
	TIM1->SMCR = (TIM_SMCR_SMS | TIM_SMCR_TS_2);
	TIM1->ARR = 3;
	TIM1->DIER = TIM_DIER_UIE;		//Bit 0 UIE: Update interrupt enable
	TIM1->CR1 = TIM_CR1_CEN;		//Bit 0 CEN: Counter enable
	TIM1->CCMR1 = TIM_CCMR1_IC1F;	//filtr

	NVIC_EnableIRQ(TIM1_UP_IRQn);	//Enable interrupt TIM1_UP_IRQn

	while (1);
} /* main */

__attribute__((interrupt)) void TIM1_UP_IRQHandler(void)
{
	if (TIM1->SR & TIM_SR_UIF)
	{
		TIM1->SR = ~TIM_SR_UIF;
		BB(GPIOA->ODR, PB3) ^=1;
	}
}
