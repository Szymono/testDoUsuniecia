/* < Szymon Ostrowski - 19.09.2018 >
 * < CW10.1_TIM2_delay_ms - mian.c >
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

uint32_t x=1;

int main(void)
{
	RCC->APB2ENR = RCC_APB2ENR_IOPAEN;		//w³. portu GPIO A
	ConfigPin(GPIOA , PA3 , output_PP_LS);
	
	config_delay();
	BB(GPIOA->ODR, PB3) =1;

	while(1)
	{
		delay_ms(x);
		x++;
		BB(GPIOA->ODR, PB3) ^=1;
	}
}
