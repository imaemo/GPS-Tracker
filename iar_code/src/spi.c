#include "spi.h"
#include <msp430g2553.h>


/****sd module define***************************/
#define		SD_DIN			BIT5
#define 	SD_SCLK			BIT4
#define		SD_DO			BIT3
#define		SD_CS			BIT6

#define		SD_SCLK_LOW()	(P1OUT &= ~SD_SCLK)
#define		SD_SCLK_HIGH()	(P1OUT |= SD_SCLK)
#define		SD_DO_LOW()		(P1OUT &= ~SD_DO)
#define		SD_DO_HIGH()	(P1OUT |= SD_DO)
#define		SD_CS_LOW()		(P1OUT &= ~SD_CS)
#define		SD_CS_HIGH()	(P1OUT |= SD_CS)
#define		SD_DI_READ()	(P1IN & SD_DIN)	

void spi_init()
{
	P1DIR |= 	SD_CS+SD_DO+SD_SCLK;
	P1DIR &=	SD_DIN;
    P1OUT |=    SD_CS;
}

void spi_send(uchar dat)
{
	uchar i;
	
//	SD_CS_LOW();
	//SD_SCLK_LOW();
	for (i=0; i<8; i++)
	{
		SD_SCLK_LOW();
		if (0x80&dat)
			SD_DO_HIGH();
		else
			SD_DO_LOW();
        __delay_cycles(10);
		SD_SCLK_HIGH();	
	}
	SD_SCLK_LOW();
//	SD_CS_HIGH();
}

uchar spi_receive()
{
	uchar i,tmp=0;
	
//	SD_CS_LOW();
	SD_SCLK_LOW();
	
	for (i=0; i<8; i++)
	{
		tmp <<= 1;
		SD_SCLK_HIGH();
		if (SD_DI_READ())
			tmp |= 0x01;
		SD_SCLK_LOW();
	}
//	SD_CS_HIGH();
	
	return tmp;
}