/*
 * main.c
 *
 *  Created on: 07.12.2018
 *      Author: Chleb
 */


#include <stdio.h>
#include "stm32f10x.h"

#include "LIB/Interrupt/intpt.h"
#include "LIB/Useful/useful.h"
#include "LIB/GPIO/gpio.h"
#include "LIB/Delay/delay.h"


void i2c_init();
void i2c_send();


int main(void)
{
	i2c_init();

	while(1)
	{
		i2c_send();
		delay_ms(100);
	}
}


void i2c_init()
{
	// ---
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPBEN;
	// ---
	ConfigPin(GPIOB,PB6,alternate_OD_LS);
	ConfigPin(GPIOB,PB7,alternate_OD_LS);
	// ---
	I2C1->CR2 = 36;		//pheriperial i2c frq = 36MHz
	I2C1->TRISE = 37;	//TRISE = ( pheriperial i2c frq / 1MHz ) + 1
	I2C1->CCR = 180;	//SCL frq = 100kHz [result = (uint16_t)(pclk1 / (I2C_InitStruct->I2C_ClockSpeed << 1));]

	I2C1->CR1 |= I2C_CR1_PE;
}

void i2c_send()
{
	I2C1->CR1 |= I2C_CR1_START;					// request a start
	while (!(I2C1->SR1 & I2C_SR1_SB));

	I2C1->DR = 0x18;
	while (!(I2C1->SR1 & I2C_SR1_TXE));

	I2C1->CR1 |= I2C_CR1_STOP;

	I2C1->SR1 = 0;
	I2C1->SR2 = 0;
}




