/**
  ******************************************************************************
  * @file	 	Synchronization.h
  * @author  	Neil Lab :: Left Radio
  * @version 	v1.0.0
  * @date
  * @brief		header
  ******************************************************************************
**/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SYNCHRONIZATION_H
#define __SYNCHRONIZATION_H

/* Includes ------------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported typedef ----------------------------------------------------------*/

typedef struct
{
	void (*StateUpdate)(void);
	void (*StateReset)(void);
	uint32_t (*Calc_WriteNumPoints)(uint32_t NumPoints, uint8_t PreTiggCoeff, uint8_t PostTrigCoeff);
	uint32_t (*Calc_ReadNumPoints)(uint32_t WinRegVal, uint8_t PostTrigCoeff);
	void (*SetTrigg_X)(uint32_t NewPosition);
	void (*PreTrigger_Delay)(void);
} Foops_TypeDef;

typedef struct
{
	Channel_ID_TypeDef  Sourse;
	SyncMode_TypeDef    Mode;
	SyncAType_TypeDef   Type;
	uint32_t preTriggerTime_us;
	uint32_t Cursor_X;
	const Foops_TypeDef* foops;

} SyncGlobalState_TypeDef;


/* Exported variables --------------------------------------------------------*/
extern volatile SyncGlobalState_TypeDef gSyncState;

/* Exported function ---------------------------------------------------------*/






#endif /* __SYNCHRONIZATION_H */
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
