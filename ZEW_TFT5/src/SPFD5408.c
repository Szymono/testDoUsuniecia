/*
 * SPFD5408.c
 *
 *  Created on: 16.11.2017
 *      Author: rafal
 */

#include <stddef.h>
#include "stm32f10x.h"
//#include "stm32f1xx_hal.h"
//#include "gpio.h"

#include "pins.h"
#include "registers.h"
#include "glcdfont.h"
#include "SPFD5408.h"
#include "LIB/Delay/delay.h"

const uint16_t colors[8] =
{
   0x0000,
   0x001F,
   0xF800,
   0x07E0,
   0x07FF,
   0xF81F,
   0xFFE0,
   0xFFFF
};

void TFT_drawCircleHelper( int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color);
void TFT_fillCircleHelper(int x0, int y0, int radius, uint16_t color);


void TFT_write8_bits (uint8_t data)
{
	DELAY_CYCLE;
	DATA_PORT = (uint32_t) (((~data)<<16) | data);//!!!
	DELAY_CYCLE;
	WR_PULSE;
	DELAY_CYCLE;
}

static inline uint8_t TFT_read8_bits (void)
{	
	DATA_IN;
	uint8_t data = 0;
	RD_PULSE;
	DATA_OUT;
	return data;
}

static inline void TFT_writeRegister8_bits(uint8_t reg, uint8_t data)
{

	  COMMAND_MODE;   DELAY_CYCLE;
	  TFT_write8_bits(reg); 
	  DATA_MODE; 	  DELAY_CYCLE;
	  TFT_write8_bits(data);
}

static inline void TFT_writeRegister16_bits(uint16_t reg, uint16_t data)
{		
	 uint8_t HI, LO;
	 HI = reg>>8;
	 LO = reg;
	 COMMAND_MODE;
	 DELAY_CYCLE;
	 TFT_write8_bits(HI);
	 TFT_write8_bits(LO);
	 HI = data>>8;
	 LO = data;
	 DATA_MODE;
	 DELAY_CYCLE;
	 TFT_write8_bits(HI);
	 TFT_write8_bits(LO);
	 DELAY_CYCLE;
}


void TFT_reset(void)
{
	RST_OFF;
	delay_ms(10);
	RST_ON;
	delay_ms(20);
	RST_OFF;
	delay_ms(150);

	CS_ON;
	DELAY_CYCLE;
	COMMAND_MODE;
	DELAY_CYCLE;
	TFT_write8_bits(0x00);
	for(uint8_t i = 0; i < 3; i++)
	{
		WR_PULSE;
		DELAY_CYCLE;
	}
	DATA_MODE;
	DELAY_CYCLE;
	CS_OFF;
	DELAY_CYCLE;

}

static inline void TFT_writeRegisters_Pair(uint8_t regLO, uint8_t regHI, uint16_t data)
{		
	  register uint8_t HI, LO;
	
	  HI = data>>8;
	  LO = data;
	  
	  COMMAND_MODE; 
	  TFT_write8_bits(regHI); 
	  DATA_MODE;
	  TFT_write8_bits(HI);
	  COMMAND_MODE; 	  	 
	  TFT_write8_bits(regLO);
	  DATA_MODE;
	  TFT_write8_bits(LO);	  
}


static inline void writeRegister24(uint8_t reg, uint32_t data) 
{
  CS_ON;
  COMMAND_MODE;
  TFT_write8_bits(reg);
  DATA_MODE;
  delay_us(10);
  TFT_write8_bits(data>> 16);
  delay_us(10);
  TFT_write8_bits(data >> 8);
  delay_us(10);
  TFT_write8_bits(data);
  CS_OFF;

}

static inline void writeRegister32(uint8_t reg, uint32_t data) 
{
  CS_ON;
  COMMAND_MODE;
  TFT_write8_bits(reg);
  DATA_MODE;
  delay_us(2);
  TFT_write8_bits(data >> 24);
  delay_us(2);
  TFT_write8_bits(data>> 16);
  delay_us(2);
  TFT_write8_bits(data >> 8);
  delay_us(2);
  TFT_write8_bits(data);
  CS_OFF;

}


void TFT_init (void)
{	

	delay_ms(2);

	CS_OFF;
	RD_OFF;
	WR_OFF;
	RS_OFF;
	RST_OFF;

	DATA_PORT = (uint32_t)(0x00FF<<16);
	delay_ms(2);
	DATA_PORT = (uint32_t)(0xFF);
	delay_ms(2);
	COMMAND_PORT->CRL = ( COMMAND_PORT->CRL & 0x000FFFFF ) | 0x33300000;
	COMMAND_PORT->CRH = ( COMMAND_PORT->CRL & 0xFFFFFF00)  | 0x00000033;
	DATA_OUT;
	delay_ms(2);
	
	TFT_reset();
	delay_ms(200);
	
	
	CS_ON;
	TFT_writeRegister8_bits(ILI9341_SOFTRESET, 0);
	delay_ms(50);
	TFT_writeRegister8_bits(ILI9341_DISPLAYOFF, 0);

	TFT_writeRegister8_bits(ILI9341_POWERCONTROL1, 0x23);
	TFT_writeRegister8_bits(ILI9341_POWERCONTROL2, 0x10);
	TFT_writeRegister16_bits(ILI9341_VCOMCONTROL1, 0x2B2B);
	TFT_writeRegister8_bits(ILI9341_VCOMCONTROL2, 0xC0);
	TFT_writeRegister8_bits(ILI9341_MEMCONTROL, ILI9341_MADCTL_MY | ILI9341_MADCTL_BGR);
	TFT_writeRegister8_bits(ILI9341_PIXELFORMAT, 0x55);
	TFT_writeRegister16_bits(ILI9341_FRAMECONTROL, 0x001B);
	    
	TFT_writeRegister8_bits(ILI9341_ENTRYMODE, 0x07);
	/* writeRegister32(ILI9341_DISPLAYFUNC, 0x0A822700);*/

	TFT_writeRegister8_bits(ILI9341_SLEEPOUT, 0);
	delay_ms(150);
	TFT_writeRegister8_bits(ILI9341_DISPLAYON, 0);
	delay_ms(500);
	TFT_setAddrWindow(0, 0, TFTWIDTH-1, TFTHEIGHT-1);

}

void TFT_setAddrWindow(int x1, int y1, int x2, int y2) 
{
  CS_ON;
  uint32_t t;
  t = x1;
  t <<= 16;
  t |= x2;
  writeRegister32(ILI9341_COLADDRSET, t);  // HX8357D uses same registers!
  t = y1;
  t <<= 16;
  t |= y2;
  writeRegister32(ILI9341_PAGEADDRSET, t); // HX8357D uses same registers!
  CS_OFF;
}

void TFT_setRotation(uint8_t x) 
{
	uint16_t t = 0;
	CS_ON;
	   switch (x) 
	   {
	   	   case 2:
	   		   t = ILI9341_MADCTL_MX | ILI9341_MADCTL_BGR;
	       break;
	   	   case 3:
	           t = ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR;
	       break;
	   	   case 0:
	   		   t = ILI9341_MADCTL_MY | ILI9341_MADCTL_BGR;
	       break;
	   	   case 1:
	           t = ILI9341_MADCTL_MX | ILI9341_MADCTL_MY | ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR;
	       break;
	  }
	 TFT_writeRegister8_bits(ILI9341_MADCTL, t ); 
	 TFT_setAddrWindow(0, 0, TFTWIDTH - 1, TFTHEIGHT - 1); 
     CS_OFF;
	   
}

void TFT_setPixel (uint16_t x, uint16_t y, uint16_t color)
{
	TFT_setAddrWindow(x, y, TFTWIDTH-1, TFTHEIGHT-1);
	CS_ON;
	COMMAND_MODE; 
	TFT_write8_bits(0x2C);
	DATA_MODE; 
	TFT_write8_bits(color >> 8); 
	TFT_write8_bits(color);
}

void TFT_drwLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint16_t color)
{ 
     // zmienne pomocnicze
     int d, dx, dy, ai, bi, xi, yi;
     int x = x1, y = y1;
     // ustalenie kierunku rysowania
     if (x1 < x2)
     { 
         xi = 1;
         dx = x2 - x1;
     } 
     else
     { 
         xi = -1;
         dx = x1 - x2;
     }
     // ustalenie kierunku rysowania
     if (y1 < y2)
     { 
         yi = 1;
         dy = y2 - y1;
     } 
     else
     { 
         yi = -1;
         dy = y1 - y2;
     }
     // pierwszy piksel
     TFT_setPixel(x, y, color);
     // oœ wiod¹ca OX
     if (dx > dy)
     {
         ai = (dy - dx) * 2;
         bi = dy * 2;
         d = bi - dx;
         // pêtla po kolejnych x
         while (x != x2)
         { 
             // test wspó³czynnika
             if (d >= 0)
             { 
                 x += xi;
                 y += yi;
                 d += ai;
             } 
             else
             {
                 d += bi;
                 x += xi;
             }
             TFT_setPixel(x, y, color);
         }
     } 
     // oœ wiod¹ca OY
     else
     { 
         ai = ( dx - dy ) * 2;
         bi = dx * 2;
         d = bi - dy;
         // pêtla po kolejnych y
         while (y != y2)
         { 
             // test wspó³czynnika
             if (d >= 0)
             { 
                 x += xi;
                 y += yi;

                 d += ai;
             }
             else
             {
                 d += bi;
                 y += yi;
             }
             TFT_setPixel(x, y, color);
         }
     }
 }

void TFT_fillScreen(uint16_t color)
{
	TFT_setAddrWindow(0, 0, TFTWIDTH-1, TFTHEIGHT-1);
	
	uint8_t x = TFTWIDTH - 1;
	uint16_t y = TFTHEIGHT - 1;
	
	register uint8_t hi = color >> 8;
	register uint8_t lo = color;
	
	CS_ON;
	CS_ON;
	COMMAND_MODE;

	TFT_write8_bits(0x2C);
	DATA_MODE; 
	do
	{
		do
		{
			DATA_PORT = (uint32_t) (((~hi)<<16) | hi);
			WR_ON;
			WR_OFF; 
			DATA_PORT = (uint32_t) (((~hi)<<16) | lo);
			WR_ON;
			WR_OFF; 
		}
		while(x--);
		x = TFTWIDTH - 1;
	}
	while(y--);
}

void TFT_sendChar (uint16_t x, uint16_t y, uint16_t color, char data, uint8_t size, uint16_t background)
{	
	TFT_fullRect(x,y,5*size+1,7*size+1,background);
	
	for (int8_t i=0; i<6; i++ ) 
	{
	    uint8_t line;
	    if (i == 5) line = 0x0;
	    else line = *(font+(data*5)+i);
	    
	    for (int8_t j = 0; j<8; j++) 
	    {
	      if (line & 0x1) 
	      {
	    	  
	    	  if(size==1) TFT_setPixel(x+i, y+j, color);
	    	  else TFT_fillRect(x+(i*size), y+(j*size), size, size, color);
	      }
	      line >>= 1;
	    }	    
	}
}

void TFT_fillRect (uint16_t x, uint16_t y, uint16_t wid, uint16_t hei, uint16_t color)
{
	
	
	for(uint16_t i = 0; i <= hei; i++)
	{
		TFT_drwLine(x, (y+i), (wid+x), (y+i), color);
	}
	
}

void TFT_fullRect (uint16_t x, uint16_t y, uint16_t wid, uint16_t hei, uint16_t color)
{
		TFT_setAddrWindow(x, y, x+wid-1, y+hei-1);
		
		uint8_t w = wid - 1;
		uint16_t h = hei - 1;
		
		uint8_t hi = color >> 8;
		uint8_t lo = color;
		
		CS_ON;
		COMMAND_MODE; 
		TFT_write8_bits(0x2C);
		DATA_MODE; 
		do
		{
			do
			{
				DATA_PORT = (uint32_t) (((~hi)<<16) | hi);
				WR_PULSE;
				DATA_PORT = (uint32_t) (((~lo)<<16) | lo);
				WR_PULSE;
			}
			while(w--);
			w = wid - 1;
		}
		while(h--);
}

void TFT_putString (uint16_t x, uint16_t y,  uint16_t color, uint8_t size, char * string, uint16_t background)
{
	char znak = 0;
	uint16_t idx = 0;
	while((znak = *string++))
	{	
		
		TFT_sendChar(x+idx,y,color,znak,size,background);
		idx=idx+(5*size)+3;
	}
}

void TFT_clrString (uint16_t x, uint16_t y,  uint16_t color, uint8_t size, uint8_t len)
{
	uint16_t idx = 0;
	while(len--)
	{
		TFT_fullRect(x+idx,y,5*size+1,7*size+1,color);
		idx=idx+(5*size)+3;
	}
}

void TFT_sendBITMAP (uint16_t x, uint16_t y, uint16_t wid, uint16_t hei, const uint8_t * color)
{
		TFT_setAddrWindow(x, y, x+wid-1, y+hei-1);
		const uint8_t * wsk = color;
		uint8_t w = wid - 1;
		uint16_t h = hei - 1;


		CS_ON;
		COMMAND_MODE;
		TFT_write8_bits(0x2C);
		DATA_MODE;
		do
		{
			do
			{
				DATA_PORT = (uint32_t) (((~(*wsk))<<16) | *(wsk+1));
				WR_PULSE;
				DATA_PORT = (uint32_t) (((~(*wsk))<<16) | *(wsk)  );
				wsk++;wsk++;
				WR_PULSE;
			}
			while(w--);
			w = wid - 1;
		}
		while(h--);

}

void TFT_drawCircleHelper( int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color)
{
  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x     = 0;
  int16_t y     = r;

  while (x<y)
  {
    if (f >= 0)
    {
      y--;
      ddF_y += 2;
      f     += ddF_y;
    }

    x++;
    ddF_x += 2;
    f     += ddF_x;

    if (cornername & 0x4)
    {
    	TFT_setPixel(x0 + x, y0 + y, color);
    	TFT_setPixel(x0 + y, y0 + x, color);
    }
    if (cornername & 0x2)
    {
    	TFT_setPixel(x0 + x, y0 - y, color);
    	TFT_setPixel(x0 + y, y0 - x, color);
    }
    if (cornername & 0x8)
    {
    	TFT_setPixel(x0 - y, y0 + x, color);
    	TFT_setPixel(x0 - x, y0 + y, color);
    }
    if (cornername & 0x1)
    {
    	TFT_setPixel(x0 - y, y0 - x, color);
    	TFT_setPixel(x0 - x, y0 - y, color);
    }
  }
}



void TFT_drawcircle(int x0, int y0, int radius, uint16_t color)
{
    int x = radius-1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (radius << 1);

    while (x >= y)
    {
    	TFT_setPixel(x0 + x, y0 + y, color );
    	TFT_setPixel(x0 + y, y0 + x, color );
    	TFT_setPixel(x0 - y, y0 + x, color );
    	TFT_setPixel(x0 - x, y0 + y, color );
    	TFT_setPixel(x0 - x, y0 - y, color );
    	TFT_setPixel(x0 - y, y0 - x, color );
    	TFT_setPixel(x0 + y, y0 - x, color );
    	TFT_setPixel(x0 + x, y0 - y, color );

        if (err <= 0)
        {
            y++;
            err += dy;
            dy += 2;
        }
        if (err > 0)
        {
            x--;
            dx += 2;
            err += dx - (radius << 1);
        }
    }
}


void TFT_fillCircleHelper(int x0, int y0, int radius, uint16_t color)
{
    int x = radius;
    int y = 0;
    int xChange = 1 - (radius << 1);
    int yChange = 0;
    int radiusError = 0;

    while (x >= y)
    {
        for (int i = x0 - x; i <= x0 + x; i++)
        {
        	TFT_setPixel(i, y0 + y, color);
        	TFT_setPixel(i, y0 - y, color);
        }
        for (int i = x0 - y; i <= x0 + y; i++)
        {
        	TFT_setPixel(i, y0 + x, color);
        	TFT_setPixel(i, y0 - x, color);
        }

        y++;
        radiusError += yChange;
        yChange += 2;
        if (((radiusError << 1) + xChange) > 0)
        {
            x--;
            radiusError += xChange;
            xChange += 2;
        }
    }
}

void TFT_fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
	TFT_fillCircleHelper(x0, y0, r, color);
}

void TFT_drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
	TFT_drwLine(x, y, x, y+h-1, color);
}

void TFT_drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
	TFT_drwLine(x, y, x+w-1, y, color);
}
