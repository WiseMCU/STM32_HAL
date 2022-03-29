#include "oled.h"
#include "stdarg.h"
#include "stdio.h"


/* ��ʼ������ */
uint8_t CMD_Data[]={
0xAE, 0x00, 0x10, 0x40, 0xB0, 0x81, 0xFF, 0xA1, 0xA6, 0xA8, 0x3F,
0xC8, 0xD3, 0x00, 0xD5, 0x80, 0xD8, 0x05, 0xD9, 0xF1, 0xDA, 0x12,				
0xD8, 0x30, 0x8D, 0x14, 0xAF};

/* ���豸д�������� */
void OLED_WR_CMD(uint8_t cmd)
{
	HAL_I2C_Mem_Write(&OLED_I2C , 0x78, 0x00, I2C_MEMADD_SIZE_8BIT, &cmd, 1, 0xFF);
}

/* ���豸д���� */
void OLED_WR_DATA(uint8_t data)
{
	HAL_I2C_Mem_Write(&OLED_I2C , 0x78, 0x40, I2C_MEMADD_SIZE_8BIT, &data, 1, 0xFF);
}

/* ���ù������ */
void OLED_Set_Pos(uint8_t x, uint8_t y)
{ 	
	OLED_WR_CMD(0xb0+y);
	OLED_WR_CMD(((x&0xf0)>>4)|0x10);
	OLED_WR_CMD(x&0x0f);
}

/* дһ���ֽڵ�ָ������ */
void WriteByte(uint8_t x, uint8_t y, uint8_t data)
{
	OLED_Set_Pos(x, y);
	OLED_WR_DATA(data);
}

/* дȫ����Ļ */
void OLED_FILL(uint8_t *data)
{
  for(int i = 0; i<(OLED_HEIGHT / 8); i++)
	{
		OLED_Set_Pos(0, i);
		HAL_I2C_Mem_Write(&OLED_I2C , 0x78, 0x40, I2C_MEMADD_SIZE_8BIT, data + OLED_WIDTH * i, OLED_WIDTH, 0xFF);
	}
}

/* OLED��ʾһ���ַ� */
void OLED_ShowChar(uint16_t x, uint16_t y, FontDef_t *Size, uint8_t c)
{
	#define ROW Size->FontWidth * Size->FontHeight / 8 + 1
	int num = 0;
	while(c != Size->data[num])
		num += ROW;
	for(int b = 0; b < (Size->FontHeight / 8); b++)
		for(int a=0; a < Size->FontWidth; a++)
			WriteByte(x + a, y + b, Size->data[num + 1 + a + (b * Size->FontWidth)]);
}

/* OLED��ʾһ���ַ� */
static char dat[128];	//16*8
void OLED_ShowString(uint16_t x, uint16_t y, FontDef_t *Size, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vsnprintf(dat, sizeof(dat) - 1, fmt, args);
	va_end(args);
	int i = 0;
	while(dat[i] != '\0')
	{
		OLED_ShowChar(x ,y, Size, dat[i]);
		x += Size->FontWidth;
		i++;
	}
}

/* OLED��ʾһ������ */
void OLED_ShowChineseChar(uint16_t x, uint16_t y, CN16FontDef_t *Size, char CN[2])
{
	int num = 0;
	while (1)
	{
		if((Size->data[num].Index[0] == *(CN + 0)) && (Size->data[num].Index[1] == *(CN + 1)))
			break;
		
		num++;
	}
	for(int b = 0; b < (Size->FontHeight / 8); b++)
		for(int a=0; a < Size->FontWidth; a++)
			WriteByte(x + a, y + b, Size->data[num].Msk[a + (b * Size->FontWidth)]);
}

/* OLED��ʾ�������ַ� */
static char Cdat[256];	//16*16
void OLED_ShowChineseString(uint16_t x, uint16_t y, CN16FontDef_t *Size, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vsnprintf(Cdat, sizeof(Cdat) - 1, fmt, args);
	va_end(args);
	int i = 0;
	while(Cdat[2*i] != '\0')
	{
		OLED_ShowChineseChar(x ,y, Size, (Cdat+2*i));//���飬128�У�2�У������ʾ128���ַ�
		x += Size->FontWidth;
		i++;
	}
}

/* ���� */
void OLED_Clear(void)
{	    
	for(int i=0;i < (OLED_HEIGHT / 8);i++)  
	{
		OLED_WR_CMD(0xb0 + i);
		OLED_WR_CMD (0x00);
		OLED_WR_CMD (0x10);
		for(int n = 0; n < OLED_WIDTH; n++)
			OLED_WR_DATA(0x00);
	} 
}

/* ��ʼ��oled��Ļ */
void OLED_Init(void)
{
	HAL_I2C_Mem_Write(&OLED_I2C , 0x78, 0x00, I2C_MEMADD_SIZE_8BIT, CMD_Data, sizeof(CMD_Data), 0x100);
	OLED_Clear();
}
