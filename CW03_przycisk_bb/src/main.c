/*CW3 LED z przyciskiem [przycisk z softwarowym zatrzaskiem]
 * >przy u¿yciu bit-bandingu
 *
 * >onboard LED C13 do Vcc
 * >przycisk pod C14 do masy
 *
 *(SET portu to RESET led)
 */
#include <stdio.h>
#include "stm32f10x.h"
#include "LIB/lib.h"



uint8_t i=0;

int main(void)
{
	RCC->APB2ENR = RCC_APB2ENR_IOPAEN;	//w³. portu GPIO A
	//RCC->APB2ENR = RCC_APB2ENR_IOPCEN;	//w³. portu GPIO C

	ConfigPin(GPIOA,2,output);	//Config PC13
	ConfigPin(GPIOA,0,input);	//Config PC14

	BB(GPIOA->ODR,0) = 1;		//pull-up

	while(1)
	{
		if ((BB(GPIOA->IDR,0) == 0) && i==0)	//switch wciœniêty
		{
			i=1;

			for(int x=1; x<100000; x++);	//delay dla oscylacji tact-switch'a

			if(BB(GPIOA->IDR,0) == 0)
			{
				if(BB(GPIOA->ODR,2) == 1)
				{
					BB(GPIOA->ODR,2) = 0;
				}

				else
				{
					BB(GPIOA->ODR,2) = 1;
				}
			}
		}
		else if ((i==1) && (BB(GPIOA->IDR,0) == 1))	//switch otwarty
		{
			i=0;
		}
	}
}
