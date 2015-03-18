/*
*header file
*author: maemo
*date: 2012/07/16 22:43
*
*/

#ifndef _LCD_H_
#define _LCD_H_

#include "type.h"

extern void LCD_Init(void);
extern void LCD_Clear(void);
extern void LCD_SetAddr(uchar x, uchar y);
extern void LCD_WriteByte(uchar data, uchar cmd);
extern void LCD_WriteChar(uchar x, uchar y, uchar data);
extern void LCD_WriteCharX(uchar x, uchar y, uchar cnt, uchar *pdata);
extern void LCD_WriteString(uchar x, uchar y, uchar *pStr);
extern void LCD_WriteCN(uchar x, uchar y, uchar Index);
extern void LCD_WriteCNString(uchar x, uchar y, uchar length, uchar pStr[]);
extern void LCD_WriteIcon(uchar x, uchar y, uchar index);
extern void LCD_WriteIconX(uchar x, uchar y, uchar index, uchar len);
#endif