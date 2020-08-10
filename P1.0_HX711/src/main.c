/* < Szymon Ostrowski - 19.09.2018 >
 * < P1_HX711 - main.c >
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

#define DATA_PIN 	PB13
#define CLK_PIN 	PB14
#define LED_PIN 	PB15

uint32_t val[51];
float sr , *adr;

extern unsigned long ReadCount(void);

int main(void)
{
	RCC->APB2ENR = RCC_APB2ENR_IOPBEN;
	ConfigPin(GPIOB , LED_PIN , output_PP_HS);
	ConfigPin(GPIOB , CLK_PIN , output_PP_HS);
	ConfigPin(GPIOB , DATA_PIN , input_PUD);
	config_delay();


	BB(GPIOB->ODR , LED_PIN) = 1;
	BB(GPIOB->ODR , DATA_PIN) = 0;
	BB(GPIOB->ODR , CLK_PIN) = 1;

	delay_ms(450);
	ReadCount();
	delay_ms(450);

	adr = &sr;
	while(1)
	{
		for(int i=0 ; i<50 ; i++)
		{
			val[i] = ReadCount(); delay_ms(2);
		}

		for(int i=0 ; i<50 ; i++)
		{
			sr = (sr + val[i])/2;
		}
		delay_ms(200);
	}
}


extern unsigned long ReadCount(void)
{
uint8_t i;
uint32_t Count;

	BB(GPIOB->ODR , DATA_PIN) = 1;
	BB(GPIOB->ODR , CLK_PIN) = 0;
	Count = 0;
	delay_ms(10);

	while(GPIOB->IDR & DATA_PIN);     //oczekiwanie na stan niski na DATA (while , kiedy stan wysoki)
	delay_us(2);
	for (i=0;i<24;i++)
	{
		//pe³ny impuls lini zegarowej
		BB(GPIOB->ODR , CLK_PIN) = 1;
		BB(GPIOB->ODR , LED_PIN) = 1;
		delay_us(2);
		BB(GPIOB->ODR , CLK_PIN) = 0;
		BB(GPIOB->ODR , LED_PIN) = 0;

		Count = Count << 1;
		if(GPIOB->IDR & DATA_PIN)


		{
			Count++;
		}

		delay_us(10);
	}

	//1 pe³ny impuls lini zegarowej , dla ustawienia gain=128 oraz kana³u A
	BB(GPIOB->ODR , CLK_PIN) = 1;
	BB(GPIOB->ODR , LED_PIN) = 1;
	delay_us(2);
	BB(GPIOB->ODR , CLK_PIN) = 0;
	BB(GPIOB->ODR , LED_PIN) = 0;
	delay_us(2);




	Count = Count ^ 0x800000;
	return Count;
}
