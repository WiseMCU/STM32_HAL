#include "ShellConsole.h"

/* Include */
#include "string.h"
#include "stdlib.h"

static void help(void);
static void ClearShellUartRx(void);

typedef struct CmdListNode{
	char *Cmd;
	char *Describe;
	void (*Function)();
	struct CmdListNode *next;
}NodeType;

static struct CmdListNode HelpNode = {"help", "Print All CMD", help, NULL};

typedef struct{
	uint16_t CmdNum;
	struct CmdListNode *hand;
	struct CmdListNode *per;
}HandNodeType;

static HandNodeType ListHand = {0};

typedef struct
{
	UART_HandleTypeDef* UartHandle;
	uint8_t							RxData[ShellConsoleMaxSize];
	__IO uint8_t				RxFlag;
	__IO uint16_t				RxLen;
	HAL_StatusTypeDef (*StartRx)(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
}ShellConsoleType;

static ShellConsoleType ShellUart;

static uint32_t rxLen = 0;


/* List */

HAL_StatusTypeDef AddCmd(char *Cmd, char * Describe, void (*Function)())
{
	
	struct CmdListNode *newNode = ListHand.hand;
	
	for(uint16_t i = ListHand.CmdNum; i > 0; i--)
	{
		if(strcmp(Cmd, newNode->Cmd) == 0)
		{
			Print("\r\nAdd [%s] CMD failed,CMD is repeat!", Cmd);
			return HAL_ERROR;
		}else{
			newNode = newNode->next;
		}
	}
	newNode = NULL;
	newNode = malloc(sizeof(struct CmdListNode));
	
	ListHand.per->next = newNode;
	
	ListHand.per = newNode;
	
	ListHand.CmdNum++;
	
	if(strlen(Cmd) > ShellCmdMaxSize)
	{
		newNode->Cmd = malloc(ShellCmdMaxSize);
		memcpy(newNode->Cmd, Cmd, ShellCmdMaxSize);
	}else{
		newNode->Cmd = malloc(strlen(Cmd));
		memcpy(newNode->Cmd, Cmd, strlen(Cmd));
	}

	newNode->Describe = malloc(strlen(Describe));
	memcpy(newNode->Describe, Describe, strlen(Describe) + 1);
	
	newNode->Function = Function;
	
	Print("\r\nAdd [%s] CMD Success", Cmd);
	
	return HAL_OK;
}

HAL_StatusTypeDef DelCmd(char *Cmd)
{
	struct CmdListNode *bufNode = ListHand.hand;
	struct CmdListNode *DelNode = bufNode->next;
	
	for(uint16_t i = ListHand.CmdNum; i > 0; i--)
	{
		if(strcmp(Cmd, DelNode->Cmd) == 0)
		{
			bufNode->next = DelNode->next;
			free(DelNode);
			ListHand.CmdNum--;
			Print("\r\nDelete [%s] CMD Success", Cmd);
			return HAL_OK;
		}else{
			bufNode = bufNode->next;
			DelNode = bufNode->next;
		}
	}
	Print("\r\nDelete [%s] CMD failed,CMD is invaild!", Cmd);
	return HAL_ERROR;
}

inline static void help(void)
{
	struct CmdListNode *bufNode = ListHand.hand;
	Print("\r\n* * * This is CMD list * * *");
	for(uint16_t i = ListHand.CmdNum; i > 0; i--)
	{
		Print("\r\n%-*s - %s", (ShellCmdMaxSize + 2), bufNode->Cmd, bufNode->Describe);
		if(i == 1)
		{
			Print("\r\n* * * PRINTED ALL CMD * * *");
		}else{
			bufNode = bufNode->next;
		}
	}
}

inline static void SearchCmdError(ShellConsoleType *Uart)
{
	Print("\r\nInvalid [%.*s] CMD,Input help or push [TAB] get CMD", Uart->RxLen, Uart->RxData);
}

static void SearchCmd(ShellConsoleType *Uart) 
{
	struct CmdListNode *bufNode = ListHand.hand;
	for(uint16_t i = ListHand.CmdNum; i > 0; i--)
	{
		if(strncmp((char*)Uart->RxData, bufNode->Cmd, Uart->RxLen) == 0)
		{
			bufNode->Function();
			return;
		}else{
			bufNode = bufNode->next;
		}
	}
	SearchCmdError(Uart);
}


/* Shell */

HAL_StatusTypeDef ShellConsoleInit(UART_HandleTypeDef* huart)
{
	ListHand.CmdNum = 1;
	ListHand.hand = &HelpNode;
	ListHand.per = &HelpNode;

	ShellUart.UartHandle = huart;
	
	if(ShellUart.UartHandle->hdmarx)
	{
		ShellUart.StartRx = HAL_UARTEx_ReceiveToIdle_DMA;
	}else{
		ShellUart.StartRx = HAL_UARTEx_ReceiveToIdle_IT;
	}
	return	ShellUart.StartRx(ShellUart.UartHandle, &ShellUart.RxData[rxLen], ShellConsoleMaxSize);
}



void ShellFunction(void)
{
	if(ShellUart.RxFlag)
		{
			if(ShellUart.RxLen)
			{
				#if SHELL_USE_ECHO
				Print("\r\nRX[%d]>%s", ShellUart.RxLen, ShellUart.RxData);
				#endif
				
				if((ShellUart.RxLen <= ShellCmdMaxSize))
				{
					SearchCmd(&ShellUart);
				}else{
					SearchCmdError(&ShellUart);
				}
			}
			ClearShellUartRx();
		}
}

inline static void CheckCmd(uint16_t Size)
{
	if(ShellUart.RxData[rxLen + Size - 1] == '\r')
	{
			ShellUart.RxFlag = 1;
			ShellUart.RxLen  = rxLen + Size - 1;
			rxLen = 0;
	}else{
			Print("%.*s", Size, &ShellUart.RxData[rxLen]);
			rxLen += Size;
			ShellUart.StartRx(ShellUart.UartHandle, &ShellUart.RxData[rxLen], (ShellConsoleMaxSize - rxLen));
	}
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if(huart->Instance == ShellUart.UartHandle->Instance)
	{
		if (huart->ReceptionType == HAL_UART_RECEPTION_STANDARD)
    {
			if((Size == 1) & (rxLen == 0))
			{
				switch(ShellUart.RxData[0])
				{
					case 9:
						HelpNode.Function();
						ClearShellUartRx();
						break;
					default: 
						CheckCmd(Size);
						break;
				}
			}else{
				CheckCmd(Size);
			}
    }
		
    if (huart->ReceptionType == HAL_UART_RECEPTION_TOIDLE)
    {
			if((ShellConsoleMaxSize - rxLen - Size) == 1)
			{
				Print("\r\nError Shell Overflow MaxSize [%d]", ShellConsoleMaxSize);
				ClearShellUartRx();
			}
    }
	}
}

inline static void ClearShellUartRx(void)
{
	memset(ShellUart.RxData, 0, ShellConsoleMaxSize);
	ShellUart.RxLen = 0;
	rxLen = 0;
	ShellUart.RxFlag = 0;
	ShellUart.StartRx(ShellUart.UartHandle, ShellUart.RxData, ShellConsoleMaxSize);
	Print("\r\nCMD>");
}

