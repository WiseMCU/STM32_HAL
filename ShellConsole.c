#include "ShellConsole.h"

/* Include */
#include "string.h"
#include "stdlib.h"

static void help(void);
static void ClearShellUartRx(void);
inline static struct CmdListNode * SearchCmd(char * Cmd);

typedef struct CmdListNode{
	char *Cmd;
	char *Describe;
	void (*Function)();
	struct CmdListNode *next;
}NodeType;

static struct CmdListNode HelpNode = {"help", "Print All CMD", help, NULL};

typedef struct{
	__IO uint8_t				SearchFlag;
	uint16_t 						CmdNum;
	struct CmdListNode 	*hand;
	struct CmdListNode 	*per;
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
	struct CmdListNode *bufNode = SearchCmd(Cmd);
	
	if(bufNode != NULL)
	{
		Print("\r\nAdd [%s] CMD failed,CMD is repeat!", Cmd);
		return HAL_ERROR;
	}
	
	struct CmdListNode *newNode = (struct CmdListNode *)malloc(sizeof(struct CmdListNode));
	
	if(newNode == NULL)
	{
		Print("\r\nAdd [%s] CMD failed,CMD is repeat!", Cmd);
		return HAL_ERROR;
	}
	
	if(strlen(Cmd) > ShellCmdMaxSize)
	{
		newNode->Cmd = (char *)malloc(ShellCmdMaxSize);
		memcpy(newNode->Cmd, Cmd, ShellCmdMaxSize);
	}else{
		newNode->Cmd = (char *)malloc(strlen(Cmd));
		memcpy(newNode->Cmd, Cmd, strlen(Cmd));
	}

	newNode->Describe = (char *)malloc(strlen(Describe));
	memcpy(newNode->Describe, Describe, strlen(Describe) + 1);
	
	newNode->Function = Function;
	
	newNode->next = NULL;
	
	ListHand.per->next = newNode;

	ListHand.per = ListHand.per->next;
	
	ListHand.CmdNum++;
	
	if(ListHand.SearchFlag)
	{
		Print("\r\nAdd [%s] CMD Success", ListHand.per->Cmd);
	}

	return HAL_OK;
}

HAL_StatusTypeDef DelCmd(char *Cmd)
{
	
	struct CmdListNode *bufNode = ListHand.hand;
	struct CmdListNode *DelNode = bufNode->next;
	
	for(uint16_t i = ListHand.CmdNum; i > 1; i--)
	{
		if((strlen(Cmd) == strlen(DelNode->Cmd)) && (strncmp(Cmd, DelNode->Cmd, (strlen(Cmd))) == 0))
		{
			bufNode->next = DelNode->next;
			
			if(DelNode == ListHand.per)
			{
				ListHand.per = bufNode;
			}
			
			free(DelNode);
			
			DelNode = NULL;
			
			ListHand.CmdNum--;
			
			Print("\r\nDelete [%s] CMD Success", Cmd);
			
			return HAL_OK;
		}else{
			bufNode = bufNode->next;
			
			DelNode = bufNode->next;
		}
	}
	
	if(ListHand.SearchFlag)
	{
		Print("\r\nDelete [%s] CMD failed,CMD is invaild!", Cmd);
	}
	
	return HAL_ERROR;
}

inline static void help(void)
{
	struct CmdListNode *bufNode = ListHand.hand;
	Print("\r\n* * * This is [%d] CMD list * * *", ListHand.CmdNum);
	for(uint16_t i = ListHand.CmdNum; i > 0; i--)
	{
		Print("\r\n%-*s - %s", (ShellCmdMaxSize + 2), bufNode->Cmd, bufNode->Describe);
		if(bufNode == ListHand.per)
		{
			Print("\r\n* * * PRINTED ALL CMD * * *");
		}else{
			bufNode = bufNode->next;
		}
	}
}

inline static struct CmdListNode* SearchCmd(char * Cmd) 
{
	struct CmdListNode *bufNode = ListHand.hand;
	if(ShellUart.RxFlag)
	{
		for(uint16_t i = ListHand.CmdNum; i > 0; i--)
		{
			if(((strlen(Cmd) - 1) == strlen(bufNode->Cmd)) & (strncmp(Cmd, bufNode->Cmd, strlen(bufNode->Cmd)) == 0))
			{
				return bufNode;
			}else{
				bufNode = bufNode->next;
			}
		}
	}else{
		for(uint16_t i = ListHand.CmdNum; i > 0; i--)
		{
			if((strlen(Cmd) == strlen(bufNode->Cmd)) & (strncmp(Cmd, bufNode->Cmd, strlen(bufNode->Cmd)) == 0))
			{
				return bufNode;
			}else{
				bufNode = bufNode->next;
			}
		}
	}
	return NULL;
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
	
	Print("\r\nCMD>");
	
	return	ShellUart.StartRx(ShellUart.UartHandle, &ShellUart.RxData[rxLen], ShellConsoleMaxSize);
}



void ShellFunction(void)
{
	if(ListHand.SearchFlag == 0)
	{
		ListHand.SearchFlag = 1;
	}
	
	if(ShellUart.RxFlag)
	{
		if(ShellUart.RxLen)
		{
			#if SHELL_USE_ECHO
			Print("\r\nRX[%d]>%s", ShellUart.RxLen, ShellUart.RxData);
			#endif
			
			if((ShellUart.RxLen <= ShellCmdMaxSize))
			{
				struct CmdListNode *bufNode = NULL;
				bufNode =	SearchCmd((char*)ShellUart.RxData);
				ShellUart.RxFlag = 0;
				if(bufNode == NULL)
				{
					Print("\r\nInvalid [%.*s] CMD,Input help or push [TAB] get CMD", ShellUart.RxLen, ShellUart.RxData);
				}else{
					bufNode->Function();
				}
			}else{
				Print("\r\nError CMD Maxsize [%d]", ShellCmdMaxSize);
			}
		}
		ClearShellUartRx();
	}
}

inline static void CheckCmd(uint16_t Size)
{
	switch(ShellUart.RxData[rxLen + Size - 1])
	{
		case '\r':
			ShellUart.RxFlag = 1;
			ShellUart.RxLen  = rxLen + Size - 1;
			rxLen = 0;
			break;
		
		default:
			Print("%.*s", Size, &ShellUart.RxData[rxLen]);
			rxLen += Size;
			ShellUart.StartRx(ShellUart.UartHandle, &ShellUart.RxData[rxLen], (ShellConsoleMaxSize - rxLen));
			break;
	}
}

inline static int ReplenishCmd(void)
{
	if(rxLen == 0)
	{
		return HAL_ERROR;
	}
	struct CmdListNode *bufNode = ListHand.hand;
	for(uint16_t i = ListHand.CmdNum; i > 0; i--)
	{
		if((rxLen <= strlen(bufNode->Cmd)) & (strncmp((char*)ShellUart.RxData, bufNode->Cmd, rxLen) == 0))
		{
			Print("%s", &bufNode->Cmd[rxLen]);
			rxLen = strlen(bufNode->Cmd);
			memcpy(ShellUart.RxData, bufNode->Cmd, rxLen);
			return HAL_OK;
		}else{
			bufNode = bufNode->next;
		}
	}
	return HAL_ERROR;
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if(huart->Instance == ShellUart.UartHandle->Instance)
	{
		if (huart->ReceptionType == HAL_UART_RECEPTION_STANDARD)
    {
			if(Size == 1)
			{
				switch(ShellUart.RxData[rxLen])
				{
					/* 删除键 */
					case 8:
						if(rxLen)
						{
							rxLen--;
							Print("%c%c%c", 0x08, 0x20, 0x08);
						}
						ShellUart.StartRx(ShellUart.UartHandle, &ShellUart.RxData[rxLen], (ShellConsoleMaxSize - rxLen));
						break;
					/* TAB键 */
					case 9:
						if(ReplenishCmd())
						{
							HelpNode.Function();
							ClearShellUartRx();
						}else{
							ShellUart.StartRx(ShellUart.UartHandle, &ShellUart.RxData[rxLen], (ShellConsoleMaxSize - rxLen));
						}
						break;
					/* 回车键 */
					case '\r':
						if(rxLen)
						{
							ShellUart.RxFlag = 1;
							ShellUart.RxLen  = rxLen + Size - 1;
							rxLen = 0;
						}else{
							ClearShellUartRx();
						}
						break;
					/* 其他键 */
					default:
						Print("%.*s", Size, &ShellUart.RxData[rxLen]);
						rxLen += Size;
						ShellUart.StartRx(ShellUart.UartHandle, &ShellUart.RxData[rxLen], (ShellConsoleMaxSize - rxLen));
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

