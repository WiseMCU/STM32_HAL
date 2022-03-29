/* OLED底层驱动 */
#ifndef __OLED_H__
#define __OLED_H__
#include "fonts.h"
#include "i2c.h"

/* 修改对应的I2C句柄和屏幕大小 */
#define OLED_I2C hi2c2
#define  OLED_WIDTH    128
#define  OLED_HEIGHT   64
#define  OLED_WIDTH_HALF    OLED_WIDTH / 2
#define  OLED_HEIGHT_HALF   OLED_HEIGHT / 2

/* 英文字库 */
extern FontDef_t Font_8x16;
/* 中文字库 */
extern CN16FontDef_t CN16Font_16x16;
/* 设置图标 */
extern uint8_t bmp[];
/* 笑脸 */
extern uint8_t SmilingFace[];
/* 火柴人图标 */
extern uint8_t matchman3_1[];
extern uint8_t matchman3_2[];
extern uint8_t matchman3_3[];
extern uint8_t matchman3_4[];


//OLED初始化
void OLED_Init(void);
//清屏
void OLED_Clear(void);
//OLED显示一个字符
void OLED_ShowChar(uint16_t x, uint16_t y, FontDef_t *Size, uint8_t c);
//OLED显示一个中文
void OLED_ShowChineseChar(uint16_t x, uint16_t y, CN16FontDef_t *Size, char CN[2]);
//显示字符串
void OLED_ShowString(uint16_t x, uint16_t y, FontDef_t *Size, const char *fmt, ...);
//显示中文字符串
void OLED_ShowChineseString(uint16_t x, uint16_t y, CN16FontDef_t *Size, const char *fmt, ...);
//更新屏幕
void OLED_FILL(uint8_t *data);

#endif
