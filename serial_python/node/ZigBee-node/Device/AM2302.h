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


#ifndef __AM2302_H  
#define __AM2302_H  

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
#define AM2302_IO_IN()  {P0DIR&=~0x40;} //P0_6 set input default
#define AM2302_IO_OUT() {P0DIR |= 0x40;} //P0_6 set output
////IO操作
#define	AM2302_IO  P0_6 //

/********************************  Hard Ware  connection  ************************/
typedef struct  
{  
    unsigned char  humi_int;      //êa?èμ???êy2?·?  
    unsigned char  humi_deci;     //êa?èμ?D?êy2?·?  
    unsigned char  temp_int;      //???èμ???êy2?·?  
    unsigned char  temp_deci;     //???èμ?D?êy2?·?  
    unsigned char  check_sum;     //D￡?éoí  
                                 
}AM2302_Data;  


/*******函数声明*********/
void delay_us(u16 timeout);
void delay_10us(void); //10 us延时
void delay_ms(u16 Time);//n ms延时

u8 AM2302_Init(void);//初始化DHT11
u8 AM2302_Read_Data(AM2302_Data * refAM2302)     ;//读取温湿度
u8 AM2302_Read_Byte(void);//读出一个字节
u8 AM2302_Read_Bit(void);//读出一个位

u8 AM2302_Check(void);//检测是否存在DHT11
void AM2302_Rst(void);//复位DHT11    


#endif















