/**
  ******************************************************************************
  * @file	 	main.h
  * @author  	Neil Lab :: Left Radio
  * @version 	v2.5.0
  * @date
  * @brief		header
  ******************************************************************************
**/

#ifndef __MAIN_H
#define __MAIN_H 

/* Includes ------------------------------------------------------------------*/	   
#include "stm32f10x.h"
#include <stdio.h>
#include <string.h>

/* Exported define -----------------------------------------------------------*/
#define ON	    			1
#define OFF					0

//#define Sync_Condition 		0
//#define Sync_Different 		1
//#define Sync_C_or_D	 		2
//#define Sync_C_and_D 		3
//#define oscSyncMode_MAX		3

#define ON_OFF_button	(GPIOC->IDR & GPIO_Pin_14)

/* Exported typedef -----------------------------------------------------------*/

/* Work mode for device */
typedef enum { OSC_MODE = (uint8_t)0, LA_MODE = (uint8_t)1} OSC_LA_Mode_Typedef;
typedef enum { STOP = 0, RUN = 1, COMPLETE = 2 } State_TypeDef;

/* Buttons */
typedef enum { LEFT = 0x1E, RIGHT = 0x1D, UP = 0x17, DOWN = 0x1B, OK = 0x0F, NO_Push = 0x1F } ButtonsCode_TypeDef;
typedef enum { SHORT_SET = 0x01, LONG_SET = 0x02, B_RESET = 0xFF } ButtonsPush_TypeDef;

/* Channels ID */
typedef enum {
	CHANNEL_A = (uint8_t)0,
	CHANNEL_B = (uint8_t)1,
	CHANNEL_DIGIT = (uint8_t)2,
	BOTH_CHANNEL = (uint8_t)3
} Channel_ID_TypeDef;

/* ADC input AC/DC state */
typedef enum { NONE = (int8_t)0, RUN_AC = (int8_t)1, RUN_DC = (int8_t)2 } Channel_AC_DC_TypeDef;

/* Synchronization work mode, type */
typedef enum {Sync_NONE = (int8_t)0, Sync_NORM = (int8_t)1, Sync_AUTO = (int8_t)2, Sync_SINGL = (int8_t)3 } SyncMode_TypeDef;
typedef enum {
	Sync_Rise = (int8_t)0, Sync_Fall = (int8_t)1,
	Sync_IN_WIN = (int8_t)2, Sync_OUT_WIN = (int8_t)3,
	Sync_LA_State = (int8_t)4, Sync_LA_Different = (int8_t)5,
	Sync_LA_State_AND_Different = (int8_t)6, Sync_LA_State_OR_Different = (int8_t)7
} SyncAType_TypeDef;

/* Auto OFF device timer */
typedef struct { uint32_t ResetTime; int8_t Work_Minutes; uint8_t Vbatt; FunctionalState State; } OFF_Struct_TypeDef;	// Auto off struct

/* BackLight and power state */
typedef enum { BCKL_MIN = 0, BCKL_MAX = 1 } BcklightState_Typedef;
typedef enum { PWR_S_DISABLE = 0, PWR_S_ENABLE = 1} PwrSaveState_Typedef;


/* ---------------------------------------- Channels structs ---------------------------------------- */
/* ADC Channels mode struct */
typedef struct {
	Channel_ID_TypeDef		ID;  			// Id for channel
	State_TypeDef			EN;     		// ENABLE/DISABLE state
	Channel_AC_DC_TypeDef	AC_DC;			// 1 - AC, 2 - DC
} gChannel_MODE;


/* Структурный тип данных для аналоговых каналов */
typedef struct
{
	uint8_t Div;									// положение аналогового делителя
	uint16_t Zero_PWM_values[12];					// массив значений ШИМ для каждого положения аналогового делителя
	volatile uint16_t *corrZ;
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

///* */
//typedef struct {
//	uint8_t i2c_address;
//	int8_t (*Configuration)(void);
//	int8_t (*Write_Pin)(uint32_t pin, uint8_t state);
//	int8_t (*Write_Port)(uint32_t val);
//	int8_t (*Read_Pin)(uint32_t pin);
//	int8_t (*Read_Port)(void* data);
//} NS_I2C_GPIO_TypeDef;

/* */
typedef enum {
	Host_Undefined = (int8_t)-1,
	Host_OFF = (int8_t)0,
	Host_Bridge_Mode = (int8_t)1,
	Host_CP2102_Mode = (int8_t)2,
	Host_ESP_Mode = (int8_t)3,
	Host_ESP_Boot_Mode = (int8_t)4,
} NS_Host_Communicate_TypeDef;

/* Global device work mode/state struct  */
typedef struct
{
	uint32_t oscNumPoints;

	OSC_LA_Mode_Typedef Mode;
	Boolean Interleave;
	Boolean  autoMeasurments;
	PwrSaveState_Typedef PowerSave;
	BcklightState_Typedef BackLight;
	uint8_t i2c_gpio_chip_index;
	NS_Host_Communicate_TypeDef HostCommunicate;
	FunctionalState BeepState;
	State_TypeDef  State;
	Boolean Configurated;

} OscMode_TypeDef;


/* Samples window struct */
typedef struct {

	uint16_t WindowWidh;
	int16_t WindowPosition;
	uint16_t WindowsNum;
	uint32_t Sweep;

} SamplesWin_Typedef;


/* Exported variables --------------------------------------------------------*/
extern CH_INFO_TypeDef *pINFO, INFO_A, INFO_B, DINFO_A;
extern volatile OscMode_TypeDef  gOSC_MODE;
extern volatile SamplesWin_Typedef gSamplesWin;
extern OFF_Struct_TypeDef AutoOff_Timer;

extern FlagStatus show_FPS_flag;
extern uint8_t FPS_counter;

extern int8_t ADC_VbattPrecent;
extern FlagStatus show_ADC_flag;
extern FlagStatus MessageEvent;

extern FlagStatus ButtonEnable;
extern ButtonsCode_TypeDef ButtonsCode;
extern ButtonsPush_TypeDef ButtonPush;
extern uint8_t speed_up_cnt;


/* Exported function --------------------------------------------------------*/
extern void (*pMNU)(void);	 /* Pointer to actived menu func */




#endif /* __MAIN_H */
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
