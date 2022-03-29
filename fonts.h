/* OLED�ֿ� */
#ifndef __FONTS_H__
#define __FONTS_H__

#include "stdint.h"

typedef struct// ������ģ���ݽṹ 
{
	uint8_t  Index[2];// ������������,һ������ռ�����ֽ�	
	uint8_t  Msk[32]; 
}CN16CharTypeDef;

typedef struct {	//�����ַ��ṹ��
	uint8_t FontWidth;
	uint8_t FontHeight;
	const CN16CharTypeDef *data;
} CN16FontDef_t;

typedef struct {	//Ӣ���ַ��ṹ��
	uint8_t FontWidth;
	uint8_t FontHeight;
	const uint8_t *data;
} FontDef_t;

#endif
