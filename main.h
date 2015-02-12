/*************************************************************************************
*
Description :  NeilScope3 Main header
Version     :  1.0.1
Date        :  7.12.2011
Author      :  Left Radio                          
Comments:  
*
**************************************************************************************/

#ifndef __MAIN_H
#define __MAIN_H 

/* Includes ------------------------------------------------------------------*/	   
#include "stm32f10x.h"
#include <stdio.h>
#include <string.h>

/* Exported define -----------------------------------------------------------*/
#define ON	    			1
#define OFF					0

#define Sync_Condition 		0
#define Sync_Different 		1
#define Sync_C_or_D	 		2
#define Sync_C_and_D 		3
#define oscSyncMode_MAX		3

#define ON_OFF_button	(GPIOC->IDR & GPIO_Pin_14)

/* Exported typedef -----------------------------------------------------------*/

/* ---------------------------------------- Описание вспомогательных структур ---------------------------------------- */

/* обозначения кнопок пользователя */
typedef enum { LEFT = 0x1E, RIGHT = 0x1D, UP = 0x17, DOWN = 0x1B, OK = 0x0F, NO_Push = 0x1F } ButtonsCode_TypeDef;
typedef enum { SHORT_SET = 0x01, LONG_SET = 0x02, B_RESET = 0xFF } ButtonsPush_TypeDef;

/* ID и состояния для каналов */
typedef enum {CHANNEL_A = (uint8_t)0, CHANNEL_B = (uint8_t)1, CHANNEL_DIGIT = (uint8_t)2, BOTH_CHANNEL = (uint8_t)3} Channel_ID_TypeDef;
typedef enum {NONE = (int8_t)0, RUN_AC = (int8_t)1, RUN_DC = (int8_t)2 } Channel_AC_DC_TypeDef;

/* виды синхронизации, типы синхронизации по аналоговым и цифровому каналам */
typedef enum {Sync_NONE = (int8_t)0, Sync_NORM = (int8_t)1, Sync_AUTO = (int8_t)2, Sync_SINGL = (int8_t)3 } SyncMode_TypeDef;
typedef enum {Sync_Rise = (int8_t)0, Sync_Fall = (int8_t)1, Sync_IN_WIN = (int8_t)2, Sync_OUT_WIN = (int8_t)3 } SyncAType_TypeDef;

/* структура для таймера автоотключения прибора */
typedef struct { uint32_t ResetTime; int8_t Work_Minutes; uint8_t Vbatt; FunctionalState State; } OFF_Struct_TypeDef;	// Auto off struct

/* Общая структура состояния, используетя при запуске останове работы осцилографа и т.п. */
typedef enum { STOP = 0, RUN = 1, COMPLETE = 2 } State_TypeDef;


/* ---------------------------------------- Описание структур каналов ---------------------------------------- */

/* Структурный тип данных режим работы каналов */
typedef struct
{
	Channel_ID_TypeDef		ID;  			// индификатор канала
	State_TypeDef			EN;     		// состояние - включен/выключен
	Channel_AC_DC_TypeDef	AC_DC;			// 1 - AC, 2 - DC
} gChannel_MODE;


/* Структурный тип данных для аналоговых каналов */
typedef struct
{
	uint8_t Div;									// положение аналогового делителя
	uint16_t Zero_PWM_values[12];					// массив значений ШИМ для каждого положения аналогового делителя
	volatile uint16_t *corrZ;
//	volatile uint8_t fft_Mag[256];
} ACH_INFO;

/* Структурный тип данных цифрового канала для логического анализатора */
typedef struct
{


} DCH_INFO;


/* Общий структурный тип данных каналов */
typedef struct
{
	/* Общие поля */
	uint16_t Color;									// Цвет канала
	uint8_t Position;								// координата на экране указателя канала
	gChannel_MODE Mode;
	int8_t DATA[800];								// данные канала
	uint8_t visDATA[800];            				// данные для вывода на экран
	FunctionalState OldData_PointsFlag[400];		// данные для очистки экрана

	/* Уникальные данные аналоговых или цифрового каналов */
	union
	{
		ACH_INFO Analog;
		DCH_INFO Digital;

	} AD_Type;

	void (*Procesing)(gChannel_MODE CH_Mode);

} CH_INFO_TypeDef;


/* ---------------------------------------- Описание общих структур и переменных ---------------------------------------- */

/* Структурный тип данных общего режима работы осциллографа */
typedef struct
{
	uint32_t oscNumPoints;          	// Количество точек для записи в память
	uint16_t WindowWidh;
	int16_t WindowPosition;
	uint32_t oscSweep;		        	// длительность развертки
	Boolean Interleave;
	SyncMode_TypeDef  oscSync;			// Cинхронизация, 0 - нет, 1 - ждущая, 2 - авто, 3 - однократная
	Channel_ID_TypeDef  SyncSourse;		// Синхронизация по каналу А, B, цифра
	SyncAType_TypeDef  AnalogSyncType;	// Режим синхронизации, по фронту 1, по спаду 2, вход/выход из окна - по входу 3, по выходу 4
	Boolean  oscPreHistory;	        	// Предыстория, TRUE/FALSE
	Boolean  autoMeasurments;	    	// Автоизмерения,  TRUE/FALSE
	State_TypeDef  State;     			// Режим работы STOP/RUN, STOP - 0, RUN - 1

	OFF_Struct_TypeDef OFF_Struct;

} OscMode_TypeDef;


/* Exported variables --------------------------------------------------------*/
extern CH_INFO_TypeDef *pINFO, INFO_A, INFO_B, DINFO_A;		// указатель и переменные каналов
extern OscMode_TypeDef  gOSC_MODE;
extern volatile OscMode_TypeDef  *pnt_gOSC_MODE;	        // указатель на переменную общего режима работы осциллографа

extern FlagStatus show_FPS_flag;
extern uint8_t FPS_counter;					// переменные для подсчета количества выводимых кадров за сек

extern int8_t ADC_VbattPrecent;
extern FlagStatus show_ADC_flag;
extern FlagStatus MessageEvent;

extern FlagStatus ButtonEnable;
extern ButtonsCode_TypeDef ButtonsCode;	  	// кнопоки
extern ButtonsPush_TypeDef ButtonPush;     	// флаг нажатия на кнопку
extern uint8_t speed_up_cnt;      			//


/* Exported function --------------------------------------------------------*/
extern void (*pMNU)(void);	 /* указатель на текущую функцию меню */




#endif /* __MAIN_H */
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
