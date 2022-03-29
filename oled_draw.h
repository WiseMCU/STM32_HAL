/* OLED��Ӧ�ò� */
#ifndef __OLED_DRAW_H__
#define __OLED_DRAW_H__
#include "oled.h"
#include "stdio.h"

/* �Ƕ�ת����ֵ */
#define RADIAN(angle)  ((angle==0)?0:(3.14159*angle/180))

/* ����ṹ�� */
typedef struct COORDINATE 
{
	int x;
	int y;
}TypeXY;

/* ��ת����ṹ�� */
typedef struct ROATE
{
	TypeXY center;
	float angle;
	int direct;
}TypeRoate;

/* ʱ�Ӷ��� */
void RoundClock(int hours , int minute , int sec);
/* ������Ļ */
void UpdateScreen(void);
/* �����Ļ���� */
void ClearScreen(void);
/* д����һ���ַ� */
void ScreenBuffer_ShowChar(uint16_t x, uint16_t y, FontDef_t *Size, uint8_t c);
/* д����һ���ַ� */
void ScreenBuffer_ShowString(uint16_t x, uint16_t y, FontDef_t *Size, const char *fmt, ...);
/* д����һ������ */
void ScreenBuffer_ShowChineseChar(uint16_t x, uint16_t y, CN16FontDef_t *Size, char *CN);
/* ������ */
void DrawBox(TypeXY Start, TypeXY End);
/* ���� */
void DrawLine(TypeXY Start, TypeXY End);
/* ��Բ */
void DrawCircle( int usX_Center, int usY_Center, int usRadius);

#endif
