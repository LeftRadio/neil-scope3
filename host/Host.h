/**
  ******************************************************************************
  * @file	 	Host.h
  * @author  	Left Radio
  * @version 	1.5.6
  * @date
  * @brief		NeilScope3 Host header
  ******************************************************************************
**/

#ifndef __HOST_H
#define __HOST_H 

/* Includes ------------------------------------------------------------------*/	   
/* Exported typedef ----------------------------------------------------------*/
typedef enum { NO_Request, Data_Request, Calibrate_Request, Disconnect, Bootoader_Request} HostRequestType;

/* Host Request struct type */
typedef struct
{
	FunctionalState State;
	HostRequestType Request;
	uint32_t DataLen;

} HostRequest_TypeDef;

/* Exported define -----------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
extern uint8_t OUT_HostData[30];

extern FunctionalState HostMode;
extern Boolean SamplesData_ACK;
extern __IO HostRequest_TypeDef gHostRequest;

/* Exported function ---------------------------------------------------------*/
void Recive_Host_Data(uint8_t IQueueIndex);
void Transmit_To_Host(uint8_t Respond_CMD, uint8_t *pData, uint8_t DataLen);
void Transmit_DataBuf_To_Host(void);
void Host_RequestReset(void);
FlagStatus Host_GetTerminateCmd(void);

uint8_t CRC8_Buff(uint8_t *pBuff, uint16_t NumBytes);
uint8_t CRC8(uint8_t Byte, uint8_t crc);



#endif /* __HOST_H */
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
