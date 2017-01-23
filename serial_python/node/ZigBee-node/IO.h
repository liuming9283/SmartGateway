//---------------------------------------------------------------
//
//	 ( ( 
//	  ) )     
//	(----)-)    Carey Creat
//	 \__/-'     T2016 
//	`----'    ==============
//----------------------------------------------------------------
//
// This program is the ___ header Sense Control Platform project.
// Tool chain: iar 8.2.2
// Date: 2016.10.28
// Author: Carey
//-----------------------------------------------------------------------------


#ifndef __IO_H  
#define __IO_H  

#include <ioCC2530.h>

typedef unsigned char   u8;
typedef unsigned short  u16;

/********************************  Hard Ware  connection  ************************
 *           -----------------
 *          |   PA0 - L1      |
 *          |   PA7 - L8      |
 *           ----------------- 																						 
**********************************************************************************/
//IO方向设置
////IO操作

#define	Relay_IO        P1_0  //
#define Relay_IO_OUT() {P1SEL &= ~0x01;P1DIR |= 0x01;} //P1_0 set output
#define	LED1_IO         P1_3  //
#define LED1_IO_OUT()  {P1SEL &= ~0x08;P1DIR |= 0x08;} //P0_6 set output
#define	LED2_IO         P1_4  //
#define LED2_IO_OUT()  {P1SEL &= ~0x10;P1DIR |= 0x10;} //P0_6 set output
#define	LED3_IO         P1_5  //
#define LED3_IO_OUT()  {P1SEL &= ~0x20;P1DIR |= 0x20;} //P0_6 set output
#define	Find_IO         P1_1  //
#define Find_IO_OUT()  {P1SEL &= ~0x02;P1DIR |= 0x02;} //P0_6 set output

#define	IO_ON    1
#define	IO_OFF   0
   

/********************************  Hard Ware  connection  ************************/

u8 IO_Init(void);//初始化

#endif















