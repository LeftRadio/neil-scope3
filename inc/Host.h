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
typedef enum { NO_Request, Data_Request, Calibrate_Request, Bootoader_Request} HostRequestType;

/* Host Request struct type */
typedef struct
{
	FunctionalState State;
	HostRequestType Request;
	uint32_t RequestDataLen;

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
void Switch_To_AutoMode(void);
void Host_RequestReset(void);
FlagStatus Host_GetTerminateCmd(void);




#endif /* __HOST_H */
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
