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
// Target: zigbee
// Tool chain: iar 8.2.2
// Date: 2016.11.11
// Modify: 
// Author: Carey
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Header File Preprocessor Directive
//-----------------------------------------------------------------------------

#include "IO_conf.h"

#include "OnBoard.h"
#include  "MT_UART.h" //use for com


//初始化AM2302的IO口 DQ 同时检测AM2302的存在
//返回1:不存在
//返回0:存在    	 
u8 FindIO_Init(void)
{	 
	P1SEL&=~0x40; //作为普通IO口
	//P0DIR&=~0x40; //P0_6 set input default
	P1DIR |= 0x40; //P0_6 set output
	P1INP &= ~0x40; //打开上拉

} 
