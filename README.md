---

title: OLED的使用
date: 2022-01-03 17:13:39
tags:

-STM32CubeMX

---

将C文件和头文件添加进工程；

修改oled.h里面的IIC的句柄

```c
#define I2C hi2c2
```

使用OLED前先初始化

```c
OLED_Init();
```

函数说明：

```c
/**
  * uint16_t x 显示的横坐标，加一向右移动1个像素点
  * uint16_t y 显示的纵坐标，加一向下移动8个像素点
  * 类似printf()输入方式
  */
void OLED_ShowString(uint16_t x,uint16_t y,const char *fmt, ...);
```

使用示例1:

```c
OLED_ShowString(0,0,"Hello World!")
```

使用示例2：

```c
int a = 0;
while(1)
{
	OLED_ShowString(0,0,"a:%d",a++);
	HAL_Delay(500);
}
```

