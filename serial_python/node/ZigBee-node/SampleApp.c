

/*********************************************************************
 * INCLUDES
 */
#include "OSAL.h"
#include "ZGlobals.h"
#include "AF.h"
#include "aps_groups.h"
#include "ZDApp.h"

#include "SampleApp.h"
#include "SampleAppHw.h"

#include "OnBoard.h"

/* HAL */
#include "hal_lcd.h"
#include "hal_led.h"
#include "hal_key.h"

#include  "MT_UART.h" //此处用于串口
#include  "MT.h"


/* self HAL */
#include "define.h"
#include "AM2302.h"
#include "ZPacketProt.h"
#include "IO.h"


/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

// This list should be filled with Application specific Cluster IDs.
const cId_t SampleApp_ClusterList[SAMPLEAPP_MAX_CLUSTERS] =
{
  SAMPLEAPP_PERIODIC_CLUSTERID,
  SAMPLEAPP_FLASH_CLUSTERID
};

const SimpleDescriptionFormat_t SampleApp_SimpleDesc =
{
  SAMPLEAPP_ENDPOINT,              //  int Endpoint;
  SAMPLEAPP_PROFID,                //  uint16 AppProfId[2];
  SAMPLEAPP_DEVICEID,              //  uint16 AppDeviceId[2];
  SAMPLEAPP_DEVICE_VERSION,        //  int   AppDevVer:4;
  SAMPLEAPP_FLAGS,                 //  int   AppFlags:4;
  SAMPLEAPP_MAX_CLUSTERS,          //  uint8  AppNumInClusters;
  (cId_t *)SampleApp_ClusterList,  //  uint8 *pAppInClusterList;
  SAMPLEAPP_MAX_CLUSTERS,          //  uint8  AppNumInClusters;
  (cId_t *)SampleApp_ClusterList   //  uint8 *pAppInClusterList;
};

// This is the Endpoint/Interface description.  It is defined here, but
// filled-in in SampleApp_Init().  Another way to go would be to fill
// in the structure here and make it a "const" (in code space).  The
// way it's defined in this sample app it is define in RAM.
endPointDesc_t SampleApp_epDesc;

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
uint8 SampleApp_TaskID;   // Task ID for internal task/event processing
                          // This variable will be received when
                          // SampleApp_Init() is called.
devStates_t SampleApp_NwkState;

uint8 SampleApp_TransID;  // This is the unique message ID (counter)

afAddrType_t SampleApp_Periodic_DstAddr;
afAddrType_t SampleApp_Flash_DstAddr;

aps_Group_t SampleApp_Group;

uint8 SampleAppPeriodicCounter = 0;
uint8 SampleAppFlashCounter = 0;


#if defined ( AM2302_En )
  AM2302_Data *redAM2302;
#endif

/*********************************************************************
 * LOCAL FUNCTIONS
 */
void SampleApp_HandleKeys( uint8 shift, uint8 keys );
void SampleApp_MessageMSGCB( afIncomingMSGPacket_t *pckt );
void SampleApp_SendPeriodicMessage( void );
void SampleApp_SendFlashMessage( uint16 flashTime );
void SampleApp_SerialCMD(mtOSALSerialData_t *cmdMsg);


void SampleApp_SendHartMessage( uint8 *sendStr );
void SampleApp_SendBackMessage( uint8 *sendType,uint8 *sendStr );
void SampleApp_SendEVTMessage(  uint8 *sendStr );
char* strcutC(char*  old, char* start, char*  end);


/*********************************************************************
 * NETWORK LAYER CALLBACKS
 */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

void dev_getDate(void);

/*********************************************************************
 * @fn      SampleApp_Init
 *
 * @brief   Initialization function for the Generic App Task.
 *          This is called during initialization and should contain
 *          any application specific initialization (ie. hardware
 *          initialization/setup, table initialization, power up
 *          notificaiton ... ).
 *
 * @param   task_id - the ID assigned by OSAL.  This ID should be
 *                    used to send messages and set timers.
 *
 * @return  none
 */



  
void SampleApp_Init( uint8 task_id )
{
  SampleApp_TaskID = task_id;
  SampleApp_NwkState = DEV_INIT;
  SampleApp_TransID = 0;
  
  MT_UartInit();//串口初始化
  MT_UartRegisterTaskID(task_id);//登记任务号
  HalUARTWrite(0,"Device part\n",12); //（串口0，'字符'，字符个数。）
  
  
  
  // Device hardware initialization can be added here or in main() (Zmain.c).
  // If the hardware is application specific - add it here.
  // If the hardware is other parts of the device add it in main().

 #if defined ( BUILD_ALL_DEVICES )
  // The "Demo" target is setup to have BUILD_ALL_DEVICES and HOLD_AUTO_START
  // We are looking at a jumper (defined in SampleAppHw.c) to be jumpered
  // together - if they are - we will start up a coordinator. Otherwise,
  // the device will start as a router.
  if ( readCoordinatorJumper() )
    zgDeviceLogicalType = ZG_DEVICETYPE_COORDINATOR;
  else
    zgDeviceLogicalType = ZG_DEVICETYPE_ROUTER;
#endif // BUILD_ALL_DEVICES

#if defined ( HOLD_AUTO_START )
  // HOLD_AUTO_START is a compile option that will surpress ZDApp
  //  from starting the device and wait for the application to
  //  start the device.
  ZDOInitDevice(0);
#endif

  // Setup for the periodic message's destination address
  // Broadcast to everyone
  SampleApp_Periodic_DstAddr.addrMode = (afAddrMode_t)AddrBroadcast;
  SampleApp_Periodic_DstAddr.endPoint = SAMPLEAPP_ENDPOINT;
  SampleApp_Periodic_DstAddr.addr.shortAddr = 0xFFFF;

  // Setup for the flash command's destination address - Group 1
  SampleApp_Flash_DstAddr.addrMode = (afAddrMode_t)afAddrGroup;
  SampleApp_Flash_DstAddr.endPoint = SAMPLEAPP_ENDPOINT;
  SampleApp_Flash_DstAddr.addr.shortAddr = SAMPLEAPP_FLASH_GROUP;

  // Fill out the endpoint description.
  SampleApp_epDesc.endPoint = SAMPLEAPP_ENDPOINT;
  SampleApp_epDesc.task_id = &SampleApp_TaskID;
  SampleApp_epDesc.simpleDesc
            = (SimpleDescriptionFormat_t *)&SampleApp_SimpleDesc;
  SampleApp_epDesc.latencyReq = noLatencyReqs;

  // Register the endpoint description with the AF
  afRegister( &SampleApp_epDesc );

  // Register for all key events - This app will handle all key events
  RegisterForKeys( SampleApp_TaskID );

  // By default, all devices start out in Group 1
  SampleApp_Group.ID = 0x0001;
  osal_memcpy( SampleApp_Group.name, "Group 1", 7  );
  aps_AddGroup( SAMPLEAPP_ENDPOINT, &SampleApp_Group );

#if defined ( LCD_SUPPORTED )
  HalLcdWriteString( "SampleApp", HAL_LCD_LINE_1 );
#endif
  
  

  
  IO_Init();
  
     sprintf((char *)NodeNum,"%s",_NodeNum);
     sprintf((char *)MAC,"%s",_MAC);
     sprintf((char *)Location,"%s",_Location);;
     sprintf((char *)Parent,"%s",_Parent);
 

}

/*********************************************************************
 * @fn      SampleApp_ProcessEvent
 *
 * @brief   Generic Application Task event processor.  This function
 *          is called to process all events for the task.  Events
 *          include timers, messages and any other user defined events.
 *
 * @param   task_id  - The OSAL assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 *
 * @return  none
 */
uint16 SampleApp_ProcessEvent( uint8 task_id, uint16 events )
{
  afIncomingMSGPacket_t *MSGpkt;
    static unsigned char cutHeart=0;
  static unsigned char cutPeriod=0;
  (void)task_id;  // Intentionally unreferenced parameter

  if ( events & SYS_EVENT_MSG )
  {
    MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( SampleApp_TaskID );
    while ( MSGpkt )
    {
      switch ( MSGpkt->hdr.event )
      {
        case CMD_SERIAL_MSG:  //串口收到数据后由MT_UART层传递过来的数据，用网蜂方法接收，编译时不定义MT相关内容 
        // SampleApp_SerialCMD((mtOSALSerialData_t *)MSGpkt);
         break;

        // Received when a key is pressed
          case KEY_CHANGE:
          SampleApp_HandleKeys( ((keyChange_t *)MSGpkt)->state, ((keyChange_t *)MSGpkt)->keys );
          break;

        // Received when a messages is received (OTA) for this endpoint
        case AF_INCOMING_MSG_CMD:
          SampleApp_MessageMSGCB( MSGpkt );  //节点接收数据，并执行
          break;

        // Received whenever the device changes state in the network
        case ZDO_STATE_CHANGE:
          SampleApp_NwkState = (devStates_t)(MSGpkt->hdr.status);
          if ( (SampleApp_NwkState == DEV_ZB_COORD)
              || (SampleApp_NwkState == DEV_ROUTER)
              || (SampleApp_NwkState == DEV_END_DEVICE) )
          {
            // Start sending the periodic message in a regular interval.
            osal_start_timerEx( SampleApp_TaskID,
                              SAMPLEAPP_SEND_PERIODIC_MSG_EVT,
                              SAMPLEAPP_SEND_PERIODIC_MSG_TIMEOUT );
          }
          else
          {
            // Device is no longer in the network
          }
          break;

        default:
          break;
      }

      // Release the memory
      osal_msg_deallocate( (uint8 *)MSGpkt );

      // Next - if one is available
      MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( SampleApp_TaskID );
    }

    // return unprocessed events
    return (events ^ SYS_EVENT_MSG);
  }

  // Send a message out - This event is generated by a timer
  //  (setup in SampleApp_Init()).
  if ( events & SAMPLEAPP_SEND_PERIODIC_MSG_EVT )
  {
    // Send the periodic message
  //  SampleApp_SendPeriodicMessage();   /// 周期上传数据

    cutHeart++;
    if(cutHeart>=setHeart)
    {
  //  SampleApp_SendHartMessage("Carey");   /// 周期上传数据
      cutHeart=0;
    }
    cutPeriod++;
    if(cutPeriod>=setPeriod)
    {
      SampleApp_SendPeriodicMessage();   /// 周期上传数据
      cutPeriod=0;
    }

    // Setup to send message again in normal period (+ a little jitter)
    osal_start_timerEx( SampleApp_TaskID, SAMPLEAPP_SEND_PERIODIC_MSG_EVT,
        (SAMPLEAPP_SEND_PERIODIC_MSG_TIMEOUT + (osal_rand() & 0x00FF)) );

    // return unprocessed events
    return (events ^ SAMPLEAPP_SEND_PERIODIC_MSG_EVT);
  }

  // Discard unknown events
  return 0;
}

/*********************************************************************
 * Event Generation Functions
 */
/*********************************************************************
 * @fn      SampleApp_HandleKeys
 *
 * @brief   Handles all key events for this device.
 *
 * @param   shift - true if in shift/alt.
 * @param   keys - bit field for key events. Valid entries:
 *                 HAL_KEY_SW_2
 *                 HAL_KEY_SW_1
 *
 * @return  none
 */
void SampleApp_HandleKeys( uint8 shift, uint8 keys )
{
  (void)shift;  // Intentionally unreferenced parameter
  
  if ( keys & HAL_KEY_SW_1 )
  {
    /* This key sends the Flash Command is sent to Group 1.
     * This device will not receive the Flash Command from this
     * device (even if it belongs to group 1).
     */
    SampleApp_SendFlashMessage( SAMPLEAPP_FLASH_DURATION );
  }

  if ( keys & HAL_KEY_SW_2 )
  {
    /* The Flashr Command is sent to Group 1.
     * This key toggles this device in and out of group 1.
     * If this device doesn't belong to group 1, this application
     * will not receive the Flash command sent to group 1.
     */
    aps_Group_t *grp;
    grp = aps_FindGroup( SAMPLEAPP_ENDPOINT, SAMPLEAPP_FLASH_GROUP );
    if ( grp )
    {
      // Remove from the group
      aps_RemoveGroup( SAMPLEAPP_ENDPOINT, SAMPLEAPP_FLASH_GROUP );
    }
    else
    {
      // Add to the flash group
      aps_AddGroup( SAMPLEAPP_ENDPOINT, &SampleApp_Group );
    }
  }
}

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * @fn      SampleApp_MessageMSGCB
 *
 * @brief   Data message processor callback.  This function processes
 *          any incoming data - probably from other devices.  So, based
 *          on cluster ID, perform the intended action.
 *
 * @param   none
 *
 * @return  none
 */


void SampleApp_MessageMSGCB( afIncomingMSGPacket_t *pkt )
{
  uint16 flashTime;
 LED3_IO=IO_ON;

   memset(getBuf,0,50);
      
  switch ( pkt->clusterId )
  {
     uint8 i,len, str[50];     //len有用数据长度
     memset(str,0,50);
     case SAMPLEAPP_COM_CLUSTERID:     //如果是串口透传的信息
      len=pkt->cmd.Data[0];
      len=pkt->cmd.DataLength;
      for(i=0;i<len;i++)
      { HalUARTWrite(0,&pkt->cmd.Data[i],1);//发给PC机
      str[i]=pkt->cmd.Data[i];
      }
      HalUARTWrite(0,"\n",1);               // 回车换行
      
      
       HalUARTWrite(0,"I will do it\n",13);               // 回车换行
       
       if(strstr(str,NodeNum)!=NULL)
       {
             if( strstr(str,"Relay=1")!=NULL)
            {
                HalUARTWrite(0," Relay=1",7);   
                Relay_IO  =IO_ON;    
              //  sprintf((char *)Location,"26");
            //  strcpy( Location ,"26");

            }
            if( strstr(str,"Relay=0")!=NULL)
             {
                HalUARTWrite(0," Relay=0",7); 
                Relay_IO  =IO_OFF;    
              //  sprintf((char *)Location,"25");
            }
            
             if( strstr(str,"Find=1")!=NULL)
            {
                HalUARTWrite(0,"Find=1",6);   
                Find_IO  =IO_ON;     

            }
            if( strstr(str,"Find=0")!=NULL)
             {
                HalUARTWrite(0," Find=0",6); 
                Find_IO  =IO_OFF;     
            }
             if( strstr(str,"Heart=")!=NULL)
             {
                 char* Heart= strcutC(str, "Heart=", "s")  ;
                 setHeart=atoi(Heart);
                      //sprintf((char *)bus,"Heart==%ss",Heart);
                  //    SampleApp_SendBackMessage(NodeCONF,bus); 

             }
            if( strstr(str,"Period=")!=NULL)
             {
                 char* Period= strcutC(str, "Period=", "s")  ;
                 setPeriod=atoi(Period);
                    //  sprintf((char *)bus,"Period==%ss",Period);
                // SampleApp_SendBackMessage(NodeCONF,bus); 
            }

             if( strstr(str,"NodeNum=")!=NULL)
             {
             //  memset( _getBuf,0,50); 
               _getBuf = strcutC(str, "NodeNum=", "E;")  ;
                  sprintf((char *)NodeNum,"%s",_getBuf);
                    //  SampleApp_SendBackMessage(NodeCONF,bus); 
            }
             if( strstr(str,"MAC=")!=NULL)
             {
                 _getBuf = strcutC(str, "MAC=", "E;")  ;
                  sprintf((char *)MAC,"%s",_getBuf);
                    //  SampleApp_SendBackMessage(NodeCONF,bus); 
            }
             if( strstr(str,"Location=")!=NULL)
             {
                 _getBuf = strcutC(str, "Location=", "E;")  ;
                  sprintf((char *)Location,"%s",_getBuf);
                    //  SampleApp_SendBackMessage(NodeCONF,bus); 
            }
             if( strstr(str,"Parent=")!=NULL)
             {
                 _getBuf = strcutC(str, "Parent=", "E;")  ;
                  sprintf((char *)Parent,"%s",_getBuf);
                    //  SampleApp_SendBackMessage(NodeCONF,bus); 
            }
       }
      break;



    /*case SAMPLEAPP_PERIODIC_CLUSTERID:
      
      break;

    case SAMPLEAPP_FLASH_CLUSTERID:
      flashTime = BUILD_UINT16(pkt->cmd.Data[1], pkt->cmd.Data[2] );
      HalLedBlink( HAL_LED_4, 4, 50, (flashTime / 4) );
      break;*/
  }
  
    LED3_IO=IO_OFF;
}

/*********************************************************************
 * @fn      SampleApp_SendPeriodicMessage
 *
 * @brief   Send the periodic message.
 *
 * @param   none
 *
 * @return  none
 */

  uint8 sendBuf[100];
  unsigned char  temp[5];
   unsigned char humi[5];
void SampleApp_SendPeriodicMessage( void )
{
      HalUARTWrite(0,"I will send\n",12);               // 回车换行
      uint8 *T="device send\n"; 

memset(sendBuf,0,100);
  //redAM2302->humi_deci,redAM2302->temp_int
  AM2302_Read_Data(redAM2302) ;

   int value=0;
 //if((redAM2302->humi_deci+redAM2302->humi_int+redAM2302->temp_deci+redAM2302->temp_int)&0xFF==redAM2302->check_sum)
  {
    value=(int )redAM2302->temp_deci*256+redAM2302->temp_int;
   temp[0] = value/100+0x30;
  temp[1] = value%100/10+0x30;
  temp[2] = 0x2E;
  temp[3] = value%10+0x30;
  temp[4] = 0;
  
   value=(int )redAM2302->humi_deci*256+redAM2302->humi_int;
  humi[0]= value/100+0x30;
  humi[1]= value%100/10+0x30;
  humi[2]= 0x2E;
  humi[3]= value%10+0x30;
  humi[4]= 0;
    
  }
 /* else
  {
  }
  */
  
   sprintf((char *)sendBuf,"S:N=%s@Type=%s@MAC=%s@Location=%s@Parent=%s@Temp=%s&Humi=%sE;",NodeNum,Type,MAC,Location,Parent,temp,humi);
   LED1_IO=~LED1_IO;
   HalUARTWrite(0,sendBuf,strlen(sendBuf) );//换行  
   if ( AF_DataRequest( &SampleApp_Periodic_DstAddr, &SampleApp_epDesc,
                       SAMPLEAPP_PERIODIC_CLUSTERID,
                       strlen(sendBuf),
                       sendBuf,
                       &SampleApp_TransID,
                       AF_DISCV_ROUTE,
                       AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
  {
  }
  else
  {
    // Error occurred in request to send.
  }
}

/*********************************************************************
 * @fn      SampleApp_SendFlashMessage
 *
 * @brief   Send the flash message to group 1.
 *
 * @param   flashTime - in milliseconds
 *
 * @return  none
 */
void SampleApp_SendFlashMessage( uint16 flashTime )
{
  uint8 buffer[3];
  buffer[0] = (uint8)(SampleAppFlashCounter++);
  buffer[1] = LO_UINT16( flashTime );
  buffer[2] = HI_UINT16( flashTime );

  if ( AF_DataRequest( &SampleApp_Flash_DstAddr, &SampleApp_epDesc,
                       SAMPLEAPP_FLASH_CLUSTERID,
                       3,
                       buffer,
                       &SampleApp_TransID,
                       AF_DISCV_ROUTE,
                       AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
  {
  }
  else
  {
    // Error occurred in request to send.
  }
}




void SampleApp_SendHartMessage(  uint8 *sendStr )
{
    
memset(sendBuf,0,100);
     LED1_IO=~LED1_IO;
     
      sprintf((char *)sendBuf,"S:N=%s@MAC=%s@Location=%s@Parent=%s@%sE;",NodeNum,MAC,Location,Parent,sendStr);
      HalUARTWrite(0,sendBuf,strlen(sendBuf) );//换行  
      if ( AF_DataRequest( &SampleApp_Periodic_DstAddr, &SampleApp_epDesc,
                       SAMPLEAPP_PERIODIC_CLUSTERID,
                       strlen(sendBuf),
                       sendBuf,
                       &SampleApp_TransID,
                       AF_DISCV_ROUTE,
                       AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
      {
      }
      else
      {
        // Error occurred in request to send.
      }
}


void SampleApp_SendEVTMessage(  uint8 *sendStr )
{
memset(sendBuf,0,100);
     // LED1_IO=~LED1_IO;
       sprintf((char *)sendBuf,"S:N=%s@MAC=%s@Location=%s@Parent=%s@%sE;",NodeNum,MAC,Location,Parent,sendStr);
      HalUARTWrite(0,sendBuf,strlen(sendBuf) );//换行  
      if ( AF_DataRequest( &SampleApp_Periodic_DstAddr, &SampleApp_epDesc,
                       SAMPLEAPP_PERIODIC_CLUSTERID,
                       strlen(sendBuf),
                       sendBuf,
                       &SampleApp_TransID,
                       AF_DISCV_ROUTE,
                       AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
      {
      }
      else
      {
        // Error occurred in request to send.
      }
}


/*********************************************************************
*********************************************************************/
void SampleApp_SerialCMD(mtOSALSerialData_t *cmdMsg)
{
  uint8 i,len,*str=NULL;     //len有用数据长度
  str=cmdMsg->msg;          //指向数据开头
  len=*str;                 //msg里的第1个字节代表后面的数据长度

  /********打印出串口接收到的数据，用于提示*********/

  for(i=1;i<=len;i++)
  HalUARTWrite(0,str+i,1 ); 
  HalUARTWrite(0,"\n",1 );//换行  
  
  

  /*******发送出去***参考网蜂 1小时无线数据传输教程*********/
  if ( AF_DataRequest( &SampleApp_Periodic_DstAddr, &SampleApp_epDesc,
	                    SAMPLEAPP_COM_CLUSTERID,//自己定义一个
	                    len+1,                  // 数据长度         
                            str,                     //数据内容
	                    &SampleApp_TransID, 
	                    AF_DISCV_ROUTE,
	                    AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
	                     {
	                     }
  else
  {
  // Error occurred in request to send.
  } 
}



char *newstr =NULL;
 char *strtmp1=NULL;
    char *strtmp2=NULL;
char* strcutC(char*  old, char* start, char*  end)
{
    
 //   char* strtmp1;
 //   char*  strtmp2 ;
    if( (strstr(old, start) != NULL) &&(strstr(old, end) != NULL))
    {
       strtmp1 = strstr(old, start)+strlen(start);
      // mDelay(1);
       strtmp2 = strstr(old, end);
        memset(newstr,0,(strtmp2-strtmp1+1));
        strncpy(newstr,strtmp1,(strlen(strtmp1)-strlen(strtmp2)));
        return newstr;
    }
    else
    {
        return newstr="No match";
    }

}
