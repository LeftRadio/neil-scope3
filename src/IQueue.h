/**
  ******************************************************************************
  * @file	 	IQueue.h
  * @author  	Left Radio
  * @version
  * @date
  * @brief		header
  ******************************************************************************
**/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _IQUEUE__H
#define _IQUEUE__H

/* Includes ------------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
#define CMD_MAX_SIZE		30
#define OUT_CMD_MAX_SIZE	30
#define IQUEUE_SIZE			4

/* Exported macro ------------------------------------------------------------*/
/* Exported typedef ----------------------------------------------------------*/
typedef struct
{
	Boolean IsEmpty;
	uint8_t Data[CMD_MAX_SIZE];
	uint8_t CMD_Length;
//	ErrorStatus WorkDone;
} IQueue_TypeDef;

/* Exported variables --------------------------------------------------------*/
extern volatile Boolean IQueue_CommandStatus;

/* Exported function ---------------------------------------------------------*/
void Host_IQueue_Initialization(void);
Boolean Host_IQueue_GetReadStatus(uint8_t *WorkIndex);
IQueue_TypeDef* Host_GetIQueue(uint8_t index);
uint8_t Host_IQueue_GetEmptyIndex(void);
void Host_IQueue_Clear(uint8_t index);
int8_t Host_IQueue_Get_CommandsCount(void);
void Host_IQueue_SetCommandCount(int8_t sign);




#endif /* _IQUEUE__H */
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
