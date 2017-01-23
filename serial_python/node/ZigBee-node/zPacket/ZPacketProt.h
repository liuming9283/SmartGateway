/**************************************************************************************************
  Filename:       ZPacketProt.h
  Revised:        $Date: 2016-10-11$
  Revision:       $Revision: 0.1 $

  Description:    This file contains the Zigbee Packet Protocol definitions.


  Copyright

**************************************************************************************************/

#ifndef ZPACKETPROT_H
#define ZPACKETPROT_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "ZComDef.h"

/*********************************************************************
 * CONSTANTS
 */
#define ZPACKETPROT_DATA_SIZE_DH21	4
#define ZPACKETPROT_DATA_SIZE_BH1750	5
#define ZPACKETPROT_DATA_SIZE_SRD	11

#define ZPACKETPROT_DEVICE_TYPE_DH21	0x8001
#define ZPACKETPROT_DEVICE_TYPE_BH1750	0x8002
#define ZPACKETPROT_DEVICE_TYPE_SRD	0x8003

/*********************************************************************
 * GLOBAL VARIABLES
 */
 typedef enum {
	ZPACKETPROT_END_DEVICE_TYPE_DH21 = 0,
	ZPACKETPROT_END_DEVICE_TYPE_BH1750,
	ZPACKETPROT_END_DEVICE_TYPE_SRD,
	ZPACKETPROT_END_DEVICE_TYPE_MAX,
} ZPACKETPROT_END_DEVICE_TYPE;

 typedef struct
{
	uint16 startCode;		/* Start code 0xAAAA */
	uint8 dataLength;		/* Data length 16-128 */
	uint16 deviceType;	/* Device type 0x8001-0x800C */
	uint8 deviceNo[6];		/* Device number 010001 */
	uint16 netAddr;		/* Network address 0x9001-0x9FFF */
	uint8 optCode;		/* Operation code 0-0xFF */
	uint8 mdobusCRCH;	/* Mdobus CRCH */
	uint8 mdobusCRCL;	/* Mdobus CRCL */
	uint16 endCode;		/* End code 0x0D0A */
	uint8 dataContent[1];	/* Data content array */
} ZPacketProtMSGPacket_t;


 typedef struct
{
	uint16 startCode;		/* Start code 0xAAAA */
	uint8 dataLength;		/* Data length 16-128 */
	uint16 deviceType;	/* Device type 0x8001-0x800C */
	uint8 deviceNo[6];		/* Device number 010001 */
	uint16 netAddr;		/* Network address 0x9001-0x9FFF */
	uint8 optCode;		/* Operation code 0-0xFF */
	uint8 mdobusCRCH;	/* Mdobus CRCH */
	uint8 mdobusCRCL;	/* Mdobus CRCL */
	uint16 endCode;		/* End code 0x0D0A */
	uint8 dataContent[1];	/* Data content array */
} ZPacketMSG;


/*********************************************************************
 * PUBLIC FUNCTIONS
 */
ZPacketProtMSGPacket_t *ZPacketProt_NewZigbeeMSGPacket(uint8 *data,uint8 size);
bool ZPacketProt_CheckZigbeeMSGPacket(uint8 *packet);
ZPacketProtMSGPacket_t *ZPacketProt_GetZigbeeMSGPacket(uint8 *packet);
void ZPacketProt_SendPointToPointMessage_DH21( afAddrType_t *dstAddr, endPointDesc_t *srcEP,uint8 *transID,uint8 *data,uint8 size );
void ZPacketProt_SendPointToPointMessage_BH1750( afAddrType_t *dstAddr, endPointDesc_t *srcEP,uint8 *transID,uint8 *data,uint8 size );
void ZPacketProt_SendPointToPointMessage_SRD( afAddrType_t *dstAddr, endPointDesc_t *srcEP,uint8 *transID,uint8 *data,uint8 size );
void ZPacketProt_UpdateEndDeviceType( void );
ZPACKETPROT_END_DEVICE_TYPE ZPacketProt_GetEndDeviceType( void );

#ifdef __cplusplus
}
#endif

#endif /* ZPACKETPROT_H */

  
