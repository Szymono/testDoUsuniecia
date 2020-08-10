/* < Szymon Ostrowski - 22.10.2018 >
 * < CW17_TFT_TouchScreen_TEST1 - main.c >
 *
 * >
 * >
 */

#include <stdio.h>
#include "stm32f10x.h"

#include "TFT/pins.h"
#include "TFT/registers.h"
#include "TFT/SPFD5408.h"

#include "LIB/Interrupt/intpt.h"
#include "LIB/Useful/useful.h"
#include "LIB/GPIO/gpio.h"
#include "LIB/Delay/delay.h"

uint8_t i=0;

void TC_trig();
void TFT_gpio_init();

int main(void)
{

config_delay();

//init TFT
RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN;
ConfigPin(GPIOC,PC13,output_PP_MS);
TFT_reset();
TFT_init();
TFT_fillScreen(0x05B4);

TC_trig();

	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN | RCC_APB2ENR_AFIOEN;

	ConfigPin(GPIOA,PA1,output_PP_MS);
	ConfigPin(GPIOA,PA0,input_PUD);
	ConfigPin(GPIOC,PC13,output_PP_MS);

	SetPin(GPIOA,PA0);
	ResetPin(GPIOA,PA1);

	//External interrupt init.
	AFIO->EXTICR[0] = AFIO_EXTICR1_EXTI0_PA;
	EXTI ->IMR = (PA0);	//Interrupt mask
	EXTI ->FTSR = (PA0);	//Falling trigger
	NVIC_EnableIRQ(EXTI0_IRQn);

	while(1)
	{/*
		TCinit();
		delay_ms(1);
		if (GPIOA->IDR & PA0)
		{
			ResetPin(GPIOC,PC13);
			TFT_gpio_init();
			TFT_fillScreen(0x0000);
		}
		else
		{
			SetPin(GPIOC,PC13);
			TFT_gpio_init();
			TFT_fillScreen(0x05B4);
		}
*/SetPin(GPIOC,PC13);


TFT_gpio_init();
TFT_fillScreen(0x05B4);
TC_trig();

delay_ms(100);
	}
}

intpt void EXTI0_IRQHandler(void)
{
	if ( EXTI->PR & PC0)	//je¿eli na pin 0 wystêpujê przerwanie
	{
		EXTI->PR = PC0;	//kasownie flagi przerwania na danym pinie A-G
		ResetPin(GPIOC,PC13);
		TFT_gpio_init();
		TFT_fillScreen(0x0000);
		delay_ms(1000);
		TC_trig();
	}
}




void TC_trig()
{
	ConfigPin(GPIOB,YP,input_fl);
	ConfigPin(GPIOB,XM,input_fl);
	ConfigPin(GPIOA,XP,input_PUD);
	ConfigPin(GPIOA,YM,output_PP_MS);
	BB(GPIOA->ODR,XP) = 1;
	BB(GPIOA->ODR,YM) = 0;
	EXTI ->FTSR = (PA0);	//Falling trigger
}

void TC_getX()
{
	ConfigPin(GPIOB,YP,analog);
	ConfigPin(GPIOB,XM,output_PP_MS);
	ConfigPin(GPIOA,XP,output_PP_MS);
	ConfigPin(GPIOA,YM,input_fl);
	BB(GPIOA->ODR,XP) = 1;
	BB(GPIOA->ODR,XM) = 0;
	EXTI ->FTSR = (PA0);	//Falling trigger
}

void TC_getY()
{
	ConfigPin(GPIOB,YP,output_PP_MS);
	ConfigPin(GPIOB,XM,input_fl);
	ConfigPin(GPIOA,XP,analog);
	ConfigPin(GPIOA,YM,output_PP_MS);
	BB(GPIOA->ODR,YP) = 1;
	BB(GPIOA->ODR,YM) = 0;
	EXTI ->FTSR = (PA0);	//Falling trigger
}

void TFT_gpio_init()
{
	EXTI ->FTSR = 0;	//Falling trigger
	ConfigPin(GPIOB,RS,output_PP_MS);
	ConfigPin(GPIOB,CS,output_PP_MS);
	DATA_OUT;
}
