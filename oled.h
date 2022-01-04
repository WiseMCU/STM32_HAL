#ifndef __OLED_H__
#define __OLED_H__

#include "main.h"
#include "i2c.h"

//修改对应的I2C句柄
#define I2C hi2c2

//OLED初始化
void OLED_Init(void);
//清屏
void OLED_Clear(void);
//显示字符串
void OLED_ShowString(uint16_t x,uint16_t y,const char *fmt, ...);

#endif
