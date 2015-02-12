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
//typedef enum { wrSRAM_STOP = 0, wrSRAM_START = 1, wrSRAM_DONE = 2} wrSRAM_TypeDef;

/* Exported define -----------------------------------------------------------*/
#define SAVE_REG      			0
#define RESTORE_REG	  			1

#define SRAM_READ_UP			0
#define SRAM_READ_DOWN			1

/* Exported variables --------------------------------------------------------*/
extern uint8_t ConditionState_Sync_Var;
extern uint8_t DifferentState_Sync_Var;
extern FunctionalState DifferentState_Sync_Rise;
extern FunctionalState DifferentState_Sync_Fall;

extern __IO FlagStatus SRAM_TimeoutState;
extern __IO int32_t TimeoutCnt;		//Counter for timeout, value in ms
//extern volatile wrSRAM_TypeDef wrSRAM;  // флаг сигнализирующий о записи в память
extern __IO int8_t InterliveCorrectionCoeff;

/* Exported function ---------------------------------------------------------*/
void Set_EPM570_RS(FlagStatus NewSets);
void EPM570_Signals_Init(void);		              	// Инициализация выводов МК под связь с EPM570
EPM_ErrorStatus EPM570_Registers_Init(void);	           	// Инициализация регистров EPM570 и проверка записанных значений
void SAVE_RESTORE_Address(uint8_t SAVE_RESTORE);	  	// Сохранение/восстановление адреса регистра
uint8_t EPM570_Read_Keys(void);					  	// Чтение состояния кнопок
void EPM570_Sync(SyncMode_TypeDef NewMode);			// Set new sync mode
void Set_Input(uint8_t Channel, uint8_t INDX);	  	// открытый или закрытый вход

void SRAM_SetWriteState(State_TypeDef NewState);
State_TypeDef SRAM_GetWriteState(void);
void SRAM_ReadState(FunctionalState NewState);		// Подготовка/завершение чтения данных из памяти
void SRAM_ReadDirection(uint8_t UP_DOWN);
State_TypeDef Write_SRAM(void);					  	// Цикл записи в память
void Read_SRAM(void);							  	// Цикл чтения из памяти

void Set_Decimation(uint32_t value);				  	// устанавливаем коэффициент развертки в регистрах ПЛИС
void Set_Trigger(uint8_t NewSyncMode);			  	// устанавливаем режим триггера/синхронизации в регистрах ПЛИС
void Set_numPoints(uint32_t points);				  	// установить количество точек для записи в память
uint32_t Get_numPoints(void);
void EPM570_ChangeBackLight(uint8_t bckLight_Val); 	// Изменить яркость подсветки
uint8_t Get_EPM570_Register_Operate_Status(void);  	// чтение статуса операций над регистрам ПЛИС


#endif /* __EPM570_H */
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

