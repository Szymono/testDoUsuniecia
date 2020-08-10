/* CW9 Enkoder
 *
 * > obs³ugiwany przez timer
 * > po przepe³nieniu licznika LED zmienia stan
 *
 * > PA8 , PA9 - Enkoder
 * > PA3 - LED
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
	ConfigPin(GPIOA , PA8 , input_fl);
	ConfigPin(GPIOA , PA9 , input_fl);

	//TIM1->CCER = TIM_CCER_CC1P;	//odwroceni fazy
	//TIM1->CCMR1 = TIM_CCMR1_CC1S_1 | TIM_CCMR1_CC1S_1;	//filtrowanie
	TIM1->SMCR = TIM_SMCR_SMS_1;	//Encoder mode
	TIM1->ARR = 100;				//Przepe³nienie licznika
	TIM1->DIER = TIM_DIER_UIE;		//Bit 0 UIE: Update interrupt enable
	TIM1->CR1 = TIM_CR1_CEN | TIM_CR1_OPM;		//Bit 0 CEN: Counter enable

	//NVIC_ClearPendingIRQ(TIM1_UP_IRQn);	//???
	NVIC_EnableIRQ(TIM1_UP_IRQn);		//Enable interrupt TIM1_UP_IRQn

	while (1);
} /* main */

intpt void TIM1_UP_IRQHandler(void)
{
	if (TIM1->SR & TIM_SR_UIF)	//flaga przerwania zsetowana
	{
		TIM1->SR = ~TIM_SR_UIF;	//zerowanie flagi
		BB(GPIOA->ODR, PB3) ^=1;
	}
}


