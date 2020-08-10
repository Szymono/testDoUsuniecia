/* < Szymon Ostrowski - 02.10.2018 >
 * < CW12_DMA - main.c >
 * [ DMA ]
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
	static char buf1[]="przykladowy tekst.";
	size_t size = sizeof(buf1);
	static volatile char buf2[30]="";

	RCC->APB2ENR = RCC_APB2ENR_IOPCEN;	//w³. portu GPIO A , enable TIM1
	ConfigPin(GPIOC , PC13 , output_PP_LS);
	BB(GPIOC->ODR, PC13) = 1;


	RCC->AHBENR |= RCC_AHBENR_DMA1EN;
	DMA1_Channel1->CMAR = (uint32_t)buf2;
	DMA1_Channel1->CPAR = (uint32_t)buf1;
	DMA1_Channel1->CNDTR = size;

	DMA1_Channel1->CCR = DMA_CCR1_MEM2MEM | DMA_CCR1_MINC | DMA_CCR1_PINC;
	DMA1_Channel1->CCR |= DMA_CCR1_EN;

	while( (DMA1->ISR & DMA_ISR_TCIF1) == 0 );
	if ( memcmp(buf1, buf2, 3) ) BB(GPIOC->ODR, PC13) = 0;
	else BB(GPIOC->ODR, PC13) = 0;
	while (1);
} /* main */





