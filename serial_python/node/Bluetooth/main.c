//---------------------------------------------------------------
//
//	 ( ( 
//	  ) )     
//	(----)-)    Carey Creat
//	 \__/-'     T2016
//	`----'    ==============
//----------------------------------------------------------------
//
// This program is the ___ body Sense Control Platform project.
// Main Code 
// 
// Target: STM32F103RCT6
// Tool chain: KEIL 5.15
// Date: 2016.08.26
// Modify: 
// Author: Carey
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Header File Preprocessor Directive
//-----------------------------------------------------------------------------
#include "stm32f10x.h"
#include "SysTick.h"
#include "led.h"
#include "usart1.h"
#include "delay.h"

#include "Time2.h"
#include "IO.h"
#include "TCP-232-V1.h"
#include "24cxx.h" 



#include "hc05.h"
	 	 
#include "string.h"	 
#include "key.h"

#define Run  			0
#define Error1  	1
#define Error2  	2
#define Error3  	3
#define Error4  	4
#define Error5  	5
#define Error6  	6
#define Error7  	7
#define Busy  		8


/* 定义例程名和例程发布日期 */
#define EXAMPLE_NAME	"BLUETECH 工程"
#define EXAMPLE_DATE	"2017-01-11"
#define DEMO_VER			"1.0"

unsigned char CRCValueBuf[2];

unsigned char  Mes_Hard[2] =   "S:";
unsigned char  Mes_End[3] =   "E\r\n";

/* 仅允许本文件内调用的函数声明 */
static void PrintfLogo(void);
static void DispMenu(void);

unsigned char RunStatus;
volatile u32 time; // ms 计数用变量
unsigned int Second=0;
unsigned int Minuet=0;


char *_NodeNum="801005";
 char *_MAC="5C-C5-D4-4C-B1-6B";
 char *_Location="25";
 char *_Parent="10.10.25.89";
 char *Type="Bluetooth";
 
unsigned char  getBuf[50];
char * _getBuf;
unsigned char setHeart =5;
unsigned char setPeriod =10;

unsigned char  NodeNum[20];
unsigned char  MAC[30];
unsigned char  Location[20];
unsigned char  Parent[20];

//要写入到24c32的字符串数组
const u8 TEXT_Buffer[]={"WarShipSTM32 IIC TEST"};
#define SIZE sizeof(TEXT_Buffer)	
	

	
	//显示ATK-HC05模块的主从状态
void HC05_Role_Show(void)
{
	if(HC05_Get_Role()==1)printf("ROLE:Master");	//主机
	else printf("ROLE:Slave ");			 		//从机
}
//显示ATK-HC05模块的连接状态 
void HC05_Sta_Show(void)
{												 
	if(HC05_LED)printf("STA:Connected ");			//连接成功
	else printf("STA:Disconnect");	 			//未连接				 
}	 



/*
 * 函数名：main
 * 描述  ：主函数
 * 输入  ：无
 * 输出  ：无
 */
int main(void)
{
		unsigned char _Second=0;
		unsigned char i=0, j=0,k=0,m=0;
	unsigned char xxx=0;
	u8 t;
	u8 key;
	u8 sendmask=0;
	u8 sendcnt=0;
	u8 sendbuf[150];	  
	u8 reclen=0;  
	
	 unsigned char  temp[5];
   unsigned char humi[5];
		/* config the sysclock to 72mhz */
		SystemInit();
		/* config IO gpio */
		LED_GPIO_Config();
		IO_GPIO_Config();
		/* 配置SysTick 为10us中断一次 */
		//  SysTick_Init();
		// AFIO->MAPR = (0x00FFFFFF & AFIO->MAPR)|0x04000000;          //??JTAG 
		USART1_Config();
 		USART2_Config();
//		USART3_Config();
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //
	
	
		/* 串口打印测试*/
		USART_printf(Debug_RT,"CareyCreat Debug_RT test\r\n");
	
		PrintfLogo();   //打印Logo
	//	DispMenu();
		/* TIM2 定时设置*/
	TIM2_NVIC_Configuration();
	TIM2_Configuration();

		/* TIM2 开始计时 */
	START_TIME2;
	//IO_GPIO_Config();
	//IWDG_Init(4,625);    			//与分频数为64,重载值为625,溢出时间为1s	
	statusMach=1;
	LED_GPIO_Config();
	
	printf("ALIENTEK STM32 ^_^");	
	printf("HC05 BLUETOOTH COM TEST");	
	printf("ATOM@ALIENTEK");
	while(HC05_Init()) 		//初始化ATK-HC05模块  
	{
		printf("ATK-HC05 Error!"); 
		delay_ms(500);
		printf("Please Check!!!"); 
		delay_ms(100);
	}	 										   	   
	printf("WK_UP:ROLE KEY0:SEND/STOP");  
	printf("ATK-HC05 Standby!");  
  printf("Send:");	
	printf("Receive:");	

	  sprintf((char *)NodeNum,"%s",_NodeNum);
     sprintf((char *)MAC,"%s",_MAC);
     sprintf((char *)Location,"%s",_Location);;
     sprintf((char *)Parent,"%s",_Parent);

	HC05_Role_Show();	  
	HC05_Set_Cmd("AT+ROLE=1");
	HC05_Role_Show();
	HC05_Set_Cmd("AT+RESET");	//复位ATK-HC05模块
	while(1)
	{
			/* 串口打印测试*/
		//USART_printf(Debug_RT,"CareyCreat Debug_RT test\r\n");
		delay_ms(100);
		sendmask=1;
	
		{
		//	sendmask=!sendmask;				//发送/停止发送  	 
		//	if(sendmask==0)LCD_Fill(30+40,160,240,160+16,WHITE);//清除显示
		}  
		if(t==50)
		{
			if(sendmask)					//定时发送
			{
				 temp[0] = '2';
  temp[1] = '0';
  temp[2] = 0x2E;
  temp[3] = '3';
  temp[4] = 0;
 
  humi[0]= '4';
  humi[1]= '7';
  humi[2]= 0x2E;
  humi[3]= '9';
  humi[4]= 0;
			//	sprintf((char*)sendbuf,"ALIENTEK HC05 %d\r\n",sendcnt);
					sprintf((char *)sendbuf,"S:N=%s@Type=%s@MAC=%s@Location=%s@Parent=%s@Temp=%s&Humi=%sE;",NodeNum,Type,MAC,Location,Parent,temp,humi);
	  	
	  		printf("%s",sendbuf);	//显示发送数据	
				//	USART_printf(USART2,"ALIENTEK HC05 %d\r\n",sendcnt);		//发送到蓝牙模块
				USART_printf(USART2,"%s\r\n",sendbuf);
				sendcnt++;
				if(sendcnt>99)sendcnt=0;
			}
			HC05_Sta_Show();  	  
			t=0;
		 //	LED0=!LED0; 	     
		}	  
		if(USART2_RX_STA&0X8000)			//接收到一次数据了
		{
			
 			reclen=USART2_RX_STA&0X7FFF;	//得到数据长度
		  	USART2_RX_BUF[reclen]=0;	 	//加入结束符
			if(reclen==9||reclen==8) 		//控制DS1检测
			{
				if(strcmp((const char*)USART2_RX_BUF,"+LED1 ON")==0) LED0(0);	//打开LED1
				if(strcmp((const char*)USART2_RX_BUF,"+LED1 OFF")==0)  LED0(1);//关闭LED1
			}
 		printf("%s",USART2_RX_BUF);//显示接收到的数据
 			USART2_RX_STA=0;	 
		}	 															     				   
		t++;	
	}
//////////////////////////////////////////////////////////////////////////


}
void RunState(unsigned char status)
{
	unsigned char i=0;
	switch(status)
	{
		case Error1:
			 //////SYSTEM RUN//////	
			for(i=0;i<1;i++)
			{
				delay_ms(200);			
				LED0(0);
				delay_ms(200);	
				LED0(1);
			}
			delay_ms(500);
			break;
			case Error2:
			 //////SYSTEM RUN//////	
			for(i=0;i<2;i++)
			{
				delay_ms(200);			
				LED0(0);
				delay_ms(200);	
				LED0(1);
			}
			delay_ms(500);
			break;
			case Error3:
			 //////SYSTEM RUN//////	
			for(i=0;i<3;i++)
			{
				delay_ms(200);			
				LED0(0);
				delay_ms(200);	
				LED0(1);
			}
			delay_ms(500);
			break;
			case Error4:
			 //////SYSTEM RUN//////	
			for(i=0;i<4;i++)
			{
				delay_ms(200);			
				LED0(0);
				delay_ms(200);	
				LED0(1);
			}
			delay_ms(500);
			break;
			case Error5:
			 //////SYSTEM RUN//////	
			for(i=0;i<5;i++)
			{
				delay_ms(200);			
				LED0(0);
				delay_ms(200);	
				LED0(1);
			}
			delay_ms(500);
			break;
			break;
		case Run:
			 //////SYSTEM RUN//////
			delay_ms(500);			
			LED0(1);
			delay_ms(500);	
			LED0(0);
			break;
		case Busy:
			 //////SYSTEM RUN//////
			delay_ms(50);			
			LED0(1);
			delay_ms(50);	
			LED0(0);
			break;
		default:break;
	}
}

/*
*********************************************************************************************************
*	函 数 名: DispMenu
*	功能说明: 显示操作提示菜单
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void DispMenu(void)
{
	USART_printf(Debug_RT,"\r\n 通讯设置 r\n");
	USART_printf(Debug_RT,"\r\n 1、AT+SetIP r\n");
	USART_printf(Debug_RT,"\r\n 2、AT+SetPort r\n");
	USART_printf(Debug_RT,"\r\n 3、AT+SetHeartTime r\n");
}

/*
*********************************************************************************************************
*	函 数 名: PrintfLogo
*	功能说明: 打印例程名称和例程发布日期, 接上串口线后，打开PC机的超级终端软件可以观察结果
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void PrintfLogo(void)
{
	USART_printf(Debug_RT,"\n\r");
	USART_printf(Debug_RT,"*************************************************************\n\r");
	USART_printf(Debug_RT,"* 工程名称   ：%s\r\n", EXAMPLE_NAME);	/* 打印例程名称 */
	USART_printf(Debug_RT,"* 工程版本   : %s\r\n", DEMO_VER);		/* 打印例程版本 */
	USART_printf(Debug_RT,"* 发布日期   : %s\r\n", EXAMPLE_DATE);	/* 打印例程日期 */
	USART_printf(Debug_RT,"* \n\r");	/* 打印一行空格 */
	USART_printf(Debug_RT,"* CareyCreat \r\n");
	USART_printf(Debug_RT,"*************************************************************\n\r");
}

////////从eeprom中读取参数配置
///
unsigned char  read_Config(unsigned char Items)
{
//	AT24CXX_Read(0,&Mes_EEs.IPs,5);
	//Mes_EES.IPs[0]=1;
}
	
unsigned char  write_Config(unsigned char Items)
{
	
}
	
/*****************************  CareyCreat@foxmail.com (END OF FILE) *********************************/




