/*
*description:lcd file
*author: maemo
*date: 2012/07/16 22:43
*
*/
#include "lcd.h"   
#include <msp430g2553.h>
#include "enchar6x8.h"
#include "CN_Char.h"

#define RST     0x01
#define SCE     0x02   
#define DC      0x04
#define SDIN    0x08
#define SCLK    0x10


#define SCLK_SET (P2OUT |= SCLK)
#define SCLK_CLR (P2OUT &= ~SCLK)

#define SDIN_SET (P2OUT |= SDIN)
#define SDIN_CLR (P2OUT &= ~SDIN)
//#define SDIN_SET(X) (P1OUT ^= (X)<<1)   /*  0000 0001 ^ 0000 0000 = 0000 0001   */

#define DC_SET (P2OUT |= DC)
#define DC_CLR (P2OUT &= ~DC)
//#define DC_SET(X) (P1OUT ^= (X)<<2)     /*1111 0000  ^ 0000 0100 = 1111 0100*/

#define RST_SET (P2OUT |= RST)
#define RST_CLR (P2OUT &= ~RST)  

#define SCE_SET (P2OUT |= SCE)
#define SCE_CLR (P2OUT &= ~SCE)





#define PD_HORIZON_BASIC 	0x20
#define PD_HORIZON_EXTEND	0x21
#define BASIC_NORMAL_MODE 	0x0c
#define BASIC_INVERSE_MODE  0x0d
#define BASIC_SET_Y_ADDR	0x40
#define BASIC_SET_X_ADDR	0x80
#define EXTEND_TEMP_CTL 	0x04
#define EXTEND_BIAS_SYS		0x10
#define EXTEND_SET_VOP		0x80



uchar FontLink[4][7];


/*
*description:initialize a byte 
*author:maemo
*date:2012/07/16 21:17
*input: none
*output: none
*history:
*/
void LCD_Init(void)
{
    P2DIR   |= 0x3f;                
    RST_CLR;
    __delay_cycles(100);				
    RST_SET;
    SCE_SET;									//close lcd
    LCD_WriteByte(PD_HORIZON_EXTEND, 0);		//use extended instructions
    LCD_WriteByte(EXTEND_SET_VOP|0x48, 0);		//set Vop, Vlcd=3.06+0.06*Vop,work voltage6.0<Vlcd<9.0
    LCD_WriteByte(EXTEND_TEMP_CTL|0x02, 0);		//set temperature coefficient
    LCD_WriteByte(EXTEND_BIAS_SYS|0x03, 0);		//set bias system 	
    LCD_WriteByte(PD_HORIZON_BASIC, 0);			//use basic instructions
    LCD_WriteByte(BASIC_NORMAL_MODE, 0);		//display control set normal mode
    LCD_Clear();
    SCE_CLR;
    for (uchar i=0; i<4; i++)
    {
        for (uchar j=0; j<7; j++)
        {
            FontLink[i][j] = 0;
        }
    }
} 
 
/*
*description:write a byte 
*author:maemo
*date:2012/07/16 21:17
*input: data, cmd
*output: none
*history:
*/
void LCD_WriteByte(uchar data, uchar cmd)
{
    uchar i=0;
	
      SCE_CLR;
//	__delay_cycles(100);
    if (cmd)
    {
	    DC_SET;
    }
    else
    {
        DC_CLR;
    }
	
	
    for (i=0; i<8; i++)
    {
        SCLK_CLR;
        if (data&0x80)						//msb first
        {
            SDIN_SET;
        }
        else
        {
            SDIN_CLR;
        }		
        data <<= 1;
        SCLK_SET;
    }
    SCE_SET;
}



void LCD_Clear(void)
{
    uint i=0;
    LCD_SetAddr(0, 0);
    for (i=0; i<504; i++)
    {
        LCD_WriteByte(0, 1);
    }
}
/*
*description:set the address 
*date:2012/07/16 22:50
*input: x address 0 --- 83, y address 0 --- 5 
*output: none
*other:the address counter is automatically increase by 1
*history:
*/
void LCD_SetAddr(uchar x, uchar y)
{
    LCD_WriteByte(BASIC_SET_X_ADDR|x, 0);
    LCD_WriteByte(BASIC_SET_Y_ADDR|y, 0);
}

/*
*description:write a char ,6 * 8 
*date:2012/07/16 22:23
*input: x address 0 --- 83, y address 0 --- 5,  data -- to be displayed
*output: none
*other:the address counter is automatically increase by 1
*history:
*/
void LCD_WriteChar(uchar x, uchar y, uchar data)
{
    unsigned char line;

    data -= 32;
    LCD_SetAddr(x, y);
    
    for (line=0; line<5; line++)
    {
        LCD_WriteByte(font6x8[data][line], 1);
    }      

}

void LCD_WriteCharX(uchar x, uchar y, uchar cnt, uchar *pdata)
{
    unsigned char i, line;

    LCD_SetAddr(x, y);

    for  (i=0; i<cnt; i++)
    {
        for (line=0; line<5; line++)
        {
            LCD_WriteByte(font6x8[*(pdata+i)-32][line], 1);
        }
    }


}


/*
*description:write a string ,6 * 8 
*date: 2012/07/16 22:23
*input: x address 0 --- 83, y address 0 --- 5,  data -- to be displayed
*output: none
*other:the address counter is automatically increase by 1
*history:
*
*/
void LCD_WriteString(uchar x, uchar y, uchar *pStr)
{
    LCD_SetAddr(x, y);
    while(*pStr)
    {
        LCD_WriteChar(x, y, *pStr);
        pStr++;
        x = x+5;
    }
}

void LCD_WriteCN(uchar x, uchar y, uchar Index)
{
    uchar i=0, data=0;
	
    switch(y)
    {
        case 0:
        FontLink[0][x] = Index;
        LCD_SetAddr(12*x, 0);
        for (i=0; i<12; i++ )							/*高8位*/
        {
                LCD_WriteByte(FontLib[Index][i], 1)	;
        }
        LCD_SetAddr(12*x, 1);
        for (i=0; i<12; i++)							/*低四位,凑成8位*/
        {	/*由两部分组成 == 当前字的后12字节的低四位 + 下一字的前12字节低四位, 后者成为高四位*/
                /*					0000 xxxx    +    mmmm nnnn 左移四位 nnnn 0000 = nnnn xxxx*/
                data = FontLib[Index][i+12] | (FontLib[FontLink[1][x]][i]<<4);
                LCD_WriteByte(data, 1);
        }
        break;
        
        case 1:
        FontLink[1][x] = Index;
        LCD_SetAddr(12*x, 1);                          /*高四位，凑成8位*/					
        for (i=0; i<12; i++)
        {	/*8位数由两部分组成 == 当前字的前12字节低四位 + 上一列同一位置的字后12位的位四位，前者变成高四位*/ 
                /*				   xxxx yyyy 左移四位 yyyy 0000  + 0000 nnnn == yyyy nnnn						*/ 
                data = FontLib[Index][i]<<4 | FontLib[FontLink[0][x]][i+12];	
                LCD_WriteByte(data, 1);
        }
        LCD_SetAddr(12*x, 2);                          /*低8位*/
        for (i=0; i<12; i++)
        {	/*低8位由两部分 == 前12字节的高四位 + 后12字节的低四位， 高-》低，低-》高 */
                /*  	xxxxx yyyy 右移四位	0000 xxxx + 0000 mmmm 左移四位 mmmm 0000 = mmmm xxxx	*/
                data = FontLib[Index][i]>>4 | FontLib[Index][i+12]<<4;
                LCD_WriteByte(data, 1);
        }
        break;
        
        case 2:
        FontLink[2][x] = Index;
        LCD_SetAddr(12*x, 3);
        for (i=0; i<12; i++)							/*高8位*/
        {
                LCD_WriteByte(FontLib[Index][i], 1);
        }
        LCD_SetAddr(12*x, 4);
        for (i=0; i<12; i++)							/*低四位，凑成8位*/
        {	/*由两部分组成 == 当前字的后12字节的低8位 + 下一字的前12字节的低四位，后者变成高四位*/
                /*					0000 xxxx   +     mmmm nnnn 左移四位 = nnnn xxxx*/
                data = FontLib[Index][i+12] | (FontLib[FontLink[3][x]][i]<<4);
                LCD_WriteByte(data, 1);				
        }
        break;
        
        case 3:
        FontLink[3][x] = Index;
        LCD_SetAddr(12*x, 4);
        for (i=0; i<12; i++)							/*高四位，凑8位*/
        {	/*8位数由两部分组成 == 当前字的前12字节低四位 + 上一列同一位置的字后12位的低四位，前者变成高四位*/ 
                /*				   xxxx yyyy 左移四位 yyyy 0000  + 0000 nnnn == yyyy nnnn						*/ 
                data = FontLib[Index][i]<<4 | FontLib[FontLink[2][x]][i+12];
                LCD_WriteByte(data, 1);
        }
        LCD_SetAddr(12*x, 5);
        for (i=0; i<12; i++)							/*低8位*/
        {	/*低8位由两部分 == 前12字节的高四位 + 后12字节的低四位， 高-》低，低-》高 */
                /*  	xxxxx yyyy 右移四位	0000 xxxx + 0000 mmmm 左移四位 mmmm 0000 = mmmm xxxx	*/
                data = FontLib[Index][i]>>4 | FontLib[Index][i+12]<<4;
                LCD_WriteByte( data , 1);
        }
        break;	
        default:
                ;
    }

}

void LCD_WriteCNString(uchar x, uchar y, uchar length, uchar pStr[])
{	
    uchar i, j , count;
    
    i = x;
    j = y;
    count = 1;
    while (count<=length)
    {
        if (i>6)
        {
            i=0;
            j++;
            if (j>3)
            {
                j=0;
            }
        }        
        LCD_WriteCN(i, j, pStr[count] );
        count++;
        i++;
    }
}

void LCD_WriteIcon(uchar x, uchar y, uchar index)
{
    uchar i=0;
    
    LCD_SetAddr(x, y);
    for (i=0; i<8; i++) 
      LCD_WriteByte(icon[index][i], 1);
	  
}

void LCD_WriteIconX(uchar x, uchar y, uchar index, uchar len)
{
	uchar i=0;
	
    LCD_SetAddr(x, y);
	for (i=0; i<len; i++) 
		LCD_WriteByte(iconx[index+i], 1);
		
}
