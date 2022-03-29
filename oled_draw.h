/* OLED的应用层 */
#ifndef __OLED_DRAW_H__
#define __OLED_DRAW_H__
#include "oled.h"
#include "stdio.h"

/* 角度转弧度值 */
#define RADIAN(angle)  ((angle==0)?0:(3.14159*angle/180))

/* 坐标结构体 */
typedef struct COORDINATE 
{
	int x;
	int y;
}TypeXY;

/* 旋转坐标结构体 */
typedef struct ROATE
{
	TypeXY center;
	float angle;
	int direct;
}TypeRoate;

/* 时钟动画 */
void RoundClock(int hours , int minute , int sec);
/* 更新屏幕 */
void UpdateScreen(void);
/* 清除屏幕缓存 */
void ClearScreen(void);
/* 写缓存一个字符 */
void ScreenBuffer_ShowChar(uint16_t x, uint16_t y, FontDef_t *Size, uint8_t c);
/* 写缓存一串字符 */
void ScreenBuffer_ShowString(uint16_t x, uint16_t y, FontDef_t *Size, const char *fmt, ...);
/* 写缓存一个中文 */
void ScreenBuffer_ShowChineseChar(uint16_t x, uint16_t y, CN16FontDef_t *Size, char *CN);
/* 画方框 */
void DrawBox(TypeXY Start, TypeXY End);
/* 画线 */
void DrawLine(TypeXY Start, TypeXY End);
/* 画圆 */
void DrawCircle( int usX_Center, int usY_Center, int usRadius);

#endif
