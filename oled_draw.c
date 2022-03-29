#include "oled_draw.h"
#include "math.h"
#include "string.h"
#include "stdarg.h"
#include "stdio.h"

/* 定义屏幕缓存 */
static uint8_t ScreenBuffer[OLED_WIDTH * OLED_HEIGHT / 8] = {0};
TypeRoate _RoateValue={{0, 0}, 0, 1};

/* 将缓存更新至屏幕 */
void UpdateScreen(void)
{
	OLED_FILL(ScreenBuffer);
}

/* 清除屏幕缓存 */
void ClearScreen(void)
{
	memset(ScreenBuffer, 0, sizeof(ScreenBuffer));
}

/* 设置屏幕缓存区某一个点的亮灭 */
void SetPointBuffer(int x, int y)
{
	if(x > OLED_WIDTH - 1 || y > OLED_HEIGHT - 1)   //超出范围
		return;
	uint16_t buf_ROW = y / 8;
	uint16_t buf = y % 8;
	ScreenBuffer[OLED_WIDTH * buf_ROW + x - 1] = ScreenBuffer[OLED_WIDTH * buf_ROW + x - 1] | (1<<buf);
}

/* 写缓存一个字符 */
void ScreenBuffer_ShowChar(uint16_t x, uint16_t y, FontDef_t *Size, uint8_t c)
{
	#define ROW Size->FontWidth * Size->FontHeight / 8 + 1
	int num = 0;
	while(c != Size->data[num])
		num += ROW;
	num++;
	for(int b = 0; b < (Size->FontHeight / 8); b++)
		for(int a=0; a < Size->FontWidth; a++)
			ScreenBuffer[(y + b) * OLED_WIDTH + x + a] = Size->data[num + a + (b * Size->FontWidth)];
}

/* 写缓存一串字符 */
static char dat[128];
void ScreenBuffer_ShowString(uint16_t x, uint16_t y, FontDef_t *Size, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vsnprintf(dat, sizeof(dat) - 1, fmt, args);
	va_end(args);
	int i = 0;
	while(dat[i] != '\0')
	{
		ScreenBuffer_ShowChar(x ,y, Size, dat[i]);
		x += Size->FontWidth;
		i++;
	}
}

/* 写缓存一个中文 */
void ScreenBuffer_ShowChineseChar(uint16_t x, uint16_t y, CN16FontDef_t *Size, char *CN)
{
	int num = 0;
	while (1)
	{
		if((Size->data[num].Index[0] == *CN) && (Size->data[num].Index[1] == *(CN + 1)))
			break;
		num++;
	}
	for(int b = 0; b < (Size->FontHeight / 8); b++)
		for(int a=0; a < Size->FontWidth; a++)
			ScreenBuffer[(y + b) * OLED_WIDTH + x + a] = Size->data[num].Msk[b * Size->FontWidth + a];
}

/* 设置旋转参数 */
TypeRoate SetRotateValue(int x, int y, float angle, int direct)
{
	TypeRoate buf;
	// 旋转中心
	buf.center.x = x;
	buf.center.y = y;
	// 旋转方向1顺时针,0逆时针
	buf.direct = direct;
	// 旋转角度
	buf.angle = RADIAN(angle);
	return buf;
}

/* 开二次方根 */
float mySqrt(float x)
{
	float a = x;
	unsigned int i = *(unsigned int *)&x;
	i = (i + 0x3f76cf62) >> 1;
	x = *(float *)&i;
	x = (x + a / x) * 0.5;
	return x;
}

/* 将一个坐标旋转一定角度 
 * Temp:需要旋转的坐标
 */
void Rotate(TypeXY* Temp, TypeRoate RoateValue)
{
	float temp = (Temp->y - RoateValue.center.y) * (Temp->y - RoateValue.center.y) + (Temp->x - RoateValue.center.x) * (Temp->x - RoateValue.center.x);
	double r = mySqrt(temp);
	double a0 = atan2(Temp->x - RoateValue.center.x, Temp->y - RoateValue.center.y);
	if(RoateValue.direct)
	{
		Temp->x = RoateValue.center.x + r * cos(a0 + RoateValue.angle);
		Temp->y = RoateValue.center.y + r * sin(a0 + RoateValue.angle);
	}
	else
	{
		Temp->x = RoateValue.center.x + r * cos(a0 - RoateValue.angle);
		Temp->y = RoateValue.center.y + r * sin(a0 - RoateValue.angle);	
	}
}

/* 将一个坐标旋转一定角度 
 * x, y:需要旋转的坐标
 * return：旋转后的坐标
 */
TypeXY GetRotateXY(int x, int y, TypeRoate RoateValue)
{
	TypeXY temp = {x, y};
	Rotate(&temp, RoateValue);
	return temp;
}

/* 画方框 */
void DrawBox(TypeXY Start, TypeXY End)
{
	/* 画竖线 */
	if(Start.x == End.x)
	{
		if(Start.y >= End.y)
			for(int i = 0; i < (Start.y - End.y); i++)
				SetPointBuffer(Start.x, End.y + i);
		else
			for(int i = 0; i < (End.y - Start.y); i++)
				SetPointBuffer(Start.x, Start.y + i);
		return;
	}
	/* 画横线 */
	if(Start.y == End.y)
	{
		if(Start.x >= End.x)
			for(int i = 0; i < (Start.x - End.x); i++)
				SetPointBuffer(End.x + i, Start.y);
		else
			for(int i = 0; i < (End.x - Start.x); i++)
				SetPointBuffer(Start.x + i, Start.y);
		return;
	}
	if(Start.x > End.x)
	{
		if(Start.y > End.y)
		{
			for(int i = 0; i < (Start.x - End.x); i++)
				SetPointBuffer(End.x + i, End.y),
				SetPointBuffer(End.x + i, Start.y);
			for(int i = 0; i < (Start.y - End.y); i++)
				SetPointBuffer(End.x, End.y + i),
				SetPointBuffer(Start.x, End.y + i);
			return;
		}else{
			for(int i = 0; i < (Start.x - End.x); i++)
				SetPointBuffer(End.x + i, End.y),
				SetPointBuffer(End.x + i, Start.y);
			for(int i = 0; i < (End.y - Start.y); i++)
				SetPointBuffer(End.x, Start.y + i),
				SetPointBuffer(Start.x, Start.y + i);
			return;
		}
	}else{
		if(Start.y > End.y)
		{
			for(int i = 0; i < (End.x - Start.x); i++)
				SetPointBuffer(Start.x + i, End.y),
				SetPointBuffer(Start.x + i, Start.y);
			for(int i = 0; i < (Start.y - End.y); i++)
				SetPointBuffer(End.x, End.y + i),
				SetPointBuffer(Start.x, End.y + i);
			return;
		}else{
			for(int i = 0; i < (End.x - Start.x); i++)
				SetPointBuffer(Start.x + i, End.y),
				SetPointBuffer(Start.x + i, Start.y);
			for(int i = 0; i < (End.y - Start.y); i++)
				SetPointBuffer(End.x, Start.y + i),
				SetPointBuffer(Start.x, Start.y + i);
			return;
		}
	}
		
}

/* 画线 */
void DrawLine(TypeXY Start, TypeXY End)
{
  unsigned short us; 
	unsigned short usX_Current, usY_Current;
	
	int lError_X = 0, lError_Y = 0, lDelta_X, lDelta_Y, lDistance; 
	int lIncrease_X, lIncrease_Y; 	

	lDelta_X = End.x - Start.x; //计算坐标增量 
	lDelta_Y = End.y - Start.y; 

	usX_Current = Start.x; 
	usY_Current = Start.y; 

	if ( lDelta_X > 0 ) 
		lIncrease_X = 1; 			//设置单步正方向 
	else if ( lDelta_X == 0 ) 
		lIncrease_X = 0;			//垂直线 
	else 
	{ 
		lIncrease_X = -1;			//设置单步反方向 
		lDelta_X = - lDelta_X;
	} 

	//Y轴的处理方式与上图X轴的处理方式同理
	if ( lDelta_Y > 0 )
		lIncrease_Y = 1; 
	else if ( lDelta_Y == 0 )
		lIncrease_Y = 0;			//水平线 
	else 
	{
		lIncrease_Y = -1;
		lDelta_Y = - lDelta_Y;
	} 

	//选取不那么陡的方向依次画点
	if ( lDelta_X > lDelta_Y )
		lDistance = lDelta_X;
	else 
		lDistance = lDelta_Y; 

	//依次画点进入缓存区
	for ( us = 0; us <= lDistance + 1; us ++ )					//画线输出 
	{
		SetPointBuffer(usX_Current , usY_Current);	//画点 
		lError_X += lDelta_X ; 
		lError_Y += lDelta_Y ; 

		if ( lError_X > lDistance ) 
		{
			lError_X -= lDistance; 
			usX_Current += lIncrease_X; 
		}  

		if ( lError_Y > lDistance ) 
		{ 
			lError_Y -= lDistance; 
			usY_Current += lIncrease_Y; 
		} 		
	}
}

/* 画圆 */
void DrawCircle ( int usX_Center, int usY_Center, int usRadius)
{
	short sCurrentX, sCurrentY;
	short sError;
	sCurrentX = 0; sCurrentY = usRadius;	  
	sError = 3 - ( usRadius << 1 );     //判断下个点位置的标志
	
	while ( sCurrentX <= sCurrentY )
	{
		SetPointBuffer ( usX_Center + sCurrentX, usY_Center + sCurrentY);
		SetPointBuffer ( usX_Center - sCurrentX, usY_Center + sCurrentY);      
		SetPointBuffer ( usX_Center - sCurrentY, usY_Center + sCurrentX);     
		SetPointBuffer ( usX_Center - sCurrentY, usY_Center - sCurrentX);      
		SetPointBuffer ( usX_Center - sCurrentX, usY_Center - sCurrentY);          
		SetPointBuffer ( usX_Center + sCurrentX, usY_Center - sCurrentY);   
		SetPointBuffer ( usX_Center + sCurrentY, usY_Center - sCurrentX);   
		SetPointBuffer ( usX_Center + sCurrentY, usY_Center + sCurrentX);      
		sCurrentX ++;		
		if ( sError < 0 ) 
			sError += 4 * sCurrentX + 6;	  
		else
		{
			sError += 10 + 4 * ( sCurrentX - sCurrentY );   
			sCurrentY --;
		} 		
	}
}

/* 时钟中心坐标 */
TypeXY HALF = {OLED_WIDTH_HALF, OLED_HEIGHT_HALF};
/* 时钟动画 */
void RoundClock(int hours , int minute , int sec)
{
	TypeXY hourspoint = {OLED_WIDTH_HALF - 14, OLED_HEIGHT_HALF};
	TypeXY minutepoint = {OLED_WIDTH_HALF - 21, OLED_HEIGHT_HALF};
	TypeXY secpoint = {OLED_WIDTH_HALF - 28, OLED_HEIGHT_HALF};
	//时针
	_RoateValue = SetRotateValue(OLED_WIDTH_HALF, OLED_HEIGHT_HALF, hours * 30 + (minute * 30) / 60, 1);
	Rotate(&hourspoint, _RoateValue);
	DrawLine(HALF, hourspoint);
	//分针
	_RoateValue = SetRotateValue(OLED_WIDTH_HALF, OLED_HEIGHT_HALF, minute * 6 + (sec * 6) / 60, 1);
	Rotate(&minutepoint, _RoateValue);
	DrawLine(HALF, minutepoint);	
	//秒针
	_RoateValue = SetRotateValue(OLED_WIDTH_HALF, OLED_HEIGHT_HALF, sec * 6, 1);
	Rotate(&secpoint, _RoateValue);
	DrawLine(HALF, secpoint);
	//表盘
	DrawCircle(OLED_WIDTH_HALF, OLED_HEIGHT_HALF, 30);
	TypeXY tmp1 = {HALF.x - 29, HALF.y};
	TypeXY tmp2 = {HALF.x - 24, HALF.y};
	DrawLine(tmp1, tmp2);
	tmp1.x = tmp1.x + 58;
	tmp2.x = tmp2.x + 48;
	DrawLine(tmp1, tmp2);
	tmp1 = HALF; 
	tmp2 = HALF;
	tmp1.y = HALF.y - 29;
	tmp2.y = HALF.y - 24;
	DrawLine(tmp1, tmp2);
	tmp1.y = tmp1.y + 58;
	tmp2.y = tmp2.y + 48;
	DrawLine(tmp1, tmp2);
	for(int i = 1; i < 3; i++)
	{
		_RoateValue = SetRotateValue(OLED_WIDTH_HALF, OLED_HEIGHT_HALF, i * 30, 1);
		tmp1 = HALF; 
		tmp2 = HALF;
		tmp1.x = tmp1.x - 29;
		tmp2.x = tmp2.x - 24;
		Rotate(&tmp1, _RoateValue);
		Rotate(&tmp2, _RoateValue);
		DrawLine(tmp1, tmp2);
		tmp1.y = tmp1.y - 2 * (tmp1.y - HALF.y);
		tmp2.y = tmp2.y - 2 * (tmp2.y - HALF.y);
		DrawLine(tmp1, tmp2);
		tmp1.x = tmp1.x - 2 * (tmp1.x - HALF.x);
		tmp2.x = tmp2.x - 2 * (tmp2.x - HALF.x);
		DrawLine(tmp1, tmp2);
		tmp1.y = tmp1.y - 2 * (tmp1.y - HALF.y);
		tmp2.y = tmp2.y - 2 * (tmp2.y - HALF.y);
		DrawLine(tmp1, tmp2);
	}
}

