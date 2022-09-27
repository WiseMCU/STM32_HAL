#include "ShellConsole.h"

/* Include */
#include "string.h"
#include "stdlib.h"

static const char UpKey[3]   = {0x1B, 0x5B, 0x41};
static const char DownKey[3] = {0x1B, 0x5B, 0x42};

static void help(void);
static void ClearShellUartRx(void);

typedef struct CmdListNode{
	char *Cmd;								//指令名称
	char *Describe;						//描述
	void (*Function)();				//函数指针
	struct CmdListNode *next;	//下一个节点
	struct CmdListNode *per;	//上一个节点
	_Bool DelFlag;						//true:可以删除
	_Bool ShowFlag;						//true:可以被help显示
}NodeType;

static struct CmdListNode HelpNode = {"help", "Print All CMD", help, NULL, NULL, 0, 1};

typedef struct{
	__IO uint8_t				PrintFlag;
	uint16_t 						CmdNum;
	struct CmdListNode 	*hand;
	struct CmdListNode 	*end;
}HandNodeType;

static HandNodeType ListHand = {0};

static ShellConsoleType ShellUart;

/* List */
/* 在指令列表里面寻找Cmd，找到了返回指针，否则返回NULL */
inline static struct CmdListNode* SearchCmd(char *Cmd) 
{
	struct CmdListNode *bufNode = ListHand.hand;
	uint16_t CmdSize = strlen(Cmd);
	
	for(uint16_t i = 0; i < ListHand.CmdNum; i++)
	{
		if((CmdSize == strlen(bufNode->Cmd)) & (strncmp(Cmd, bufNode->Cmd, CmdSize) == 0))
		{
			//找到了
			return bufNode;
		}else{
			bufNode = bufNode->next;
		}
	}
	//没找到
	return NULL;
}

HAL_StatusTypeDef ShellAddCmd(char *Cmd, char * Describe, void (*Function)(), _Bool Del)
{
	struct CmdListNode *bufNode = SearchCmd(Cmd);
	//查找是否重复
	if(bufNode != NULL)
	{
		Print("\r\nAdd [%s] CMD failed,CMD is repeat!", Cmd);
		return HAL_ERROR;
	}
	
	struct CmdListNode *newNode = (struct CmdListNode *)ShellMalloc(sizeof(struct CmdListNode));
	//空间是否申请成功
	if(newNode == NULL)
	{
		Print("\r\nAdd [%s] CMD failed,No Memory Space!", Cmd);
		return HAL_ERROR;
	}else{
		newNode->DelFlag = Del;
	}
	//复制指令名字
	if(strlen(Cmd) > ShellCmdMaxSize)
	{
		newNode->Cmd = (char *)ShellMalloc(ShellCmdMaxSize + 1);
		newNode->Cmd[ShellCmdMaxSize] = 0;
		memcpy(newNode->Cmd, Cmd, ShellCmdMaxSize);
	}else{
		newNode->Cmd = (char *)ShellMalloc(strlen(Cmd) + 1);
		newNode->Cmd[strlen(Cmd)] = 0;
		memcpy(newNode->Cmd, Cmd, strlen(Cmd));
	}
	//复制指令简介
	newNode->Describe = (char *)ShellMalloc(strlen(Describe));
	memcpy(newNode->Describe, Describe, strlen(Describe) + 1);
	//设置指令函数
	newNode->Function = Function;
	//默认指令可见
	newNode->ShowFlag = 1;

	newNode->next = NULL;
	
	newNode->per = ListHand.end;
	
	ListHand.end->next = newNode;

	ListHand.end = newNode;
	
	ListHand.CmdNum++;
	
	if(ListHand.PrintFlag)
	{
		Print("\r\nAdd [%s] CMD Success", ListHand.end->Cmd);
	}

	return HAL_OK;
}

HAL_StatusTypeDef ShellDelCmd(char *Cmd)
{
	struct CmdListNode *DelNode = SearchCmd(Cmd);
	//查找是否重复
	if(DelNode == NULL)
	{
		if(ListHand.PrintFlag)
		{
			Print("\r\nDelete [%s] CMD failed,CMD is invaild!", Cmd);
		}
		return HAL_ERROR;
	}else{
		//指令是否可以删除
		if(DelNode->DelFlag)
		{
			//因为头指令是help指令不会被删除，所以不用判断是不是第一个指令
			//是否删除的是最后一个指令
			if(DelNode == ListHand.end)
			{
				ListHand.end = DelNode->per;
				ListHand.end->next = NULL;
			}else{
				DelNode->per->next = DelNode->next;
				DelNode->next->per = DelNode->per;
			}
			ShellFree(DelNode->Cmd);
			ShellFree(DelNode->Describe);
			ShellFree(DelNode);
			DelNode = NULL;
			ListHand.CmdNum--;
			Print("\r\nDelete [%s] CMD Success", Cmd);
			return HAL_OK;
		}else{
			Print("\r\nDelete [%s] CMD failed,CMD is Not Allow Del!", DelNode->Cmd);
			return HAL_ERROR;
		}
	}
}

HAL_StatusTypeDef ShellSetCmdShow(char *Cmd, _Bool Show)
{
	struct CmdListNode *Node = SearchCmd(Cmd);
	//查找是否重复
	if(Node == NULL)
	{
			Print("\r\nSet [%s] CMD failed,CMD is invaild!", Cmd);
	}else{
		//设置指令是否可见
		Node->ShowFlag = Show;
		return HAL_OK;
	}
	return HAL_ERROR;
}

inline static void help(void)
{
	struct CmdListNode *bufNode = ListHand.hand;
	Print("\r\n* * * This is [%d] CMD list * * *", ListHand.CmdNum);
	for(uint16_t i = 0; i < ListHand.CmdNum; i++)
	{
		if(bufNode->ShowFlag)
		{
			Print("\r\n%-*s - %s", (ShellCmdMaxSize + 2), bufNode->Cmd, bufNode->Describe);
		}
		
		if(bufNode == ListHand.end)
		{
			Print("\r\n* * * PRINTED ALL CMD * * *");
		}else{
			bufNode = bufNode->next;
		}
	}
}

/* Shell */

uint8_t* ShellGetStrCMD(void)
{
	memset(ShellUart.RxData, 0, ShellConsoleMaxSize);
	ShellUart.RxLen = 0;
	ShellUart.RxFlag = WaitCMD;
	ShellUart.StartRx(ShellUart.UartHandle, ShellUart.RxData, ShellConsoleMaxSize);
	Print("\r\n>");
	while(ShellUart.RxFlag == WaitCMD)
	{
		/* 操作系统挂起放在这里 */
	}
	ShellUart.RxData[ShellUart.RxLen] = 0;
	return (ShellUart.RxData);
}

#if MEMORY_CMD
static HandNodeType RmbCMDHand = {0};
static void MEMREY_CMD_ADD(struct CmdListNode *Node)
{
	struct CmdListNode *newNode = RmbCMDHand.hand;
	RmbCMDHand.end = NULL;//用尾指针表示当前显示的
	
	for(uint16_t i = 0; i < RmbCMDHand.CmdNum; i++)
	{
		if(newNode->Cmd == Node->Cmd)
		{
			//如果有重复指令就把指令放到头指令
			while(newNode != RmbCMDHand.hand)
			{
				newNode->Cmd = newNode->next->Cmd;
				newNode = newNode->next;
			}
			newNode->Cmd = Node->Cmd;
			return;
		}else{
			newNode = newNode->next;
		}
	}
	
	switch(RmbCMDHand.CmdNum)
	{
		case 0:
			newNode = (struct CmdListNode *)ShellMalloc(sizeof(struct CmdListNode));
			//空间是否申请成功
			if(newNode == NULL)
			{
				Print("\r\nMemory [%s] CMD failed,No Memory Space!", Node->Cmd);
				return;
			}
			//设置指令名字
			newNode->Cmd = Node->Cmd;
	
			newNode->next = newNode;
			newNode->per = newNode;
		
			RmbCMDHand.hand = newNode;//用头指针指向最近的指令
			RmbCMDHand.CmdNum++;
			break;
		
		case MEMORY_CMD:
			newNode = RmbCMDHand.hand->next;
			for(uint16_t i = 1; i < MEMORY_CMD; i++)
			{
				newNode->Cmd = newNode->next->Cmd;
				newNode = newNode->next;
			}
			RmbCMDHand.hand->Cmd = Node->Cmd;
			break;
		
		default:
			newNode = (struct CmdListNode *)ShellMalloc(sizeof(struct CmdListNode));
			//空间是否申请成功
			if(newNode == NULL)
			{
				Print("\r\nMemory [%s] CMD failed,No Memory Space!", Node->Cmd);
				return;
			}
			//设置指令名字
			newNode->Cmd = Node->Cmd;
	
			RmbCMDHand.hand->next->per = newNode;
			newNode->next = RmbCMDHand.hand->next;
		
			RmbCMDHand.hand->next = newNode;
			newNode->per = RmbCMDHand.hand;
			
			RmbCMDHand.hand = newNode;
			RmbCMDHand.CmdNum++;
			break;
	}
}
#endif

/* 判断是否有指令待运行 */
void ShellFunction(void)
{
	if(ListHand.PrintFlag == 0)
	{
		ListHand.PrintFlag = 1;
	}
	
	if(ShellUart.RxFlag == GetCMD)
	{
		static struct CmdListNode *bufNode = NULL;
		if(ShellUart.RxLen)
		{
			#if SHELL_USE_ECHO
			Print("\r\nRX[%d]>%s", ShellUart.RxLen, ShellUart.RxData);
			#endif
			
			if((ShellUart.RxLen > ShellCmdMaxSize))
			{
				Print("\r\nError CMD Maxsize [%d]", ShellCmdMaxSize);
			}else{
				ShellUart.RxData[ShellUart.RxLen] = 0;
				bufNode =	SearchCmd((char*)ShellUart.RxData);
				if(bufNode == NULL)
				{
					Print("\r\nInvalid [%.*s] CMD,Input help or push [TAB] get CMD", ShellUart.RxLen, ShellUart.RxData);
				}else{
					#if MEMORY_CMD
						MEMREY_CMD_ADD(bufNode);
					#endif
					bufNode->Function();
				}
			}
		}
		ClearShellUartRx();
	}
}

#if USE_YMODEM
/*
*********************************************************************************************************
*	函 数 名: UpdateCRC16
*	功能说明: 上次计算的CRC结果 crcIn 再加上一个字节数据计算CRC
*	形    参: crcIn 上一次CRC计算结果
*             byte  新添加字节
*	返 回 值: 无
*********************************************************************************************************
*/
static uint16_t UpdateCRC16(uint16_t crcIn, uint8_t byte)
{
  uint32_t crc = crcIn;
  uint32_t in = byte | 0x100;

  do
  {
	crc <<= 1;
	in <<= 1;
	if(in & 0x100)
		++crc;
	if(crc & 0x10000)
		crc ^= 0x1021;
  }while(!(in & 0x10000));

  return crc & 0xffffu;
}

/*
*********************************************************************************************************
*	函 数 名: Cal_CRC16
*	功能说明: 计算一串数据的CRC
*	形    参: data  数据
*             size  数据长度
*	返 回 值: CRC计算结果
*********************************************************************************************************
*/
static uint16_t Cal_CRC16(const uint8_t* data, uint32_t size)
{
	uint32_t crc = 0;
	const uint8_t* dataEnd = data+size;

	while(data < dataEnd)
	{
		crc = UpdateCRC16(crc, *data++);
	}

	crc = UpdateCRC16(crc, 0);
	crc = UpdateCRC16(crc, 0);

	return (crc & 0xffffu);
}

/* Ymodem文件剩余字节数 */
static uint32_t YmodemSizeTotal;
/* 包号 */
static uint32_t YmodemPackNum;
/* 文件对象 */
static FIL YmodemFile;
/* 帧数据指针 */
static char *YmodemData;
/* 文件名称 */
static char YmodemName[50];
/* Ymodem指令 */
#define SOH 0x01
#define STX 0x02
#define EOT 0x04
#define ACK 0x06
#define NAK 0x15
#define CA 	0x18
#define C	  0x43

/* 处理当前帧，成功返回1 */
static void DealYmodemDataFrame(char Index)
{
	uint32_t fnum;
	if(YmodemPackNum)
	{
		/* 数据帧 */
		switch(Index)
		{
			/* 短数据帧 */
			case SOH:
				if(YmodemSizeTotal > 128)
				{
					/* 中间帧 */
					if(f_write(&YmodemFile, YmodemData, 128, &fnum) == FR_OK)
					{
						if(fnum == 128)
						{
							/* 写入成功 */
							YmodemSizeTotal -= 128;
							/* 接收成功等待下一帧 */
							ShellUart.RxFlag = YmodemNext;
						}
					}
				}else{
					/* 最后一帧 */
					if(f_write(&YmodemFile, YmodemData, YmodemSizeTotal, &fnum) == FR_OK)
					{
						if(fnum == YmodemSizeTotal)
						{
							/* 写入成功 */
							YmodemSizeTotal = 0;
							/* 接收完毕,等待EOT */
							ShellUart.RxFlag = YmodemNext;
						}
					}
				}
				break;
				
			case STX:
				/* 长数据帧 */
				if(YmodemSizeTotal > 1024)
				{
					/* 中间帧 */
					if(f_write(&YmodemFile, YmodemData, 1024, &fnum) == FR_OK)
					{
						if(fnum == 1024)
						{
							/* 写入成功 */
							YmodemSizeTotal -= 1024;
							/* 接收成功等待下一帧 */
							ShellUart.RxFlag = YmodemNext;
						}
					}
				}else{
					/* 最后一帧 */
					if(f_write(&YmodemFile, YmodemData, YmodemSizeTotal, &fnum) == FR_OK)
					{
						if(fnum == YmodemSizeTotal)
						{
							/* 写入成功 */
							YmodemSizeTotal = 0;
							/* 接收完毕,等待EOT */
							ShellUart.RxFlag = YmodemNext;
						}
					}
				}
				break;
		}
	}else{
		if(*YmodemData)
		{
			/* 起始帧 */
			memcpy(YmodemName, YmodemData, strlen(YmodemData) + 1);
			if(f_open(&YmodemFile, YmodemName, FA_CREATE_ALWAYS | FA_WRITE) == FR_OK)
			{
				char* YmodemStr = YmodemData;
				while(*YmodemStr)
				{
					YmodemStr++;
				}
				YmodemStr++;
				YmodemSizeTotal = 0;
				while(*YmodemStr != ' ')
				{
					YmodemSizeTotal *= 10;
					YmodemSizeTotal += *YmodemStr - '0';
					YmodemStr++;
				}
				ShellUart.RxFlag = YmodemNext;
			}
		}else{
			/* 结束帧 */
			ShellUart.RxFlag = YmodemQuit;
		}
	}
	
	if(ShellUart.RxFlag == YmodemGet)
	{
		ShellUart.RxFlag = YmodemRepeat;
	}
}

static uint8_t YmodemTime;
static _Bool GetDataFrame(void)
{
	/* 等待帧 */
	YmodemTime = YMODEM_WAIT_TIME;
	while(YmodemTime--)
	{
		switch(ShellUart.RxFlag)
		{
			case YmodemGet:
				/* 获取到了帧,并检查 */
				switch(ShellUart.RxData[0])
				{
					case SOH:
						/* 配对包号 */
						if(ShellUart.RxData[1] == (uint8_t)YmodemPackNum)
						{
							/* CRC检验 */
							uint16_t CRC16 = (ShellUart.RxData[131] << 8) | ShellUart.RxData[132];
							if(CRC16 == Cal_CRC16(&ShellUart.RxData[3], 128))
							{
								YmodemData = (char*)&ShellUart.RxData[3];
								return 1;
							}
						}
						break;
						
					#if (ShellConsoleMaxSize > 1029)
					case STX:
						/* 配对包号 */
						if(ShellUart.RxData[1] == (uint8_t)YmodemPackNum)
						{
							/* CRC检验 */
							uint16_t CRC16 = (ShellUart.RxData[1027] << 8) | ShellUart.RxData[1028];
							if(CRC16 == Cal_CRC16(&ShellUart.RxData[3], 1024))
							{
								YmodemData = (char*)&ShellUart.RxData[3];
								return 1;
							}
						}
						break;
					#else
					case STX:
						/* 接收长度不支持STX格式 */
						break;
					#endif
						
					default:
						/* 帧头有问题，重新接收 */
						break;
				}
				/* 帧有问题重新发 */
				ShellUart.RxFlag = YmodemRepeat;
				continue;
			
			case YmodemOk:
				/* 当前文件传输完成 */
				f_close(&YmodemFile);
				ShellUart.RxFlag = YmodemWait;
				YmodemPackNum = 0;
				ShellUart.RxLen = 0;
				ShellUart.StartRx(ShellUart.UartHandle, ShellUart.RxData, ShellConsoleMaxSize);
				Print("\r\nYmodem [%s] Transfer Success", YmodemName);
				Print("%c", C);
				/* 133字节在115200波特率大约需要12ms传输完毕 */
				ShellDelay(20);
				break;
			
			case YmodemRepeat:
				/* 再发送一次当前帧 */
				ShellUart.RxFlag = YmodemWait;
				ShellUart.RxLen = 0;
				ShellUart.StartRx(ShellUart.UartHandle, ShellUart.RxData, ShellConsoleMaxSize);
				Print("%c", NAK);
				break;
			
			case YmodemQuit:
				/* Ymodem所有文件传输完毕 */
				return 0;
			
			case YmodemEOH:
				/* 重新确认EOT */
				ShellUart.RxLen = 0;
				ShellUart.StartRx(ShellUart.UartHandle, ShellUart.RxData, ShellConsoleMaxSize);
				Print("%c", NAK);
				ShellDelay(1);
				break;
			
			case YmodemWait:
				/* 等一下 */
				ShellDelay(1);//因为是用DMA所以需要等待
				if((YmodemTime % 25) == 0)
				{
					/* 可能没有接到在发送一次 */
					ShellUart.RxFlag = YmodemRepeat;
				}
				break;
			
			default:
				/* 不应该有其他指令 */
				ShellUart.RxFlag = YmodemWait;
				continue;
		}
	}
	/* 传输超时 */
	ShellUart.RxFlag = YmodemOut;
	return 0;
}

static void Ymodem(void)
{
	/* 进入Ymodem状态 */
	Print("\r\nYmodem Start");
	YmodemSizeTotal = 0;
	YmodemPackNum = 0;
	ShellUart.RxFlag = YmodemWait;
	ShellUart.RxLen = 0;
	ShellUart.StartRx(ShellUart.UartHandle, ShellUart.RxData, ShellConsoleMaxSize);
	Print("%c", C);
	while(1)
	{
		if(GetDataFrame())
		{
			/* 处理获取到的帧 */
			DealYmodemDataFrame(ShellUart.RxData[0]);
			switch(ShellUart.RxFlag)
			{
				case YmodemOk:
				case YmodemRepeat:
				case YmodemQuit:
					break;
				
				case YmodemNext:
					/* 获取下一帧 */
					YmodemPackNum++;
					ShellUart.RxFlag = YmodemWait;
					ShellUart.RxLen = 0;
					ShellUart.StartRx(ShellUart.UartHandle, ShellUart.RxData, ShellConsoleMaxSize);
					Print("%c", ACK);
					break;
					
				default:
					/* 不应该有其他状态 */
					Print("\r\nError Flag[%d]", ShellUart.RxFlag);
					break;
			}
		}else{
			/* 没有获取到帧退出 */
			Print("%c%c%c%c%c%c", CA, CA, CA, CA, CA, CA);
			switch(ShellUart.RxFlag)
			{
				case YmodemQuit:
					Print("\r\nYmodem Transfer Finish");
					break;
				
				case YmodemOut:
					Print("\r\nYmodem Time Out");
					break;
				
				default:
					Print("\r\nYmodem Error [%d]", ShellUart.RxFlag);
					break;
			}
			/* 退出Ymodem状态 */
			ShellUart.RxFlag = WaitCMD;
			Print("\r\nYmodem End");
			return;
		}
	}
}
#endif

/* 初始化命令台 */
HAL_StatusTypeDef ShellConsoleInit(UART_HandleTypeDef* huart)
{
	ListHand.CmdNum = 1;
	ListHand.hand = &HelpNode;
	ListHand.end = &HelpNode;
	
	#if USE_YMODEM
	ShellAddCmd(YmodemCmd, "Ymodem", Ymodem, 0);
	ShellSetCmdShow(YmodemCmd, 0);
	#endif
	
	ShellUart.UartHandle = huart;
	ShellUart.RxLen = 0;
	ShellUart.RxFlag = WaitCMD;
	memset(ShellUart.RxData, 0, ShellConsoleMaxSize);
	
	if(ShellUart.UartHandle->hdmarx)
	{
		ShellUart.StartRx = HAL_UARTEx_ReceiveToIdle_DMA;
	}else{
		ShellUart.StartRx = HAL_UARTEx_ReceiveToIdle_IT;
	}
	Print("\r\nShell Start");
	Print("\r\nCMD>");
	
	return	ShellUart.StartRx(ShellUart.UartHandle, ShellUart.RxData, ShellConsoleMaxSize);
}

/* 检查是否输入了指令 */
inline static void CheckCmd(uint16_t Size)
{
	#if USE_YMODEM
	/* 是否在Ymodem程序中 */
	if((ShellUart.RxFlag >= YmodemWait) & (ShellUart.RxFlag <= YmodemOut))
	{
		/* 等待帧 */
		if(ShellUart.RxFlag == YmodemWait)
		{
			ShellUart.RxLen += Size;
			switch(ShellUart.RxData[0])
			{
				case SOH:
					if(ShellUart.RxLen == 133)
					{
						/* 得到了帧 */
						ShellUart.RxFlag = YmodemGet;
					}
					/* 帧不完全继续接收 */
					if(ShellUart.RxLen < 133)
					{
						ShellUart.StartRx(ShellUart.UartHandle, &ShellUart.RxData[ShellUart.RxLen], (ShellConsoleMaxSize - ShellUart.RxLen));
					}
					/* 帧长度超出了，重新接收 */
					if(ShellUart.RxLen > 133)
					{
						ShellUart.RxFlag = YmodemRepeat;
					}
					break;
				
				#if (ShellConsoleMaxSize > 1029)
				case STX:
					if(ShellUart.RxLen == 1029)
					{
						/* 得到了帧 */
						ShellUart.RxFlag = YmodemGet;
					}
					/* 帧不完全继续接收 */
					if(ShellUart.RxLen < 1029)
					{
						ShellUart.StartRx(ShellUart.UartHandle, &ShellUart.RxData[ShellUart.RxLen], (ShellConsoleMaxSize - ShellUart.RxLen));
					}
					/* 帧长度超出了，重新接收 */
					if(ShellUart.RxLen > 1029)
					{
						ShellUart.RxFlag = YmodemRepeat;
					}
					break;
				#else
				case STX:
					/* 接收长度不支持STX格式 */
					ShellUart.RxFlag = YmodemRepeat;
					break;
				#endif
					
				default:
					/* 帧头有问题，重新接收 */
					ShellUart.RxFlag = YmodemRepeat;
					break;
			}
		}
		return;
	}
	#endif
	
	switch(ShellUart.RxData[ShellUart.RxLen + Size - 1])
	{
		case 0x0D:
			ShellUart.RxFlag = GetCMD;
			ShellUart.RxLen += Size - 1;
			break;
		
		default:
			Print("%.*s", Size, &ShellUart.RxData[ShellUart.RxLen]);
			ShellUart.RxLen += Size;
			ShellUart.StartRx(ShellUart.UartHandle, &ShellUart.RxData[ShellUart.RxLen], (ShellConsoleMaxSize - ShellUart.RxLen));
			break;
	}
}

/* 检查是否可以补全 */
inline static int ReplenishCmd(void)
{
	if(ShellUart.RxLen == 0)
	{
		//如果没有残缺指令
		return HAL_ERROR;
	}
	struct CmdListNode *bufNode = ListHand.hand;
	for(uint16_t i = ListHand.CmdNum; i > 0; i--)
	{
		if(bufNode->ShowFlag & (ShellUart.RxLen <= strlen(bufNode->Cmd)) & (strncmp((char*)ShellUart.RxData, bufNode->Cmd, ShellUart.RxLen) == 0))
		{
			Print("%s", &bufNode->Cmd[ShellUart.RxLen]);
			ShellUart.RxLen = strlen(bufNode->Cmd);
			memset(ShellUart.RxData, 0, ShellUart.RxLen + 1);
			memcpy(ShellUart.RxData, bufNode->Cmd, ShellUart.RxLen + 1);
			return HAL_OK;
		}else{
			bufNode = bufNode->next;
		}
	}
	//没有匹配的指令
	return HAL_ERROR;
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if(huart->Instance == ShellUart.UartHandle->Instance)
	{
		if (huart->ReceptionType == HAL_UART_RECEPTION_STANDARD)
    {
			switch(Size)
			{
				case 1:
					switch(ShellUart.RxData[ShellUart.RxLen + Size - 1])
					{
						/* 删除键 */
						case 0x08:
							if(ShellUart.RxLen)
							{
								ShellUart.RxLen--;
								Print("%c%c%c", 0x08, 0x20, 0x08);
							}
							ShellUart.StartRx(ShellUart.UartHandle, &ShellUart.RxData[ShellUart.RxLen], (ShellConsoleMaxSize - ShellUart.RxLen));
							break;
						/* TAB键 */
						case 0x09:
							if(ReplenishCmd())
							{
								HelpNode.Function();
								ClearShellUartRx();
							}else{
								ShellUart.StartRx(ShellUart.UartHandle, &ShellUart.RxData[ShellUart.RxLen], (ShellConsoleMaxSize - ShellUart.RxLen));
							}
							break;
						/* 回车键 */
						case 0x0D:
							/* 表示接收到了指令，ShellUart.RxLen指向回车 */
							ShellUart.RxFlag = GetCMD;
							break;
						
						#if USE_YMODEM
						/* 传送完毕终止 */
						case EOT:
							if((ShellUart.RxFlag >= YmodemWait) & (ShellUart.RxFlag <= YmodemOut))
							{
								if(ShellUart.RxFlag == YmodemEOH)
								{
									ShellUart.RxFlag = YmodemOk;
								}else{
									ShellUart.RxFlag = YmodemEOH;
								}
							}else{
								Print("%c", ShellUart.RxData[ShellUart.RxLen]);
								ShellUart.RxLen++;
								ShellUart.StartRx(ShellUart.UartHandle, &ShellUart.RxData[ShellUart.RxLen], (ShellConsoleMaxSize - ShellUart.RxLen));
							}
							break;
						#endif
						
						/* 其他键 */
						default:
							Print("%c", ShellUart.RxData[ShellUart.RxLen]);
							ShellUart.RxLen++;
							ShellUart.StartRx(ShellUart.UartHandle, &ShellUart.RxData[ShellUart.RxLen], (ShellConsoleMaxSize - ShellUart.RxLen));
							break;
					}
					break;
					
				#if MEMORY_CMD
					case 3:
						if(RmbCMDHand.CmdNum)
						{
							if(strncmp((char*)&ShellUart.RxData[ShellUart.RxLen], UpKey, 3) == 0)
							{
								if(RmbCMDHand.end == NULL)
								{
									RmbCMDHand.end = RmbCMDHand.hand;
								}else{
									RmbCMDHand.end = RmbCMDHand.end->per;
								}
								for(uint16_t i = 0; i < ShellUart.RxLen; i++)
								{
									Print("%c%c%c", 0x08, 0x20, 0x08);
								}
								
								ShellUart.RxLen = strlen(RmbCMDHand.end->Cmd);
								memcpy(ShellUart.RxData, RmbCMDHand.end->Cmd, ShellUart.RxLen);
								Print("%.*s", ShellUart.RxLen, ShellUart.RxData);
								ShellUart.StartRx(ShellUart.UartHandle, &ShellUart.RxData[ShellUart.RxLen], (ShellConsoleMaxSize - ShellUart.RxLen));
							}else if(strncmp((char*)&ShellUart.RxData[ShellUart.RxLen], DownKey, 3) == 0){
								if(RmbCMDHand.end == NULL)
								{
									RmbCMDHand.end = RmbCMDHand.hand;
								}else{
									RmbCMDHand.end = RmbCMDHand.end->next;
								}
								for(uint16_t i = 0; i < ShellUart.RxLen; i++)
								{
									Print("%c%c%c", 0x08, 0x20, 0x08);
								}
								ShellUart.RxLen = strlen(RmbCMDHand.end->Cmd);
								memcpy(ShellUart.RxData, RmbCMDHand.end->Cmd, ShellUart.RxLen);
								Print("%.*s", ShellUart.RxLen, ShellUart.RxData);
								ShellUart.StartRx(ShellUart.UartHandle, &ShellUart.RxData[ShellUart.RxLen], (ShellConsoleMaxSize - ShellUart.RxLen));
							}else{
								CheckCmd(Size);
							}
						}else{
							ShellUart.StartRx(ShellUart.UartHandle, &ShellUart.RxData[ShellUart.RxLen], (ShellConsoleMaxSize - ShellUart.RxLen));
						}
						break;
				#endif

				default:
					CheckCmd(Size);
					break;
			}
    }
		
    if (huart->ReceptionType == HAL_UART_RECEPTION_TOIDLE)
    {
			if((ShellUart.RxLen + Size) > ShellConsoleMaxSize)
			{
				Print("\r\nError Shell Overflow MaxSize [%d]", ShellConsoleMaxSize);
				ClearShellUartRx();
			}
    }
	}
}

/* 清空指令行 */
inline static void ClearShellUartRx(void)
{
	memset(ShellUart.RxData, 0, ShellConsoleMaxSize);
	ShellUart.RxLen = 0;
	ShellUart.RxFlag = WaitCMD;
	ShellUart.StartRx(ShellUart.UartHandle, ShellUart.RxData, ShellConsoleMaxSize);
	Print("\r\nCMD>");
}

/* 热更新接收串口 */
void UpShellUartHandle(UART_HandleTypeDef *huart)
{
	HAL_UART_AbortReceive_IT(ShellUart.UartHandle);
	ShellConsoleInit(huart);
}

