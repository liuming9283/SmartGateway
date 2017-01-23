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

#ifndef __DEFINE_H  
#define __DEFINE_H  

#include <ioCC2530.h>
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define isCoord   0
#if(isCoord)
#define AM2302_En 0
#else
#define AM2302_En 1
#endif

///S:N=802001@mac=这里是实际的设备mac@location=25@parent=10.10.25.89@Temp=31.2&Humi=31.2E;
/* char *NodeNum="801002";
 char *MAC="E1CD";
 char *Location="25";
 char *Parent="10.10.25.89";
*/


 char *_NodeNum="801002";
 char *_MAC="1C-60-DE-77-45-8C";
 char *_Location="25";
 char *_Parent="10.10.25.89";
 char *Type="Zigbee";
 
unsigned char  getBuf[50];
char * _getBuf;
unsigned char setHeart =5;
unsigned char setPeriod =10;

unsigned char  NodeNum[20];
unsigned char  MAC[30];
unsigned char  Location[20];
unsigned char  Parent[20];

#define setNodeNum()


/********************************  Hard Ware  connection  ************************/

/********************************  Hard Ware  connection  ************************/


/*******函数声明*********/


#endif