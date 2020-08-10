/* < Szymon Ostrowski - 10.10.2018 >
 * < ZEW_TFT2 - main.c >
 *
 * >
 * >
 */

#include <stdio.h>
#include "stm32f10x.h"
#include "pins.h"
#include "registers.h"
#include "SPFD5408.h"

#include "LIB/Interrupt/intpt.h"
#include "LIB/Useful/useful.h"
#include "LIB/GPIO/gpio.h"
#include "LIB/Delay/delay.h"

uint8_t q;

void blink(uint8_t x);
void tlo(uint16_t kolor);

int main(void)
{
 	RCC->APB2ENR = RCC_APB2ENR_IOPAEN;
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

	config_delay();

	ConfigPin(GPIOB,RD,output_PP_MS);
	ConfigPin(GPIOB,WR,output_PP_MS);
	ConfigPin(GPIOB,RS,output_PP_MS);
	ConfigPin(GPIOB,CS,output_PP_MS);
	ConfigPin(GPIOB,RST,output_PP_MS);
	ConfigPin(GPIOC,PC13,output_PP_MS);

	blink(100);

	TFT_reset();
	TFT_init();
	TFT_fillScreen(0x0000);

	blink(100);

	TFT_putString(0,0,0xF800,4,"TEST",0xffff);
	delay_ms(2000);
	delay_ms(2000);
	while(1)
	{
		//tlo(0xF800);
		TFT_fillScreen(q);
		q++;
	}
}

void blink(uint8_t x)
{
	BB(GPIOC->ODR,PC13) = 0;
	delay_ms(x);
	BB(GPIOC->ODR,PC13) = 1;
	delay_ms(x);

}

void tlo(uint16_t kolor)
{
	uint8_t x=0;
	uint16_t y=0;
	while(y<320)
	{
		while(x<240)
		{
			TFT_setPixel(x,y,kolor);
			x++;
		}
		x=0;
		y++;
	}
	y=0;
}


