//CW1 blinking LED ~S.O.
#include <stdio.h>
#include "stm32f10x.h"


int main(void)
{
	volatile uint32_t x=0;

	RCC->APB2ENR = RCC_APB2ENR_IOPCEN;

	GPIOC->CRH &= ~(0b1111<<20); //Output
	GPIOC->CRH |= (0b101<<20);
	GPIOC->BSRR = GPIO_BSRR_BR13;

	while(1)
	{
		for(x=1; x<1000000; x++);
		GPIOC->BSRR = GPIO_BSRR_BS13;

		for(x=1; x<1000000; x++);
		GPIOC->BSRR = GPIO_BSRR_BR13;
	}

}

