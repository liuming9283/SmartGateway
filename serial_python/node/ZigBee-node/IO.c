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
// Date: 2016.10.28
// Modify: 
// Author: Carey
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Header File Preprocessor Directive
//-----------------------------------------------------------------------------

#include "IO.H"

#include "OnBoard.h"
#include  "MT_UART.h" //use for com

//��ʼ��AM2302��IO�� DQ ͬʱ���AM2302�Ĵ���
//����1:������
//����0:����    	 
u8 IO_Init(void)
{	 

	P1SEL &= ~0x01;P1DIR |= 0x01;P1INP &= ~0x01; //������
        P1SEL &= ~0x08;P1DIR |= 0x08;P1INP &= ~0x08; //������
       // P1SEL &= ~0x10;P1DIR |= 0x10;P1INP &= ~0x10; //������
        P1SEL &= ~0x20;P1DIR |= 0x20;P1INP &= ~0x20; //������
        P1SEL &= ~0x40;P1DIR |= 0x40;P1INP &= ~0x40; //������
        Relay_IO  =IO_OFF;        
        LED1_IO   =IO_OFF;        
       // LED2_IO  =IO_OFF;          
        LED3_IO   =IO_OFF;         
        Find_IO   =IO_OFF;   
} 
