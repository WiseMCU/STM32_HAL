#ifndef __SHELLCONSOLE_H
#define __SHELLCONSOLE_H

#include "main.h"
#include "usart.h"

/* 接收最大长度 */
#define ShellConsoleMaxSize 		256

/* 指令最大长度 */
#define ShellCmdMaxSize 				16

/* 串口打印接口 */
#define Print 									PrintfDebug

/* 是否开启回显 */
#define SHELL_USE_ECHO 					1

/* 初始化 */
HAL_StatusTypeDef ShellConsoleInit(UART_HandleTypeDef* huart);

/* 循环调用 */
void ShellFunction(void);

/* 添加指令 */
HAL_StatusTypeDef AddCmd(char *Cmd, char * Describe, void (*Function)());

/* 删除指令 */
HAL_StatusTypeDef DelCmd(char *Cmd);

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
	DelCmd("t1");
}

void del_help()
{
	PrintfDebug("\r\n 删除查询指令！ Delete help!");
	DelCmd("help");
}

void add()
{
	PrintfDebug("\r\n 添加测试一！ add Test 1!");
	AddCmd("t1", "Printf test 1", test1);//添加测试指令1
}

int main(void)
{
	PrintfDebug("Start\r\n");
	ShellConsoleInit(&huart1);
	AddCmd("t1", "运行test1程序", test1);
	AddCmd("t2", "运行test2程序", test2);
	AddCmd("del", "删除t1指令", del);
	AddCmd("add", "添加t1指令", add);
	AddCmd("del help", "删除help指令", del_help);
  while (1)
  {
		ShellFunction();//需要循环调用，才可以运行
  }
}
*/

#endif
