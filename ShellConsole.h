#ifndef __SHELLCONSOLE_H
#define __SHELLCONSOLE_H

/* 轻量的串口控制台(原创)
 * 版本: 2.0
 * 来源: https://gitee.com/qq3162173168/STM32CubeMX
 */

#include "usart.h"

/* 接收最大长度 */
#define ShellConsoleMaxSize 		2048

/* 指令最大长度 */
#define ShellCmdMaxSize 				16

/* 指令台输出接口 */
#define Print 									PrintfDebug

/* 指令台毫秒级延时接口 */
#define ShellDelay 							HAL_Delay

/* 指令台申请空间接口 */
#define ShellMalloc 						malloc

/* 指令台释放空间接口 */
#define ShellFree 							free

/* 是否开启回显 */
#define SHELL_USE_ECHO 					0

/* 回溯指令数量 */
#define MEMORY_CMD 							3

/* 是否开启Ymodem传输文件(完美兼容Xshell) */
#define USE_YMODEM 							1
#if USE_YMODEM
	#include "fatfs.h"
	/* Ymodem起始指令(最好大于三个字节) */
	#define YmodemCmd "rb -E"
	/* Ymodem帧超时时间 */
	#define YMODEM_WAIT_TIME 200
#endif

/* 初始化 */
HAL_StatusTypeDef ShellConsoleInit(UART_HandleTypeDef* huart);

/* 循环调用 */
void ShellFunction(void);

/* 热更新接收串口 */
void UpShellUartHandle(UART_HandleTypeDef *huart);

/* 添加指令,Del为假(0)表示该指令添加后不可被删除 */
HAL_StatusTypeDef ShellAddCmd(char *Cmd, char * Describe, void (*Function)(), _Bool Del);

/* 删除指令 */
HAL_StatusTypeDef ShellDelCmd(char *Cmd);

/* 设置指令是否可见，默认为1:可见 */
HAL_StatusTypeDef ShellSetCmdShow(char *Cmd, _Bool Show);

/* 阻塞方式获取下一条输入的指令字符串指针 */
uint8_t* ShellGetStrCMD(void);

enum CmdState
{
	WaitCMD = 0,
	GetCMD,
#if USE_YMODEM
	YmodemWait,
	YmodemGet,
	YmodemNext,
	YmodemRepeat,
	YmodemOk,
	YmodemEOH,
	YmodemQuit,
	YmodemOut,
#endif
};

typedef struct
{
	UART_HandleTypeDef* UartHandle;
	uint8_t							RxData[ShellConsoleMaxSize];
	__IO enum CmdState	RxFlag;
	__IO uint32_t				RxLen;
	HAL_StatusTypeDef (*StartRx)(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
}ShellConsoleType;

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
	ShellAddCmd("t1", "Printf test 1", test1, 1);//添加测试指令1
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
	ShellAddCmd("t1", "运行test1程序", test1, 1);
	ShellAddCmd("t2", "运行test2程序", test2, 1);
	ShellAddCmd("t3", "运行UartTx程序", UartTx, 1);
	ShellAddCmd("del", "删除t1指令", del, 1);
	ShellAddCmd("add", "添加t1指令", add, 1);
	ShellAddCmd("del help", "删除help指令", del_help, 1);
  while (1)
  {
		ShellFunction();//需要循环调用，才可以运行
  }
}
*/

#endif
