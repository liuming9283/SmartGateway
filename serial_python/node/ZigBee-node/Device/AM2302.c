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

#include "AM2302.H"

#include "OnBoard.h"
#include  "MT_UART.h" //use for com

/****************************
        延时函数
*****************************/
void delay_us(u16 timeout)
{
   MicroWait(timeout);
}

void delay_10us() //10 us延时
{
  MicroWait(10);
}

void delay_ms(u16 Time)//n ms延时
{
  unsigned char i;
  while(Time--)
  {
    for(i=0;i<100;i++)
     delay_10us();
  }
}

//复位AM2302
void AM2302_Rst(void)	   
{                 
	AM2302_IO_OUT();
	AM2302_IO = 0;
	delay_ms(20);    	//拉低至少18ms
	AM2302_IO = 1;
	delay_us(30);     	//主机拉高20~40us
}
//等待AM2302的回应
//返回1:未检测到AM2302的存在
//返回0:存在
u8 AM2302_Check(void) 	   
{   
	u8 retry=0;
	//AM2302_IO_IN();//SET INPUT	 
	AM2302_IO_IN();

	//while (AM2302_DQ_IN&&retry<100)//AM2302会拉低40~80us
	while (AM2302_IO&&retry<100)//AM2302会拉低40~80us
	{
		retry++;
		delay_us(1);
	}
	if(retry>=100){
		HalUARTWrite(0,"L error\n",8);
		return 1;
	}
	else{
		retry=0;
	}
	
	//while (!AM2302_DQ_IN&&retry<100)//AM2302拉低后会再次拉高40~80us
	while (!AM2302_IO&&retry<100)//AM2302拉低后会再次拉高40~80us
	{
		retry++;
		delay_us(1);
	}
	if(retry>=100){
		HalUARTWrite(0,"H error\n",8);
		return 1;
	}

	return 0;
}

//从AM2302读取一个位
//返回值：1/0
u8 AM2302_Read_Bit(void) 			 
{
 	u8 retry=0;
	while(AM2302_IO&&retry<100)//等待变为低电平
	{
		retry++;
		delay_us(1);
	}
	retry=0;
	while(!AM2302_IO&&retry<100)//等待变高电平
	{
		retry++;
		delay_us(1);
	}
	delay_us(40);//等待40us
	if(AM2302_IO){
		return 1;
	}
	else{
		return 0;
	}
}
//从AM2302读取一个字节
//返回值：读到的数据
u8 AM2302_Read_Byte(void)    
{        
    u8 i,dat;
    dat=0;
	for (i=0;i<8;i++) 
	{
            dat<<=1; 
	    dat|=AM2302_Read_Bit();
    }						    
    return dat;
}
//从AM2302读取一次数据
//temp:温度值(范围:0~50°)
//humi:湿度值(范围:20%~90%)
//返回值：0,正常;1,读取失败
u8 AM2302_Read_Data(AM2302_Data * refAM2302)    
{        
 	u8 buf[5];
	u8 i;
	AM2302_Rst();
	if(AM2302_Check()==0)
	{
		for(i=0;i<5;i++)//??40???
		{
			buf[i]=AM2302_Read_Byte();
		}
                
                  refAM2302->humi_deci=buf[0];
                  refAM2302->humi_int=buf[1];
                  refAM2302->temp_deci=buf[2];
                  refAM2302->temp_int=buf[3];
                  refAM2302->check_sum=buf[4];
		if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])
		{
			/*refAM2302. =buf[0];
			*humiL=buf[1];
			*tempH=buf[2];
			*tempL=buf[3];*/
                  refAM2302->humi_deci=buf[0];
                  refAM2302->humi_int=buf[1];
                  refAM2302->temp_deci=buf[2];
                  refAM2302->temp_int=buf[3];
		}
	}else return 1;
	return 0;	    
}
//初始化AM2302的IO口 DQ 同时检测AM2302的存在
//返回1:不存在
//返回0:存在    	 
u8 AM2302_Init(void)
{	 
	P0SEL&=~0x40; //作为普通IO口
	//P0DIR&=~0x02; //P0_6 set input default
	P0DIR |= 0x40; //P0_6 set output
	P0INP &= ~0x40; //打开上拉

	return AM2302_Check();//等待AM2302的回应
} 
