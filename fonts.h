/* OLED字库 */
#ifndef __FONTS_H__
#define __FONTS_H__

#include "stdint.h"

typedef struct// 汉字字模数据结构 
{
	uint8_t  Index[2];// 汉字内码索引,一个汉字占两个字节	
	uint8_t  Msk[32]; 
}CN16CharTypeDef;

typedef struct {	//中文字符结构体
	uint8_t FontWidth;
	uint8_t FontHeight;
	const CN16CharTypeDef *data;
} CN16FontDef_t;

typedef struct {	//英文字符结构体
	uint8_t FontWidth;
	uint8_t FontHeight;
	const uint8_t *data;
} FontDef_t;

#endif
