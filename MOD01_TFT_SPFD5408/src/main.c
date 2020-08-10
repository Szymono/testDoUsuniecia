/* < Szymon Ostrowski - 19.10.2018 >
 * < MOD01_TFT_SPFD5408 - main.c >
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

uint16_t pomiar=0;
char buffer[10];
float wynik;
uint16_t jedno=0;
uint16_t precyz=0;

int main(void)
{
	//init
 	RCC->APB2ENR = RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN;
 	config_delay();
	ConfigPin(GPIOB,RD,output_PP_MS);
	ConfigPin(GPIOB,WR,output_PP_MS);
	ConfigPin(GPIOB,RS,output_PP_MS);
	ConfigPin(GPIOB,CS,output_PP_MS);
	ConfigPin(GPIOB,RST,output_PP_MS);
	ConfigPin(GPIOC,PC13,output_PP_MS);

	//ADCinit
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_ADC1EN;	//w³. zegarów ADC, TIM1, GPIOA
	ConfigPin(GPIOA,PA8,analog);
	ADC1->CR2 = ADC_CR2_ADON | ADC_CR2_CONT; 					//w³. ADC1 , tryb ci¹g³y
	delay_ms(100);
	ADC1->SQR3 = 8;									//set kana³ów - ustawiony 1 kana³ PA1 (MAX 4 kana³y , przy trybie Injected)
	ADC1->CR2 |= ADC_CR2_ADON;						//w³. konwersji
	while( ! (ADC1->SR & ADC_SR_EOC) );


	TFT_reset();
	TFT_init();
	TFT_fillScreen(0x05B4);
	TFT_fullRect(45+5,140+5,150,45,0x0000);
	TFT_fullRect(45,140,150,45,0xffff);
	TFT_putString(58,151,0xF800,3,"x.xxx V",0xffff);

	while(1)
	{
		pomiar = ADC1->DR;
		wynik = (float)pomiar/1222.1212;
			//wynik = 3.54;
		jedno = (uint16_t)wynik;
		precyz = (wynik-jedno) * 1000;

		//sprintf(buffer, "%d", wynik);
		sprintf (buffer, "adc_read = %d.%d V", jedno, precyz);
		TFT_putString(0,0,0xFfff,1,buffer,0x05B4);

		sprintf (buffer, "%d.%d ", jedno, precyz);
		TFT_putString(58,151,0xF800,3,buffer,0xffff);


		delay_ms(300);
	}

}

