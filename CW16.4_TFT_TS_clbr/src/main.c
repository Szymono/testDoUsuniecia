 /* < Szymon Ostrowski - 29.10.2018 >
 * < CW16.4_TFT_TS_clbr - main.c >
 *
 * > Procesor STM32F103c8t6 , p³ytka STM32 blue pill
 * > Wyœwietlacz SPFD5408 240x320 + panel dotyk. rezystancyjny
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


void clbr();
void TC_getXY();
void gpio_TC();
void gpio_TFT();
void info(uint8_t x , uint8_t y);

#define LED_ON	BB(GPIOC-ODR,PC13) = 1;
#define LED_OFF	BB(GPIOC-ODR,PC13) = 0;
#define tlo		0x0020

volatile uint16_t
		Px1 , Py1 , Px2 , Py2 ,
		x_raw , y_raw ,
		x_tft , y_tft ,
		delta_x , delta_y;
float kx , ky;				//skala
char buffer[20];



int main(void)
{
	x_raw	= 0;
	x_tft	= 0;
	delta_x	= 0;
	kx		= 1;

	y_raw	= 0;
	y_tft	= 0;
	delta_y	= 0;
	ky		= 1;

	//Zegary enable
	RCC->APB2ENR =
			RCC_APB2ENR_IOPAEN | 	//GPIOA
			RCC_APB2ENR_IOPBEN | 	//GPIOB
			RCC_APB2ENR_IOPCEN | 	//GPIOC
			RCC_APB2ENR_ADC1EN | 	//ADC1
			RCC_APB2ENR_AFIOEN;		//External interrupt

	ConfigPin(GPIOC,PC13,output_PP_MS);	//onboard LED
	config_delay();

	//ADC Config
	ADC1->CR2 = ADC_CR2_JEXTSEL | ADC_CR2_JEXTTRIG; //Software control bit , w³. ADC1
	ADC1->JSQR |= 0<<15;		//kana³ 1 - PA0
	ADC1->CR2 |= ADC_CR2_ADON;	//w³. przetwornika

	//External interrupt init.
	AFIO->EXTICR[0] = AFIO_EXTICR1_EXTI0_PA;
	EXTI ->IMR = (PA0);			//Interrupt mask
	EXTI ->FTSR = (PA0);		//Falling trigger
	NVIC_EnableIRQ(EXTI0_IRQn);

	//Init TFT
	gpio_TFT();
	TFT_reset();
	TFT_init();
	TFT_fillScreen(tlo);
	clbr();

	gpio_TFT();
	TFT_fillScreen(tlo);
	gpio_TC();

NVIC_DisableIRQ(EXTI0_IRQn);
	while(1)
	{
		gpio_TC();
		while(BB(GPIOA->IDR,PA0) == 1);
		TC_getXY();
		gpio_TFT();
		//info(50,50);
		TFT_fillCircle(x_tft,y_tft,3,0xF800);
		//gpio_TC();
		//delay_ms(20);
	}
}


void EXTI0_IRQHandler(void)
{
	if ( EXTI->PR & PA0)	//je¿eli na pinie PA0 wyst¹pi zbocze opadaj¹ce (wciœniêcie panelu dotyk.)
	{EXTI->PR = PA0;	//kasownie flagi przerwania na danym pinie A-G
		delay_ms(10);
		if(BB(GPIOA->IDR,PA0) == 0)
		{
			TC_getXY();
			gpio_TC();
		}
	}
}

void clbr()
{
	gpio_TFT();
	TFT_fillScreen(0x0000);
	TFT_putString(83,150,0xFFFF,1,"KALIBRACJA",0x0000);
	delay_ms(500);

	//pkt. 1 , delta
	TFT_putString(83,160,0xFFFF,1,"nacisnij punkt 1",0x0000);
	TFT_fillCircle(4,4,3,0xFFFF);
	gpio_TC();

	while( (x_raw<500 || x_raw>1000) || (y_raw<400 || y_raw>900) )
	{
		if(BB(GPIOA->IDR,PA0) == 0)
		{
			gpio_TFT();
			TFT_fillCircle(4,4,3,0xF800);
			gpio_TC();
		}
		delay_ms(50);
	}
	Px1 = x_raw;
	Py1 = y_raw;
	delta_x = x_raw - 40;
	delta_y = y_raw - 40;

	gpio_TFT();
	TFT_fillCircle(4,4,3,0x07E8);

	TFT_putString(83,160,0xFFFF,1,"                ",0x0000);
	gpio_TC();
	while(BB(GPIOA->IDR,PA0) == 0);
	gpio_TFT();
	delay_ms(1000);

	//pkt. 2 , skala
	TFT_putString(83,160,0xFFFF,1,"nacisnij punkt 2",0x0000);
	TFT_fillCircle(235,315,3,0xFFFF);
	gpio_TC();

	while( (x_raw<2950 || x_raw>3450) || (y_raw<3250 || y_raw>3750) )
	{
			if(BB(GPIOA->IDR,PA0) == 0)
			{
				gpio_TFT();
				TFT_fillCircle(235,315,3,0xF800);
				gpio_TC();
			}
			delay_ms(50);
	}
	Px2 = x_raw;
	Py2 = y_raw;
	kx = (float)(Px2 - delta_x) / 239;
	ky = (float)(Py2 - delta_y) / 319;


	gpio_TFT();
	TFT_fillCircle(235,315,3,0x07E8);
	TFT_putString(83,160,0xFFFF,1,"       OK       ",0x0000);
	delay_ms(800);
	gpio_TC();
}

void info(uint8_t x , uint8_t y)
{
	sprintf (buffer, "x_raw = %d   ", x_raw);//??? delta_y
	TFT_putString(x,y,0xFfff,1,buffer,tlo);
	sprintf (buffer, "y_raw = %d   ", y_raw);
	TFT_putString(x,y+10,0xFfff,1,buffer,tlo);

	sprintf (buffer, "x_tft = %d   ", x_tft);
	TFT_putString(x,y+20,0xFfff,1,buffer,tlo);
	sprintf (buffer, "y_tft = %d   ", y_tft);
	TFT_putString(x,y+30,0xFfff,1,buffer,tlo);

	sprintf (buffer, "delta_x = %d   ", delta_x);
	TFT_putString(x,y+40,0xFfff,1,buffer,tlo);
	sprintf (buffer, "delta_y = %d   ", delta_y);
	TFT_putString(x,y+50,0xFfff,1,buffer,tlo);

	sprintf (buffer, "kx = ??   ");
	TFT_putString(x,y+60,0xFfff,1,buffer,tlo);
	sprintf (buffer, "ky = ??   ");
	TFT_putString(x,y+70,0xFfff,1,buffer,tlo);
}

void TC_getXY()
{
	EXTI ->IMR = 0;			//Interrupt mask disable

	//X
	ConfigPin(GPIOB,YP,analog);
	ConfigPin(GPIOB,XM,output_PP_MS);
	ConfigPin(GPIOA,XP,output_PP_MS);
	ConfigPin(GPIOA,YM,input_fl);
	BB(GPIOA->ODR,XP) = 1;
	BB(GPIOB->ODR,XM) = 0;

	ADC1->JSQR |= 8<<15;	//kana³ 1 - PB0
	ADC1->CR2 |= ADC_CR2_JSWSTART;	//Pojedyñcza konwersja
	delay_ms(2);

	x_raw = ADC1->JDR1;
	x_tft = x_raw - delta_x;
	x_tft = x_tft / kx;
	//max 3308
	//min 678
	// /10,9583
	if(x_tft>239)x_tft=1000;

	//Y
	ConfigPin(GPIOB,YP,output_PP_MS);
	ConfigPin(GPIOB,XM,input_fl);
	ConfigPin(GPIOA,XP,analog);
	ConfigPin(GPIOA,YM,output_PP_MS);
	BB(GPIOB->ODR,YP) = 1;
	BB(GPIOA->ODR,YM) = 0;

	ADC1->JSQR = 0;			//kana³ 1 - PA0
	ADC1->CR2 |= ADC_CR2_JSWSTART;	//Pojedyñcza konwersja
	delay_ms(2);

	y_raw = ADC1->JDR1;
	y_tft = y_raw - delta_y;
	y_tft = y_tft / ky;
	//max 3600
	//min 580
	// /9,4374
	if(y_tft>319)y_tft=1000;

	EXTI ->IMR = (PA0);			//Interrupt mask dla pinu PA0
}

void gpio_TC()
{
	ConfigPin(GPIOB,YP,input_fl);
	ConfigPin(GPIOB,XM,input_fl);
	ConfigPin(GPIOA,XP,input_PUD);
	ConfigPin(GPIOA,YM,output_PP_MS);
	BB(GPIOA->ODR,XP) = 1;
	BB(GPIOA->ODR,YM) = 0;

	delay_ms(2);
	EXTI ->IMR = (PA0);			//Interrupt mask dla pinu PA0
}

void gpio_TFT()
{
	EXTI ->IMR = 0;			//Interrupt mask disable

	ConfigPin(GPIOB,RS,output_PP_MS);
	ConfigPin(GPIOB,CS,output_PP_MS);
	DATA_OUT;
}
