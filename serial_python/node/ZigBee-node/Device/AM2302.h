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
//IO��������
#define AM2302_IO_IN()  {P0DIR&=~0x40;} //P0_6 set input default
#define AM2302_IO_OUT() {P0DIR |= 0x40;} //P0_6 set output
////IO����
#define	AM2302_IO  P0_6 //

/********************************  Hard Ware  connection  ************************/
typedef struct  
{  
    unsigned char  humi_int;      //��a?����???��y2?��?  
    unsigned char  humi_deci;     //��a?����?D?��y2?��?  
    unsigned char  temp_int;      //???����???��y2?��?  
    unsigned char  temp_deci;     //???����?D?��y2?��?  
    unsigned char  check_sum;     //D��?��o��  
                                 
}AM2302_Data;  


/*******��������*********/
void delay_us(u16 timeout);
void delay_10us(void); //10 us��ʱ
void delay_ms(u16 Time);//n ms��ʱ

u8 AM2302_Init(void);//��ʼ��DHT11
u8 AM2302_Read_Data(AM2302_Data * refAM2302)     ;//��ȡ��ʪ��
u8 AM2302_Read_Byte(void);//����һ���ֽ�
u8 AM2302_Read_Bit(void);//����һ��λ

u8 AM2302_Check(void);//����Ƿ����DHT11
void AM2302_Rst(void);//��λDHT11    


#endif















