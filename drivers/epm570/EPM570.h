/*************************************************************************************
*
Description :  NeilScope3 EPM570 header
Version     :  1.0.0
Date        :  7.12.2011
Author      :  Left Radio                          
Comments:  
*
**************************************************************************************/

#ifndef __EPM570_H
#define __EPM570_H

/* Includes ------------------------------------------------------------------*/
/* Exported typedef -----------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
#define SRAM_READ_UP			0
#define SRAM_READ_DOWN			1


/* Exported variables --------------------------------------------------------*/
extern volatile FlagStatus SRAM_TimeoutState;
extern volatile int32_t TimeoutCnt;
extern volatile int8_t InterliveCorrectionCoeff;
extern volatile uint8_t LA_Diff_MASK, LA_Cond_MASK;
extern FunctionalState RLE_State;

/* Exported function ---------------------------------------------------------*/
void EPM570_Signals_Init(void);
EPM_ErrorStatus EPM570_Registers_Init(void);

void EPM570_Set_numPoints(uint32_t points);
uint32_t EPM570_Get_numPoints(void);

void EPM570_Set_Decimation(uint32_t value);
uint32_t EPM570_Get_Decimation(void);

void EPM570_SRAM_SetWriteState(State_TypeDef NewState);
State_TypeDef EPM570_SRAM_GetWriteState(void);
void EPM570_SRAM_ReadState(FunctionalState NewState);
void EPM570_SRAM_Shift(int32_t cnt, uint8_t UP_DOWN);
void EPM570_SRAM_ReadDirection(uint8_t UP_DOWN);
State_TypeDef EPM570_SRAM_Write(void);
void EPM570_SRAM_Read(void);

void EPM570_Update_SynchronizationMode(Channel_ID_TypeDef Sourse, SyncMode_TypeDef Mode, SyncAType_TypeDef Type);

void EPM570_Set_AnalogDigital_DataInput(OSC_LA_Mode_Typedef NewMode);
void EPM570_Set_LA_RLE_State(FunctionalState NewState);
FunctionalState EPM570_Get_LA_RLE_State(void);

void EPM570_Set_BackLight(BcklightState_Typedef bckLight_Val);
uint8_t EPM570_Read_Keys(void);



#endif /* __EPM570_H */
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
