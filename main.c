﻿/**
  ******************************************************************************
  * @file	 	main.c
  * @author  	Left Radio
  * @version 	1.5.6
  * @date
  * @brief		NeilScope3 Main sourse
  ******************************************************************************
**/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "defines.h"
#include "main.h"
#include "init.h"
#include "Settings.h"
#include "User_Interface.h"
#include "EPM570.h"
#include "Synchronization.h"
#include "Processing_and_output.h"
#include "Host.h"
#include "IQueue.h"
#include "i2c_gpio.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t i;
uint8_t IQueue_WorkIndex = 0;
FunctionalState VisibleDigitSyncChannel = DISABLE;
FlagStatus show_FPS_flag = RESET;
uint8_t FPS_counter = 0;
float temp = 0;
uint8_t t_WorkMinutes = 0;

/* Global work mode for device */
__IO OscMode_TypeDef gOSC_MODE =
{
	388,				// Number points to write SRAM
	OSC_MODE,			// Oscilloscope or LogicAnalyzer work mode state
	FALSE,				// Interleave mode state
	OFF,				// Automeasurments state
	PWR_S_DISABLE,		// Power save mode state
	BCKL_MAX,
	(void*)0,			// Pointer fo external i2c ports chip
	ENABLE,				// Beeper state
	RUN,				// Work state
	FALSE				// Configure state
};

/*  */
__IO SamplesWin_Typedef gSamplesWin = {
		388,			// Sample 1 window width
		0,				// Active window position
		1,				// Windows number in gOSC_MODE.NumPoints
		0,				// Horizontal scale coefficient
};

/* Timer for auto OFF power when Vbatt is less the 3.45V, or manual OFF timeout */
OFF_Struct_TypeDef AutoOff_Timer = {
		0,			// Reset(start counting) time
		0,			// OFF time
		0,			// Batt low voltage flag
		DISABLE,	// Auto OFF State
};

#ifdef __MAIN_C_HOST_DEBUG__
	char DBG_CMD[5] = { 0 };
	char DBG_LEN[5] = { 0 };
	char DBG_CNT[5] = { 0 };
	char DBG_CNT_MAX[10] = { 0 };
	char DBG_ALL_CNT[10] = { 0 };
	__IO int8_t IQueue_CommandCount_MAX = 0;
	__IO uint16_t SuccessWorkedCommand = 0;
#endif
#ifdef __VAR_DEBUG__
//	__IO uint32_t dCommandCounter = 0;
//	__IO uint32_t dTrDataCounter = 0;
#endif

extern btnINFO btnRUN_HOLD;
extern volatile uint8_t IN_HostData[CMD_MAX_SIZE];

/* Extern function -----------------------------------------------------------*/
extern FlagStatus ADC_Ready(void);
void setCondition(uint8_t RUN_HOLD);
void (*pMNU)(void) = Change_Menu_Indx;     /* указатель на функцию меню */

uint16_t portData = 0;

/* Private function prototypes -----------------------------------------------*/
/* Private Functions --------------------------------------------------------*/

/**
 * @brief  main cycle
 * @param  None
 * @retval None
 */
int main(void)
{
	__disable_irq();

	/* Init internal and external peripheal */
	Global_Init();

	/* Init and verify external peripheal */
	External_Peripheral_Init();

	/* Load preference from EEPROM */
	LoadPreference();

	/* Config external I2C GPIO */
	I2CIO_Configuration(gOSC_MODE.i2c_gpio_chip);

	/* Draw main interface */
	Draw_Interface();

	/* other */
	EPM570_Set_LA_RLE_State(DISABLE);
	GPIOC->BRR = GPIO_Pin_13;

	gOSC_MODE.Configurated = TRUE;

	NVIC_EnableIRQ(RTC_IRQn);
	__enable_irq();


	//	I2CIO_Write_Pin(GPIO_Pin_0, 0);
	//	portData = I2CIO_Read_Pin(GPIO_Pin_0);
	//	I2CIO_Write_Port(0xFF0F);
	//	portData = I2CIO_Read_Pin(GPIO_Pin_0);
	//	I2CIO_Read_Port(&portData);


	/* Main work */
	while(1)
	{

#ifndef __SWD_DEBUG__
		if(gOSC_MODE.PowerSave == PWR_S_ENABLE)	__WFI();
#endif

		if(Host_IQueue_Get_CommandsCount() > 0)	{
			if(Host_IQueue_GetReadStatus((uint8_t*)&IQueue_WorkIndex) == TRUE) {
				Recive_Host_Data(IQueue_WorkIndex);
				Host_IQueue_Clear(IQueue_WorkIndex);
				Host_IQueue_SetCommandCount(-1);
			}
		}


		if(HostMode != ENABLE)
		{
			UpdateAllCursors();

			/* Накапливаем, обрабатываем и выводим данные осциллограмм */
			INFO_A.Procesing(INFO_A.Mode);
			INFO_B.Procesing(INFO_B.Mode);

			/* Если режим однократной развертки то останавливаемся до перезапуска */
			if((gSyncState.Mode == Sync_SINGL) && (gOSC_MODE.State == RUN) && (EPM570_SRAM_GetWriteState() == COMPLETE))
			{
				setCondition(STOP);
			}

			/* если были нажаты какие либо кнопки, то переходим в функцию
		 	   на которую сейчас указывает void (*pMNU)(void).
			--- Опрос кнопок происходит через чтение регистра ПЛИС по прерыванию таймера TIM2 --- */
			if(ButtonPush != B_RESET){ pMNU(); ButtonPush = B_RESET; ButtonsCode = NO_Push; }


			/* подсчет и отображение кадров/сек. */
			if(show_FPS_flag == SET){ UI_ShowFPS(FPS_counter); FPS_counter = RESET; }
			else FPS_counter++;

			/* обновление уровня заряда */
			if(show_ADC_flag == SET)
			{
				if(ADC_VbattPrecent <= 0)
				{
					if(AutoOff_Timer.Vbatt != 1)
					{
						gMessage.TimeShow = 60;
						Show_Message("Connect Cargher, auto off in 1 min");
						t_WorkMinutes = AutoOff_Timer.Work_Minutes;
						AutoOff_Timer.ResetTime = RTC_GetCounter();
						AutoOff_Timer.Vbatt = 1;
						AutoOff_Timer.Work_Minutes = 1;
						AutoOff_Timer.State = ENABLE;
						Draw_Batt(1, 1);
					}
				}
				else
				{
					if(AutoOff_Timer.Vbatt == 1)
					{
						AutoOff_Timer.Vbatt = 0;
						gMessage.TimeShow = 1;

						Clear_Message();

						AutoOff_Timer.ResetTime = RTC_GetCounter();
						AutoOff_Timer.Work_Minutes = t_WorkMinutes;
						AutoOff_Timer.State = DISABLE;
					}

					Draw_Batt(ADC_VbattPrecent, 1);
				}

				show_ADC_flag = RESET;
			}

			/* Message event */
			if(MessageEvent == SET){ Clear_Message(); MessageEvent = RESET;	}
		}
	}
}


/**
 * @brief  setCondition, global work state - RUN or HOLD
 * @param  None
 * @retval None
 */
void setCondition(State_TypeDef NewState)
{
	static Boolean pwr;

	/* Switch to power save in STOP work state */
	if(NewState == STOP)
	{
		pwr = gOSC_MODE.PowerSave;
		gOSC_MODE.PowerSave = TRUE;

		/* Update button RUN/HOLD */
		btnRUN_HOLD.Text = "STOP";
	}
	else
	{
		if(gOSC_MODE.State == STOP) gOSC_MODE.PowerSave = pwr;

		/* Update button RUN/HOLD */
		btnRUN_HOLD.Text = "RUN";
	}

	/* Set the new global work state */
	gOSC_MODE.State = NewState;

	if(btn == &btnRUN_HOLD)	LCD_DrawButton(&btnRUN_HOLD, activeButton);
	else LCD_DrawButton(&btnRUN_HOLD, NO_activeButton);
}




/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
