/* < Szymon Ostrowski - 04.10.2018 >
 * < CW14_ADC_Continuous - main.c >
 *
 * > ADC1 - Continuous mode (przez TIM1)  & Injected mode
 * > Pomiar z PA1 ci¹g³y
 */


#include <stdio.h>
#include "stm32f10x.h"
#include "LIB/Interrupt/intpt.h"
#include "LIB/Useful/useful.h"
#include "LIB/GPIO/gpio.h"
#include "LIB/Delay/delay.h"


int main(void)
{
	config_delay();

	RCC->APB2ENR = RCC_APB2ENR_IOPAEN | RCC_APB2ENR_ADC1EN | RCC_APB2ENR_TIM1EN; //w³. zegarów ADC, TIM1, GPIOA
	ConfigPin(GPIOA,PA1,analog);
	ConfigPin(GPIOA,PA2,analog);
	ConfigPin(GPIOA,PA3,analog);
	ConfigPin(GPIOA,PA4,analog);

	//TIM1 Config
	TIM1->PSC = 7200;			//wartoœc preskalera
	TIM1->ARR = 100;			//wartoœc ARR
	TIM1->EGR = TIM_EGR_UG;		//wymuszenie UEV
	TIM1->CR2 |= TIM_CR2_MMS_1;	//timer w trybie master

	//ADC Config
	//ADC1->CR1 = ADC_CR1_JDISCEN;
	ADC1->CR2 = ADC_CR2_JEXTTRIG | ADC_CR2_ADON ; //set TIM1_TRGO , w³. ADC1
	delay_ms(100);
	ADC1->JSQR = 3<<20;
	ADC1->JSQR |= 1<<15;		//set kana³ów - ustawiony 1 kana³ PA1 (MAX 4 kana³y , przy trybie Injected)
	ADC1->JSQR |= 2<<10;		//set kana³ów - ustawiony 1 kana³ PA1 (MAX 4 kana³y , przy trybie Injected)
	ADC1->JSQR |= 3<<5;		//set kana³ów - ustawiony 1 kana³ PA1 (MAX 4 kana³y , przy trybie Injected)
	ADC1->JSQR |= 4<<0;		//set kana³ów - ustawiony 1 kana³ PA1 (MAX 4 kana³y , przy trybie Injected)

	TIM1->CR1 = TIM_CR1_CEN;	//w³. TIM1
	while( ! (ADC1->SR & ADC_SR_JEOC) );

	while(1)
	{
		delay_ms(100);
	}

}

