/* < Szymon Ostrowski - 22.10.2018 >
 * < CW16_TFT_Touchscreen - main.c >
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

char buffer[10];
uint8_t i=0;
volatile uint16_t x=0 , y=0;


void TC_trig();
void TC_getXY();
void TFT_gpio_init();

int main(void)
{
	//Zegary enable
	RCC->APB2ENR =
			RCC_APB2ENR_IOPAEN | 	//GPIOA
			RCC_APB2ENR_IOPBEN | 	//GPIOB
			RCC_APB2ENR_IOPCEN | 	//GPIOC
			RCC_APB2ENR_ADC1EN | 	//ADC1
			RCC_APB2ENR_AFIOEN;		//External interrupt

	config_delay();

	//ADC Config
	ADC1->CR2 = ADC_CR2_JEXTSEL | ADC_CR2_JEXTTRIG; //Software control bit , w�. ADC1
		delay_ms(100);
		ADC1->JSQR |= 0<<15;	//kana� 1 - PA0
	ADC1->CR2 |= ADC_CR2_ADON;

	//init TFT
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN;
	TFT_gpio_init();
	TFT_reset();
	TFT_init();
	TFT_fillScreen(0x05a4);
	TC_trig();

	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN | RCC_APB2ENR_AFIOEN;
	ConfigPin(GPIOC,PC13,output_PP_MS);

	//External interrupt init.
	AFIO->EXTICR[0] = AFIO_EXTICR1_EXTI0_PA;
	EXTI ->IMR = (PA0);			//Interrupt mask
	EXTI ->FTSR = (PA0);		//Falling trigger
	NVIC_EnableIRQ(EXTI0_IRQn);

	TFT_gpio_init();
	TFT_fillScreen(0x05B4);

	while(1)
	{
		SetPin(GPIOC,PC13);

		sprintf (buffer, "adc_read X = %d   ", x);
		TFT_putString(0,10,0xFfff,1,buffer,0x05B4);
		sprintf (buffer, "adc_read Y = %d   ", y);
		TFT_putString(0,0,0xFfff,1,buffer,0x05B4);

		TC_trig();
		delay_ms(100);
		TFT_gpio_init();
	}
}


intpt void EXTI0_IRQHandler(void)
{
	if ( EXTI->PR & PA0)	//je�eli na pin 0 wyst�puj� przerwanie
	{EXTI->PR = PA0;	//kasownie flagi przerwania na danym pinie A-G
		delay_ms(20);
		if(BB(GPIOA->IDR,PA0) == 0)
		{
			TC_getXY();
			TC_trig();
		}
	}
}


void TC_getXY()
{
	//X
	ConfigPin(GPIOB,YP,analog);
	ConfigPin(GPIOB,XM,output_PP_MS);
	ConfigPin(GPIOA,XP,output_PP_MS);
	ConfigPin(GPIOA,YM,input_fl);
	BB(GPIOA->ODR,XP) = 1;
	BB(GPIOB->ODR,XM) = 0;

	ADC1->JSQR |= 8<<15;	//kana� 1 - PB0
	ADC1->CR2 |= ADC_CR2_JSWSTART;
	delay_ms(1);
	x = ADC1->JDR1;


	//Y
	ConfigPin(GPIOB,YP,output_PP_MS);
	ConfigPin(GPIOB,XM,input_fl);
	ConfigPin(GPIOA,XP,analog);
	ConfigPin(GPIOA,YM,output_PP_MS);
	//BB(GPIOA->ODR,XP) = 0;
	BB(GPIOB->ODR,YP) = 1;
	BB(GPIOA->ODR,YM) = 0;

	ADC1->JSQR = 0;		//kana� 1 - PA0
	ADC1->CR2 |= ADC_CR2_JSWSTART;
	delay_ms(1);
	y = ADC1->JDR1;
}

void TC_trig()
{
	ConfigPin(GPIOB,YP,input_fl);
	ConfigPin(GPIOB,XM,input_fl);
	ConfigPin(GPIOA,XP,input_PUD);
	ConfigPin(GPIOA,YM,output_PP_MS);
	BB(GPIOA->ODR,XP) = 1;
	BB(GPIOA->ODR,YM) = 0;
	EXTI->PR = PA0;	//kasownie flagi przerwania na danym pinie A-G
	NVIC_EnableIRQ(EXTI0_IRQn);
}

void TFT_gpio_init()
{
	NVIC_DisableIRQ(EXTI0_IRQn);
	ConfigPin(GPIOB,RS,output_PP_MS);
	ConfigPin(GPIOB,CS,output_PP_MS);
	DATA_OUT;
}


