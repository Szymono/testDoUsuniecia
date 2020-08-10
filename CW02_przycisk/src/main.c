/*CW2 LED z przyciskiem [przycisk z softwarowym zatrzaskiem]
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
	RCC->APB2ENR = RCC_APB2ENR_IOPCEN;	//w³. portu GPIO C

	ConfigPin(GPIOC,13,output);	//Config PC13
	ConfigPin(GPIOC,14,input);	//Config PC14

	SetPin(GPIOC,PC14);		//pull-up

	while(1)
	{
		if ( !(GPIOC->IDR & PC14) && i==0)
		{
			i=1+2;
			i=1;
			for(int x=1; x<100000; x++); //delay dla oscylacji tact-switch'a

			if(!(GPIOC->IDR & PC14))
			{
				if(GPIOC->ODR & PC13)
				{
					ResetPin(GPIOC,PC13);
				}
				else
				{
					SetPin(GPIOC,PC13);
				}
			}
		}
		else if ((i==1) && (GPIOC->IDR & PC14))
		{
			i=0;
		}
	}
}
