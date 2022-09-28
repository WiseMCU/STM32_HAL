# 基于STM32的轻量控制台V2.0（by欧阳云鹏）

### 版本更新

> 2022.8.17：V1.0	
>
> 2022.8.17：V1.1   修复组件指针错误
>
> 2022.8.17：V1.2   添加Backspace按键支持
>
> 2022.8.19：V1.3   添加TAB键复用功能，TAB初始功能为查询指令功能，同时添加指令补全功能
>
> 2022.8.20：V1.4   添加ShellGetStrCMD函数
>
> 2022.9.27：V2.0   可以稳定运行，添加指令不可见，添加指令不可删除，添加Ymodem协议接收端功能全实现，支持Ymodem-1K（完美兼容Xshell），添加重要注释
>
> 2022.9.28：V2.1   修复Ymodem连续接收大文件偶尔超时的BUG

### V1.0宏开关说明

```c
/* 接收最大长度 
接收空间，尽量开大一点，单次指令长度不要超过这个
*/
#define ShellConsoleMaxSize 			256

/* 指令最大长度 
指令长度如果超过这个，在查询指令里面只显示前MaxSize位指令，运行指令使用时也只需要输入前MaxSize位指令
*/
#define ShellCmdMaxSize 				16

/* 串口打印接口 
自定义组件打印接口
*/
#define Print 							PrintfDebug

/* 是否开启回显 
如果开启回显，回车确认指令时组件会再打印一遍运行的指令
*/
#define SHELL_USE_ECHO 					1
```

### V1.0函数说明

```c
/* 初始化 
 @参数1：串口句柄
*/
HAL_StatusTypeDef ShellConsoleInit(UART_HandleTypeDef* huart);

/* 循环调用 */
void ShellFunction(void);

/* 添加指令 
 @参数1：指令名称
 @参数2：指令描述
 @参数3：指令函数（运行的函数可以带参数带返回值，但是会报警告无视就好，而且目前在输入指令时不支持输入参数，所以并没有实际意义同无参数无返回值的函数没有区别）
 @return： 成功返回HAL_OK
*/
HAL_StatusTypeDef AddCmd(char *Cmd, char * Describe, void (*Function)());

/* 删除指令 
 @参数1：指令名称
 @return： 成功返回HAL_OK
*/
HAL_StatusTypeDef DelCmd(char *Cmd);

/* 阻塞方式获取下一条输入的指令
 @return： 成功返回获取的指令字符串指针 
里面阻塞可以在操作系统里面挂起
*/
uint8_t* ShellGetStrCMD(void);
```

### 组件说明

> 已知BUG：如果单次指令长度超过**接收最大长度**会导致程序崩溃，目前仅修复了Xshell，所以建议使用Xshell调试该组件或者将接收最大长度增大并确保单次指令长度不超过接收最大长度。
>
> 组件使用的串口一定要**开启中断！！！**否则会无法输入，初始化里面已经做了DMA的适配，Rx_DMA开不开组件都可以正常使用，只需要默认的Rx_DMA配置就可以无须做任何配置，Tx_DMA并没有用到。
>
> 组件使用与其他命令行操作一致，输入指令按回车确定，如果使用的其他串口软件（ATKXCOM等）需要注意回车操作相当于用16进制发送‘0D’
>
> 组件内置查询指令，输入【help + 回车】或者按【TAB】键都可以查询指令列表
>
> 组件支持指令补全，输入指令的开头按【TAB】补全

### 例程DEMO

```c
void test1()
{
	PrintfDebug("\r\n 我是一号测试！I am Test 1!");
}

void test2()
{
	PrintfDebug("\r\n 我是二号测试！ I am Test 2!");
}

void del()
{
	PrintfDebug("\r\n 删除测试一！ Delete Test 1!");
	ShellDelCmd("t1");
}

void del_help()
{
	PrintfDebug("\r\n 删除查询指令！ Delete help!");
	ShellDelCmd("help");
}

void add()
{
	PrintfDebug("\r\n 添加测试一！ add Test 1!");
	ShellAddCmd("t1", "Printf test 1", test1);//添加测试指令1
}

void UartTx(void)
{
	uint8_t* Cmd = ShellGetStrCMD();
	PrintfDebug("\r\nGetStr[%.*s]", (strlen((char*)Cmd) - 1), Cmd);
}

int main(void)
{
	PrintfDebug("Start");
	ShellConsoleInit(&huart1);
	ShellAddCmd("t1", "运行test1程序", test1);
	ShellAddCmd("t2", "运行test2程序", test2);
	ShellAddCmd("t3", "运行UartTx程序", UartTx);
	ShellAddCmd("del", "删除t1指令", del);
	ShellAddCmd("add", "添加t1指令", add);
	ShellAddCmd("del help", "删除help指令", del_help);
  while (1)
  {
		ShellFunction();//需要循环调用，才可以运行
  }
}
```

### PrintfDebug原型

```c
#include "stdio.h"
#include "stdarg.h"
#define CONSOLEBUF_SIZE 256
static char Uart_buf[CONSOLEBUF_SIZE];
void PrintfDebug(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	int length = vsnprintf(Uart_buf, sizeof(Uart_buf) - 1, fmt, args);
	va_end(args);
	HAL_UART_Transmit(&huart1,(uint8_t *)Uart_buf, length, 0xff);
}
```

