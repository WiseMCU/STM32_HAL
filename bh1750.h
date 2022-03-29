#ifndef	_BH1750_H_
#define	_BH1750_H_
#include "i2c.h"
#include "stdint.h"

#define BH1750_IIC hi2c1

#define	BH1750_ADDR_WRITE	0x46
#define	BH1750_ADDR_READ	0x47

typedef enum
{
		POWER_OFF_CMD				=	0x00,	//断电：无激活状态
		POWER_ON_CMD				=	0x01,	//通电：等待测量指令
		RESET_REGISTER				=	0x07,	//重置数字寄存器（在断电状态下不起作用）
		CONT_H_MODE					=	0x10,	//连续H分辨率模式：在11x分辨率下开始测量，测量时间120ms
		CONT_H_MODE2				=	0x11,	//连续H分辨率模式2：在0.51x分辨率下开始测量，测量时间120ms
		CONT_L_MODE					=	0x13,	//连续L分辨率模式：在411分辨率下开始测量，测量时间16ms
		ONCE_H_MODE					=	0x20,	//一次高分辨率模式：在11x分辨率下开始测量，测量时间120ms，测量后自动设置为断电模式
		ONCE_H_MODE2				=	0x21,	//一次高分辨率模式2：在0.51x分辨率下开始测量，测量时间120ms，测量后自动设置为断电模式
		ONCE_L_MODE					=	0x23	//一次低分辨率模式：在411x分辨率下开始测量，测量时间16ms，测量后自动设置为断电模式
} BH1750_MODE;

uint8_t	BH1750_Send_Cmd(BH1750_MODE cmd);
uint8_t BH1750_Read_Dat(uint8_t* dat);
uint16_t BH1750_Dat_To_Lux(uint8_t* dat);

/* example
 uint8_t dat[2] = {0};
 BH1750_Send_Cmd(ONCE_H_MODE);
 HAL_Delay(200);
 BH1750_Read_Dat(dat);
 printf("current: %5d lux\r\n", BH1750_Dat_To_Lux(dat));
 */


#endif
