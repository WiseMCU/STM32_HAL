#ifndef __OLED_H__
#define __OLED_H__

#include "main.h"
#include "i2c.h"

//�޸Ķ�Ӧ��I2C���
#define I2C hi2c2

//OLED��ʼ��
void OLED_Init(void);
//����
void OLED_Clear(void);
//��ʾ�ַ���
void OLED_ShowString(uint16_t x,uint16_t y,const char *fmt, ...);

#endif
