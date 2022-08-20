#ifndef __SHELLCONSOLE_H
#define __SHELLCONSOLE_H

#include "main.h"
#include "usart.h"

#include "cmd.h"

/* 接收最大长度 */
#define ShellConsoleMaxSize 		    256

/* 指令最大长度 */
#define ShellCmdMaxSize 				16

/* 串口打印接口 */
#define Print 							PrintfDebug

/* 是否开启回显 */
#define SHELL_USE_ECHO 					1

/* 初始化 */
HAL_StatusTypeDef ShellConsoleInit(UART_HandleTypeDef* huart);

/* 循环调用 */
void ShellFunction(void);

/* 添加指令 */
HAL_StatusTypeDef ShellAddCmd(char *Cmd, char * Describe, void (*Function)());

/* 删除指令 */
HAL_StatusTypeDef ShellDelCmd(char *Cmd);

/* 阻塞方式获取下一条输入的指令字符串指针 */
uint8_t* ShellGetStrCMD(void);

/*  DEMO

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
	PrintfDebug("Start\r\n");
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
*/

#endif
