# V1.0

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
OLED_ShowString(0,0,"Hello World!");
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

# V2.0

**改动：**

**添加应用层，可以在缓存自由绘画再将缓存更新到屏幕上**

**添加中文格式字库，只有几个中文，如果需要其他的参照格式自行添加**

**将字库从oled.c中移动到fonts.c**

**字符显示改为调用字库结构体**

**添加几个图案，如果要显示其他图案可以参考格式**

**添加时钟动画**

```c
/* 字符显示改动 */
OLED_ShowString(0, 0, &Font_8x16, "Hello World!");

/* 变量显示改动 */
OLED_ShowString(0, 0, &Font_8x16， "a:%d",a++);

/* 显示一个中文字符 */
OLED_ShowChineseChar(0, 0, &CN16Font_16x16, "日");

/* 显示一串中文 */
OLED_ShowString(0, 0, &CN16Font_16x16, "年月日");

/* 显示设置图案 */
OLED_FILL(bmp);

/* 时钟动画 */
RoundClock(12， 34， 56);//把时钟画在缓存上 12:34:56
UpdateScreen();//将缓存更新到屏幕
```

应用层调用方法与上面基本一致，可以自行测试





感谢学弟 @尹旭阳 支持