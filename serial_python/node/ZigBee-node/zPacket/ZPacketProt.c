/**************************************************************************************************
  Filename:       ZPacketProt.c
  Revised:        $Date: 2016-10-11$
  Revision:       $Revision: 0.1 $

  Description:    Zigbee Packet Protocol (no Profile).


  Copyright
**************************************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include "AF.h"
#include "ZPacketProt.h"
#include "SampleApp.h"

#include  "MT_UART.h" //use for com


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
ZPACKETPROT_END_DEVICE_TYPE ZPacketProtEndDeviceType=ZPACKETPROT_END_DEVICE_TYPE_DH21;
uint8 ZPacketProtBrightness[ZPACKETPROT_DATA_SIZE_BH1750];//BH1750
ZPacketProtMSGPacket_t getZPacketProtMSGPacket;

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * NETWORK LAYER CALLBACKS
 */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
*********************************************************************/
ZPacketProtMSGPacket_t *ZPacketProt_NewZigbeeMSGPacket(uint8 *data,uint8 size)
{
	ZPacketProtMSGPacket_t *zigbeeMSGPacket;
	zigbeeMSGPacket = (ZPacketProtMSGPacket_t*)osal_mem_alloc( sizeof(ZPacketProtMSGPacket_t)+size-1 );
	osal_memcpy( zigbeeMSGPacket->dataContent, data, size );

	zigbeeMSGPacket->startCode = 0xAAAA;
	zigbeeMSGPacket->dataLength = 14+size;

	if (ZPacketProtEndDeviceType == ZPACKETPROT_END_DEVICE_TYPE_DH21)
	{
		zigbeeMSGPacket->deviceType = ZPACKETPROT_DEVICE_TYPE_DH21;
	}
	
	zigbeeMSGPacket->deviceNo[0] = 0x38;
	zigbeeMSGPacket->deviceNo[1] = 0x30;
	zigbeeMSGPacket->deviceNo[2] = 0x31;
	zigbeeMSGPacket->deviceNo[3] = 0x30;
	zigbeeMSGPacket->deviceNo[4] = 0x30;
	zigbeeMSGPacket->deviceNo[5] = 0x31;
	zigbeeMSGPacket->netAddr = 0x9001;
	zigbeeMSGPacket->optCode = 0x01;
	zigbeeMSGPacket->mdobusCRCH = 0x32;
	zigbeeMSGPacket->mdobusCRCL = 0x33;
	zigbeeMSGPacket->endCode = 0xD0A;

	return zigbeeMSGPacket;
}

/*********************************************************************
*********************************************************************/
bool ZPacketProt_CheckZigbeeMSGPacket(uint8 *packet)
{
	ZPacketProtMSGPacket_t * packetPtr = (ZPacketProtMSGPacket_t *)packet;

	if (packetPtr->mdobusCRCH == 0x32 &&
		packetPtr->mdobusCRCL == 0x33 &&
		packetPtr->endCode == 0x0D0A){
		return TRUE;
	}
	else{
		return FALSE;
	}
}

/*********************************************************************
*********************************************************************/
ZPacketProtMSGPacket_t *ZPacketProt_GetZigbeeMSGPacket(uint8 *packet)
{
	ZPacketProtMSGPacket_t * packetPtr = (ZPacketProtMSGPacket_t *)packet;
	
	getZPacketProtMSGPacket.startCode = packetPtr->startCode;
	getZPacketProtMSGPacket.dataLength = packetPtr->dataLength;
	getZPacketProtMSGPacket.deviceType = packetPtr->deviceType;
	getZPacketProtMSGPacket.deviceNo[0] = packetPtr->deviceNo[0];
	getZPacketProtMSGPacket.deviceNo[1] = packetPtr->deviceNo[1];
	getZPacketProtMSGPacket.deviceNo[2] = packetPtr->deviceNo[2];
	getZPacketProtMSGPacket.deviceNo[3] = packetPtr->deviceNo[3];
	getZPacketProtMSGPacket.deviceNo[4] = packetPtr->deviceNo[4];
	getZPacketProtMSGPacket.deviceNo[5] = packetPtr->deviceNo[5];
	getZPacketProtMSGPacket.netAddr = packetPtr->netAddr;
	getZPacketProtMSGPacket.optCode = packetPtr->optCode;
	getZPacketProtMSGPacket.mdobusCRCH = packetPtr->mdobusCRCH;
	getZPacketProtMSGPacket.mdobusCRCL = packetPtr->mdobusCRCL;
	getZPacketProtMSGPacket.endCode = packetPtr->endCode;

	if (packetPtr->deviceType == ZPACKETPROT_DEVICE_TYPE_DH21){
		osal_memcpy( getZPacketProtMSGPacket.dataContent, packetPtr->dataContent, ZPACKETPROT_DATA_SIZE_DH21 );
	}
	else if (packetPtr->deviceType == ZPACKETPROT_DEVICE_TYPE_BH1750){
		osal_memcpy( getZPacketProtMSGPacket.dataContent, packetPtr->dataContent, ZPACKETPROT_DATA_SIZE_BH1750 );
	}
	else if (packetPtr->deviceType == ZPACKETPROT_DEVICE_TYPE_SRD){
		osal_memcpy( getZPacketProtMSGPacket.dataContent, packetPtr->dataContent, ZPACKETPROT_DATA_SIZE_SRD );
	}

	return &getZPacketProtMSGPacket;
}

/*********************************************************************
*********************************************************************/
void ZPacketProt_SendPointToPointMessage_DH21( afAddrType_t *dstAddr, endPointDesc_t *srcEP,uint8 *transID,uint8 *data,uint8 size )
{
  ZPacketProtMSGPacket_t *zigbeeMSGPacket;
  zigbeeMSGPacket=ZPacketProt_NewZigbeeMSGPacket(data,size);

  HalUARTWrite(0,zigbeeMSGPacket->deviceNo,6);
  HalUARTWrite(0,":",1);
  HalUARTWrite(0,zigbeeMSGPacket->dataContent,2);
  HalUARTWrite(0,",",1);
  HalUARTWrite(0,zigbeeMSGPacket->dataContent+2,2);
  HalUARTWrite(0,"\n",1);              //
  
  osal_mem_free(zigbeeMSGPacket);
}

/*********************************************************************
*********************************************************************/
void ZPacketProt_SendPointToPointMessage_BH1750( afAddrType_t *dstAddr, endPointDesc_t *srcEP,uint8 *transID,uint8 *data,uint8 size )
{
  ZPacketProtMSGPacket_t *zigbeeMSGPacket;
  zigbeeMSGPacket=ZPacketProt_NewZigbeeMSGPacket(data,size);

  HalUARTWrite(0,zigbeeMSGPacket->deviceNo,6);
  HalUARTWrite(0,":",1);
  HalUARTWrite(0,zigbeeMSGPacket->dataContent,size);
  HalUARTWrite(0,"\n",1);              //


  osal_mem_free(zigbeeMSGPacket);
}

/*********************************************************************
*********************************************************************/
void ZPacketProt_SendPointToPointMessage_SRD( afAddrType_t *dstAddr, endPointDesc_t *srcEP,uint8 *transID,uint8 *data,uint8 size )
{
  ZPacketProtMSGPacket_t *zigbeeMSGPacket;
  zigbeeMSGPacket=ZPacketProt_NewZigbeeMSGPacket(data,size);

  HalUARTWrite(0,zigbeeMSGPacket->deviceNo,6);
  HalUARTWrite(0,":",1);
  HalUARTWrite(0,zigbeeMSGPacket->dataContent,size);
  HalUARTWrite(0,"\n",1);              //


  osal_mem_free(zigbeeMSGPacket);
}

void ZPacketProt_UpdateEndDeviceType( void )
{
	ZPacketProtEndDeviceType++;
	if (ZPacketProtEndDeviceType==ZPACKETPROT_END_DEVICE_TYPE_MAX)
	{
		ZPacketProtEndDeviceType=(ZPACKETPROT_END_DEVICE_TYPE)0;
	}
}

ZPACKETPROT_END_DEVICE_TYPE ZPacketProt_GetEndDeviceType( void )
{
	return ZPacketProtEndDeviceType;
}


/*********************************************************************
*********************************************************************/
void creat_ZPacket( uint8 *transID,uint8 *data,uint8 size )
{
  ZPacketProtMSGPacket_t *zigbeeMSGPacket;
  zigbeeMSGPacket=ZPacketProt_NewZigbeeMSGPacket(data,size);
  osal_mem_free(zigbeeMSGPacket);
}


