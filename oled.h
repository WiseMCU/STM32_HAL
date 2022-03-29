/* OLED�ײ����� */
#ifndef __OLED_H__
#define __OLED_H__
#include "fonts.h"
#include "i2c.h"

/* �޸Ķ�Ӧ��I2C�������Ļ��С */
#define OLED_I2C hi2c2
#define  OLED_WIDTH    128
#define  OLED_HEIGHT   64
#define  OLED_WIDTH_HALF    OLED_WIDTH / 2
#define  OLED_HEIGHT_HALF   OLED_HEIGHT / 2

/* Ӣ���ֿ� */
extern FontDef_t Font_8x16;
/* �����ֿ� */
extern CN16FontDef_t CN16Font_16x16;
/* ����ͼ�� */
extern uint8_t bmp[];
/* Ц�� */
extern uint8_t SmilingFace[];
/* �����ͼ�� */
extern uint8_t matchman3_1[];
extern uint8_t matchman3_2[];
extern uint8_t matchman3_3[];
extern uint8_t matchman3_4[];


//OLED��ʼ��
void OLED_Init(void);
//����
void OLED_Clear(void);
//OLED��ʾһ���ַ�
void OLED_ShowChar(uint16_t x, uint16_t y, FontDef_t *Size, uint8_t c);
//OLED��ʾһ������
void OLED_ShowChineseChar(uint16_t x, uint16_t y, CN16FontDef_t *Size, char CN[2]);
//��ʾ�ַ���
void OLED_ShowString(uint16_t x, uint16_t y, FontDef_t *Size, const char *fmt, ...);
//��ʾ�����ַ���
void OLED_ShowChineseString(uint16_t x, uint16_t y, CN16FontDef_t *Size, const char *fmt, ...);
//������Ļ
void OLED_FILL(uint8_t *data);

#endif
