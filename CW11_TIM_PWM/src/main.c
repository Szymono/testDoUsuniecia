/* < Szymon Ostrowski - 18.09.2018 >
 * < CW11_TIM_PWM - main.c >
 * [ pwm ]
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
	RCC->APB2ENR = RCC_APB2ENR_IOPAEN | RCC_APB2ENR_TIM1EN;	//w³. portu GPIO A , enable TIM1

	ConfigPin(GPIOA , PA8 , alternate_PP_LS);
	ConfigPin(GPIOA , PA9 , output_PP_LS);

	TIM1->CCMR1 = (TIM_CCMR1_OC1PE | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2);	//w³. bufor dla CCR1 , 110: PWM mode 1
	TIM1->CCER = TIM_CCER_CC1E;		//enable CC1
	TIM1->BDTR = TIM_BDTR_MOE;		//reset break
	TIM1->PSC = 7200;			//prescaler
	TIM1->ARR = 9000;			//przepe³nienie licznika
	TIM1->CCR1 = 5000;			//wartoœc porównawcza
	TIM1->EGR = TIM_EGR_UG;		//wymuszenie UEV
	TIM1->DIER = TIM_DIER_UIE;	//Bit 0 UIE: Update interrupt enable
	TIM1->CR1 = TIM_CR1_ARPE | TIM_CR1_CEN ;	//Bit 0 CEN: Counter enable

	NVIC_EnableIRQ(TIM1_UP_IRQn);		//Enable interrupt TIM1_UP_IRQn

	while (1);
} /* main */

intpt void TIM1_UP_IRQHandler(void)
{
	if (TIM1->SR & TIM_SR_UIF)	//flaga przerwania zsetowana
	{
		TIM1->SR = ~TIM_SR_UIF;	//zerowanie flagi
		BB(GPIOA->ODR, PB9) ^=1; UART_send
	}
}
