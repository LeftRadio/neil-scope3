/**
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
#include "Processing_and_output.h"
#include "Host.h"
#include "IQueue.h"

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

/* инициализация переменной общего режима работы осциллографа */
OscMode_TypeDef gOSC_MODE =
{
	388,			// Количество точек для записи в память
	388,
	0,
	0,				// коэффициент развертки
	FALSE,			// Interleave
	Sync_NONE,		// Режим синхронизации, 0 - нет, 1 авто, 2 - однократная
	CHANNEL_A,		// Синхронизация по каналу А когда 0 и B когда 1
	0,				// Синхронизация, по уровню; по фронту 0, по спаду 1, вход/выход из окна - по входу - 2, по выходу 3
	OFF,			// Предыстория, вкл. 1, выкл. 0
	OFF,			// Автоизмерени, вкл. 1, выкл. 0
	RUN,			// Режим работы RUN/HOLD, HOLD - 0, RUN - 1

	/* OFF timer */
	{
		0,			// Reset(start counting) time
		0,			// OFF time
		0,			// Batt low voltage flag
		DISABLE,	// Auto OFF State
	}
};

/* указатель, тип OSC_MODE_type */
volatile OscMode_TypeDef  *pnt_gOSC_MODE = &gOSC_MODE;

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
extern __IO uint8_t IN_HostData[CMD_MAX_SIZE];

/* Extern function -----------------------------------------------------------*/
extern FlagStatus ADC_Ready(void);
void setCondition(uint8_t RUN_HOLD);
void (*pMNU)(void) = Change_Menu_Indx;     /* указатель на функцию меню */


/* Private function prototypes -----------------------------------------------*/
/* Private Functions --------------------------------------------------------*/
/*******************************************************************************
* Function Name  : Main Function
* Description    : 
* Input          : None
* Return         : None
*******************************************************************************/
int main(void)
{
	__disable_irq();

	/* Init internal and external peripheal */
	Global_Init();

	/* Oscillocope run in autonome mode */
	Switch_To_AutoMode();

	__enable_irq();

	/* Main work */
	while(1)
	{
		if(Host_IQueue_Get_CommandsCount() > 0)
		{
			if(Host_IQueue_GetReadStatus((uint8_t*)&IQueue_WorkIndex) == TRUE)
			{
#ifdef __MAIN_C_HOST_DEBUG__
				if(HostMode == ENABLE)
				{
					IQueue_TypeDef *IQueue = Host_GetIQueue(IQueue_WorkIndex);
					uint32_t c = Host_IQueue_Get_CommandsCount();

					ConvertToString(IQueue->Data[1], DBG_CMD, 3);
					ConvertToString(IQueue->CMD_Length, DBG_LEN, 3);
					ConvertToString(c, DBG_CNT, 2);
					if(IQueue_CommandCount_MAX < c) IQueue_CommandCount_MAX = c;
					ConvertToString(IQueue_CommandCount_MAX, DBG_CNT_MAX, 2);

					LCD_SetTextColor(White);
					LCD_PutStrig(5, 110, 0, DBG_CMD);
					LCD_PutStrig(5, 95, 0, DBG_LEN);
					LCD_PutStrig(5, 80, 0, DBG_CNT);
					LCD_PutStrig(5, 65, 0, DBG_CNT_MAX);

					SuccessWorkedCommand += c;
					ConvertToString(SuccessWorkedCommand, DBG_ALL_CNT, 5);
					LCD_PutStrig(5, 45, 0, DBG_ALL_CNT);
				}
#endif
				Recive_Host_Data(IQueue_WorkIndex);
				Host_IQueue_Clear(IQueue_WorkIndex);
				Host_IQueue_SetCommandCount(-1);
			}
		}


		if(HostMode != ENABLE)	// Автономная работа
		{
			UpdateAllCursors();						// обновлем курсоры

			/* Накапливаем, обрабатываем и выводим данные осциллограмм */
			INFO_A.Procesing(INFO_A.Mode);
			INFO_B.Procesing(INFO_B.Mode);

			/* Если режим однократной развертки то останавливаемся до перезапуска */
			if((pnt_gOSC_MODE->oscSync == Sync_SINGL) && (pnt_gOSC_MODE->State == RUN) && (SRAM_GetWriteState() == COMPLETE))
			{
				setCondition(STOP);
			}

			/* если были нажаты какие либо кнопки, то переходим в функцию
		 	   на которую сейчас указывает void (*pMNU)(void).
			--- Опрос кнопок происходит через чтение регистра ПЛИС по прерыванию таймера TIM2 --- */
			if(ButtonPush != B_RESET){ pMNU();	ButtonPush = B_RESET; ButtonsCode = NO_Push; }

			/* подсчет и отображение кадров/сек. */
			if(show_FPS_flag == SET){ UI_ShowFPS(FPS_counter); FPS_counter = RESET; }
			else FPS_counter++;

			/* обновление уровня заряда */
			if(show_ADC_flag == SET)
			{
				if(ADC_VbattPrecent <= 0)
				{
					if(pnt_gOSC_MODE->OFF_Struct.Vbatt != 1)
					{
						gMessage.TimeShow = 60;
						Show_Message("Connect Cargher, auto off in 1 min");
						t_WorkMinutes = pnt_gOSC_MODE->OFF_Struct.Work_Minutes;
						pnt_gOSC_MODE->OFF_Struct.ResetTime = RTC_GetCounter();
						pnt_gOSC_MODE->OFF_Struct.Vbatt = 1;
						pnt_gOSC_MODE->OFF_Struct.Work_Minutes = 1;
						pnt_gOSC_MODE->OFF_Struct.State = ENABLE;
						Draw_Batt(1, 1);
					}
				}
				else
				{
					if(pnt_gOSC_MODE->OFF_Struct.Vbatt == 1)
					{
						pnt_gOSC_MODE->OFF_Struct.Vbatt = 0;
						gMessage.TimeShow = 1;
						Clear_Message();
						pnt_gOSC_MODE->OFF_Struct.ResetTime = RTC_GetCounter();
						pnt_gOSC_MODE->OFF_Struct.Work_Minutes = t_WorkMinutes;
						pnt_gOSC_MODE->OFF_Struct.State = DISABLE;
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


/*******************************************************************************
 * Function Name  : setCondition
 * Description    : приостановка/запуск каналов
 * Input          : None
 * Return         : None
 *******************************************************************************/
void setCondition(State_TypeDef NewState)
{
	saveActiveButton(btn);
	setActiveButton(&btnRUN_HOLD);

	if(NewState == STOP) btn->Text = "STOP";
	else if(NewState == RUN) btn->Text = "RUN";
	else return;

	pnt_gOSC_MODE->State = NewState;

	if(saved_btn == &btnRUN_HOLD) LCD_DrawButton(btn, activeButton);
	else
	{
		LCD_DrawButton(btn, NO_activeButton);
		setActiveButton(saved_btn);
	}
}





