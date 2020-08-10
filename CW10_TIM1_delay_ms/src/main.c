/* < Szymon Ostrowski - 14.09.2018 >
 * < CW10 - main.c >
 * [funkcja delay na TIM1]
 * >
 * >
 */

#include <stdio.h>
#include "stm32f10x.h"
#include "LIB/Interrupt/intpt.h"
#include "LIB/Useful/useful.h"
#include "LIB/GPIO/gpio.h"

void delay_ms(uint16_t ms);
void config_delay();

uint32_t x;
int main(void)
{
	RCC->APB2ENR = RCC_APB2ENR_IOPAEN;	//w³. portu GPIO A
	ConfigPin(GPIOA , PA3 , output_PP_LS);
	
	config_delay();
	BB(GPIOA->ODR, PB3) =1;

	delay_ms(1000);
	BB(GPIOA->ODR, PB3) ^=1;

	delay_ms(1000);
	BB(GPIOA->ODR, PB3) ^=1;

	delay_ms(1000);
	BB(GPIOA->ODR, PB3) ^=1;

		while(1);
}

void delay_ms(uint16_t ms)
{
	TIM1->ARR = 10000;
	TIM1->CR1 |= TIM_CR1_CEN;
	while(TIM1->CR1 & TIM_CR1_CEN);
}

void config_delay()
{

	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;		//enable TIM1
	TIM1->PSC = 7200;				//CPU frequency
	TIM1->ARR = 10000;				//Przepe³nienie licznika
	TIM1->CR1 |= (TIM_CR1_OPM | TIM_CR1_CEN | TIM_CR1_DIR);		//Bit 3 OPM: One pulse mode
}

