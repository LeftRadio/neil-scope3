/*************************************************************************************
*
Description :  NeilScope3 EPM570
Version     :  1.0.0
Date        :  7.12.2011
Author      :  Left Radio                          
Comments:  
*
**************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include <math.h>

#include "main.h"
#include "Host.h"
#include "systick.h"
#include "Analog.h"
#include "EPM570.h"
#include "EPM570_Registers.h"
#include "Processing_and_output.h"
#include "Settings.h"
#include "gInterface_MENU.h"
#include "Trig_Menu.h"
#include "HX8352_Library.h"
#include "colors 5-6-5.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define TrigTimeout				0x001FFFFF

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t preTrigger_time = 1;

/* Exported variables --------------------------------------------------------*/
uint8_t ConditionState_Sync_Var = 0;
uint8_t DifferentState_Sync_Var = 0;
FunctionalState DifferentState_Sync_Rise = DISABLE;
FunctionalState DifferentState_Sync_Fall = DISABLE;

__IO uint8_t timeout_flag = 0;
__IO uint8_t wrSRAM = wrSRAM_STOP;       // флаг сигнализирующий о записи в память 


/* Private function prototypes -----------------------------------------------*/
uint32_t Get_Decimation(void);		     // Чтение коэффициента прореживания
uint32_t Get_numPoints(void);		     // Чтение количество точек для записи 
void delay_preTrigger(uint32_t us_del);

extern void IndicateTrigTimeout(FlagStatus new_state);


/* Функции -------------------------------------------------------------------*/

/*******************************************************************************
* Function Name  : Set_EPM570_RS
* Description    :
* Input          : None
* Return         : None
*******************************************************************************/
void Set_EPM570_RS(FlagStatus NewSets)
{
	if(NewSets != RESET) EPM570_RS_set;
	else EPM570_RS_clr;
}


/*******************************************************************************
* Function Name  : EPM570_Signals_Init
* Description    : Инициализация выводов МК под связь с EPM570
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EPM570_Signals_Init(void)
{  	  
	GPIO_InitTypeDef EPM570_GPIO_InitStructure;

	Set_EPM570_Port(IN);  // Настройк порта, вход
	
	EPM570_GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_1;	              // WR
	EPM570_GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;
	EPM570_GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &EPM570_GPIO_InitStructure);
	
	EPM570_GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_6 | GPIO_Pin_7;  // RS, RD
	EPM570_GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;
	EPM570_GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &EPM570_GPIO_InitStructure);
	
	EPM570_GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_5;		          // WR_SRAM_READY
	EPM570_GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;
	EPM570_GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_IPU;
	GPIO_Init(GPIOC, &EPM570_GPIO_InitStructure);
}


/*******************************************************************************
* Function Name  : EPM570_Registers_Init
* Description    : Инициализация регистров EPM570 и проверка записанных значений
* Input          : None
* Output         : None
* Return         : результат проверки - EPM570_success / EPM570_RD_WR_reg_err
*******************************************************************************/
EPM_ErrorStatus EPM570_Registers_Init(void)
{
	ResetEPM570_cntrlSignals();	  // Reset registers control signals
		
	/* Запись регистров, пишем начальные значения */
	Set_Decimation(0x00);           		          			// прореживание
	Set_numPoints(0x190);			         					// количество точек для записи в память
	
	EPM570_Write_Register(Write_Control, 0x00);	
	EPM570_Write_Register(cnfPin, 0x00);						// внутренние сигналы ПЛИС
	
	// внешние сигналы, регистр extPin_reg_0
	EPM570_Write_Register(extPin_reg_0, ADC_Standby_BothCH & Closed_IN_A & Closed_IN_B);
	
	// внешние сигналы, регистр extPin_reg_1
	EPM570_Write_Register(extPin_reg_1, OSC_Disable);
	EPM570_Write_Register(extPin_reg_1, BackLight_MAX);

	/* Read registers & verify values */
	if(Get_Decimation() != 0x00) return ERROR_D;
	if(Get_numPoints() != 0x190) return ERROR_N;
	
	return eSUCCESS;
}


/*******************************************************************************
* Function Name  : SAVE_RESTORE_Address
* Description    : Сохранение/восстановление адреса регистра EPM570
* Input          : SAVE_RESTORE - сохранить текущий адрес регистра EPM570 если 0,
				   или востановить ранее сохраненый если 1
* Output         : None
* Return         : None
*******************************************************************************/
void SAVE_RESTORE_Address(uint8_t SAVE_RESTORE)
{
   static uint8_t savedReg;
      
   if(SAVE_RESTORE == SAVE_REG)savedReg = EPM570_actual_Register;
   else	if(SAVE_RESTORE == RESTORE_REG)
   { 
		EPM570_actual_Register = savedReg;
		Write_Reg_Address(EPM570_actual_Register);
   }
}


/*******************************************************************************
* Function Name  : EPM570_Read_Keys
* Description    : Чтение состояния кнопок
* Input          : None
* Output         : None
* Return         : код нажатых кнопок
*******************************************************************************/
uint8_t EPM570_Read_Keys(void)
{
   return EPM570_Read_Register(IN_KEY);
}


/*******************************************************************************
* Function Name  : Set_Decimation
* Description    : Запись коэффициента прореживания
* Input          : value - коэффициент прореживания
* Output         : None
* Return         : None
*******************************************************************************/
void Set_Decimation(uint32_t value)
{
	EPM570_Write_Register(Decim_Low, (value & 0x000000FF));    
	EPM570_Write_Register(Decim_High0, ((value & 0x0000FF00) >> 8));
	EPM570_Write_Register(Decim_High1, ((value & 0x00FF0000) >> 16));
}


/*******************************************************************************
* Function Name  : Get_Decimation
* Description    : Чтение коэффициента прореживания
* Input          : None
* Output         : возвращает текущий коэффициент прореживания
* Return         : None
*******************************************************************************/
uint32_t Get_Decimation(void)
{
   return  EPM570_Read_Register(Decim_Low) |    
          (EPM570_Read_Register(Decim_High0) << 8) |
          (EPM570_Read_Register(Decim_High1) << 16);
}


/*******************************************************************************
* Function Name  : Set_numPoints
* Description    : Устанавливаем количество точек для записи в регистрах ПЛИС WIN_DATA_Low, WIN_DATA_High0, WIN_DATA_High1
* Input          : NumPoints - количество точек для записи
* Output         : None
* Return         : None
*******************************************************************************/
void Set_numPoints(uint32_t NumPoints)
{   
	uint32_t regVal = NumPoints;
	uint16_t tTrigPosition = (trigPosX_cursor.Position - leftLimit) - 1;
	uint8_t InterliveCoeff = (gOSC_MODE.Interleave == TRUE)? 2 : 1;

	/* расчет времени задержки для предыстории в автономном режиме
	 *    Необходимое количество точек до триггера = trigPosX_cursor.Position - (leftLimit - 1)
	 *    preTrigger_time(наносекунд)  = количество точек до триггера * коэффициент прореживания * 10нс
	 *    preTrigger_time(микросекунд) = preTrigger_time(нсек) / 1000
	 */

	/* если включена синхронизация */
	if(pnt_gOSC_MODE->oscSync != Sync_NONE)
	{
		if(HostMode == DISABLE)
		{
			regVal = (uint32_t)(( NumPoints - ( tTrigPosition / ((*SwipScale) * InterliveCoeff))) );		 // значение для записи в регистры ПЛИС
			preTrigger_time = (tTrigPosition * Get_Decimation() * 10 * ActiveMode->oscNumPoints_Ratio) / (1000 * (*SwipScale));
		}
		else
		{
			regVal = NumPoints - trigPosX_cursor.Position;
			preTrigger_time = (trigPosX_cursor.Position * Get_Decimation() * 10  * ActiveMode->oscNumPoints_Ratio) / 1000;
		}

//		if(gOSC_MODE.Interleave == TRUE) regVal /= 2;
		if(ActiveMode != &IntMIN_MAX) preTrigger_time = preTrigger_time * 2;
		if(preTrigger_time <= 1) preTrigger_time = 2;		// если время задержки 0, то установить в 1
	}
	else preTrigger_time = 1;	 // если предыстория выключена то время задержки в минимум, то есть в 1

//	if((SwipScale >= 2) && (HostMode == DISABLE)) regVal++;

//	regVal *= ActiveMode->oscNumPoints_Ratio;

	/* пишем регистры ПЛИС */
	EPM570_Write_Register(WIN_DATA_Low, (regVal & 0x000000FF));    
	EPM570_Write_Register(WIN_DATA_High0, ((regVal & 0x0000FF00) >> 8));
	EPM570_Write_Register(WIN_DATA_High1, ((regVal & 0x00FF0000) >> 16));
}


/*******************************************************************************
* Function Name  : Get_numPoints
* Description    : Читаем количество точек для записи в регистрах ПЛИС WIN_DATA_Low, WIN_DATA_High0, WIN_DATA_High1
* Input          : None
* Return         : текущее количество точек для записи
*******************************************************************************/
uint32_t Get_numPoints(void)
{   
	uint8_t InterliveCoeff = (gOSC_MODE.Interleave == TRUE)? 2 : 1;
	uint32_t reg_val =  (EPM570_Read_Register(WIN_DATA_Low)) |
						(EPM570_Read_Register(WIN_DATA_High0) << 8) |
						(EPM570_Read_Register(WIN_DATA_High1) << 16);

	if(pnt_gOSC_MODE->oscSync != Sync_NONE)	reg_val += (((trigPosX_cursor.Position - leftLimit) - 1) / ((*SwipScale) * InterliveCoeff));

	return reg_val;
}


/*******************************************************************************
* Function Name  : Write_SRAM
* Description    : Цикл записи в память
* Input          : None
* Output         : возвращает флаг wrSRAM_STOP или wrSRAM_DONE
* Return         : None
*******************************************************************************/
uint8_t Write_SRAM(void)
{   
	int32_t TimeoutCnt = TrigTimeout;
	FunctionalState TimeoutCounter_Status = ENABLE;

	wrSRAM = wrSRAM_START;         // Устанавливаем флаг сигнализирующий о записи в память
	NVIC_DisableIRQ(TIM2_IRQn);	   // запрет прерываний таймера опроса кнопок

	/* Interlive mode or not */
	if(gOSC_MODE.Interleave == TRUE){ EPM570_Write_Register(ExtPin_B, Enable_Interleave); GPIOB->BRR = GPIO_BRR_BR4; }
	else { EPM570_Write_Register(ExtPin_B, Disable_Interleave); GPIOB->BSRR = GPIO_BSRR_BS4; }

	/* Enable OA, ADC, Oscillator */
	Control_AD8129(ENABLE);							               // Если канал А и/или В запущен то включаем ОУ AD8921
   	EPM570_Write_Register(extPin_reg_1, OSC_Enable);	           // Enable Oscillator	KXO97
	EPM570_Write_Register(extPin_reg_0, ADC_AlignOperation);	   // Enable ADC
	delay_ms(1);	                                               // Ждем пока стабилизируется осциллятор и т.д.
	EPM570_Write_Register(Write_Control, WriteSRAM_Start);         // Устанавливаем сигнал старта записи внутри ПЛИС
	delay_us(preTrigger_time);					        		   // Ждем сколько надо для предыстории
	EPM570_Write_Register(Write_Control, Enable_Trigger);		   // Разрешаем срабатывание триггера внутри ПЛИС

	NVIC_EnableIRQ(TIM2_IRQn);	  // разрешение прерываний таймера опроса кнопок
	
	/* ждем пока не закончится запись, в случае прерывания записи возвращаем соответсвующий флаг */
	while(EPM570_Write_Ready == 0)                                // Wait write end	
	{
		if(wrSRAM == wrSRAM_STOP) return wrSRAM_STOP;	          // Возвращаем флаг прерванной записи в память
		else if((HostMode != DISABLE) && (IN_HostData[0] == 0x50)) return wrSRAM_STOP;
		else if((TimeoutCnt-- <= 0) && (TimeoutCounter_Status == ENABLE))
		{
			TimeoutCnt = TrigTimeout;
			IndicateTrigTimeout(SET);
			if(pnt_gOSC_MODE->oscSync == Sync_NONE) TimeoutCounter_Status = DISABLE;
			else
			{
				if(pnt_gOSC_MODE->oscSync != Sync_SINGL) EPM570_Sync(OFF);
			}
		}
	}	
	IndicateTrigTimeout(RESET);

	EPM570_Sync(pnt_gOSC_MODE->oscSync);

	NVIC_DisableIRQ(TIM2_IRQn);	  // запрет прерываний таймера опроса кнопок

	/* если дошли сюда значит запись закончилась успешно.
	   Выключаем ОУ, АЦП, Генератор и сбрасываем сигналы старта записи внутри ПЛИС */
	Control_AD8129(DISABLE);						                           // Выключаем оба ОУ AD8921
	EPM570_Write_Register(Write_Control, WriteSRAM_Stop & Disable_Trigger);    // Clear Start_Write and Disable trigger
	EPM570_Write_Register(extPin_reg_1, OSC_Disable);                          // Disable Oscillator KXO97
	EPM570_Write_Register(extPin_reg_0, ADC_Standby_BothCH);                   // Disable ADC
	
	NVIC_EnableIRQ(TIM2_IRQn);	  // разрешение прерываний таймера опроса кнопок
	
	return wrSRAM = wrSRAM_DONE;  // Возвращаем флаг успешной записи в память
}


/*******************************************************************************
* Function Name  : SRAM_ReadDirection
* Description    :
* Input          : None
* Return         : None
*******************************************************************************/
void SRAM_ReadDirection(uint8_t UP_DOWN)
{
	uint8_t tUP_DOWN = (UP_DOWN == SRAM_READ_UP)? (uint8_t)(EPM570_cnfPin_DATA | 0x10) : (uint8_t)(EPM570_cnfPin_DATA & ~0x10);

	EPM570_Write_Register(cnfPin, tUP_DOWN);
}


/*******************************************************************************
* Function Name  : Prepare_SRAM_Read
* Description    : Подготовка/завершение чтения данных из памяти
* Input          : None
* Return         : None
*******************************************************************************/
void SRAM_ReadState(FunctionalState NewState)
{
	if(NewState == ENABLE)
	{
		/* Read Counter Enable */
		EPM570_Write_Register(cnfPin, EPM570_cnfPin_DATA | 0x08);

		/* Switch in EPM570 SRAM data bus to MCU bus */
		EPM570_Read_Register(SRAM_DATA);

		EPM570_RD_set;	/* Read enable */
//		EPM570_RS_set; EPM570_RS_clr; EPM570_RS_set;
	}
	else
	{
		EPM570_RS_clr; EPM570_RS_clr;
		EPM570_RD_clr;	/* Read disable */

		/* Read Counter Disable */
		EPM570_Write_Register(cnfPin, EPM570_cnfPin_DATA & ~0x08);
	}
}


/*******************************************************************************
* Function Name  : SRAM_ReadDirection
* Description    :
* Input          : None
* Return         : None
*******************************************************************************/
static void SRAM_Shift(int32_t cnt, uint8_t UP_DOWN)
{
	int32_t sCNT = cnt;
	SRAM_ReadDirection(UP_DOWN);

	if(cnt > 0)
	{
		if(sCNT == 0x7fffffff)
		{
			EPM570_Write_Register(ExtPin_B, Enable_sResetReadCounter);
			EPM570_RS_set; // EPM570_RS_set;
			EPM570_RS_clr; // EPM570_RS_clr;
			EPM570_Write_Register(ExtPin_B, Disable_sResetReadCounter);
		}
		else
		{
			SRAM_ReadState(ENABLE);
			do
			{
				EPM570_RS_set; EPM570_RS_set;
				EPM570_RS_clr; EPM570_RS_clr;

				if((ActiveMode != &IntMIN_MAX) && (pnt_gOSC_MODE->oscSweep != 0))
				{
					EPM570_RS_set; EPM570_RS_set;
					EPM570_RS_clr; EPM570_RS_clr;
				}
			}
			while(sCNT-- > 0);
			SRAM_ReadState(DISABLE);
		}
	}
}


/*******************************************************************************
* Function Name  : Read_SRAM
* Description    : Цикл чтения из памяти
* Input          : None
* Return         : None
*******************************************************************************/
void Read_SRAM(void)
{  
	int32_t SamplesWindow, VisibleSamples;
	int32_t sCNT, StarPointRead = 0;
	int8_t s;
	uint8_t InterliveCoeff = (gOSC_MODE.Interleave == TRUE)? 2 : 1;

	SamplesWindow = Get_numPoints() * ActiveMode->oscNumPoints_Ratio * InterliveCoeff;
	VisibleSamples = (((rightLimit - leftLimit) - 1) * ActiveMode->oscNumPoints_Ratio) / (*SwipScale);
	StarPointRead = (SamplesWindow - ((gOSC_MODE.WindowPosition + 1) * VisibleSamples)) / InterliveCoeff;

	NVIC_DisableIRQ(TIM2_IRQn);	  		// запрет прерываний таймера опроса кнопок

	/* Reset read state */
	SRAM_ReadState(DISABLE);

	/* Shift to read data */
	if(ActiveMode == &IntMIN_MAX) s = 4;
	else if(gOSC_MODE.oscSweep == 0) s = 3;
	else s = 2;
	SRAM_Shift(StarPointRead - s, SRAM_READ_DOWN);

	/* Enable read state for capture data */
	SRAM_ReadState(ENABLE);
	
	if(ActiveMode == &IntMIN_MAX)
	{
		EPM570_RS_set; EPM570_RS_set;
		EPM570_RS_clr; EPM570_RS_clr;
	}

	/* Read Data */
	sCNT = VisibleSamples;
	if(gOSC_MODE.Interleave == TRUE)
	{
		do
		{
			/* Read data CH A */
			EPM570_RS_set; EPM570_RS_set;
			INFO_A.DATA[sCNT] = ~((GPIOB->IDR >> 8) - 127);
			sCNT--;

			EPM570_RS_clr; EPM570_RS_clr;
			INFO_A.DATA[sCNT] = ~((GPIOB->IDR >> 8) - 127);
			INFO_A.DATA[sCNT] -= 2;
			sCNT--;

		}
		while(sCNT > 0);
	}
	else
	{
		do
		{
			/* Read data CH A */
			EPM570_RS_set; EPM570_RS_set;
			INFO_A.DATA[sCNT] = ~((GPIOB->IDR >> 8) - 127);

			/* Read data CH B */
			EPM570_RS_clr; EPM570_RS_clr;
			INFO_B.DATA[sCNT] = ~((GPIOB->IDR >> 8) - 127);

			if((ActiveMode != &IntMIN_MAX) && (pnt_gOSC_MODE->oscSweep != 0))
			{
				EPM570_RS_set; EPM570_RS_set;
				EPM570_RS_clr; EPM570_RS_clr;
			}

		}
		while(sCNT-- > 0);
	}

	/* Reset read state */
	SRAM_ReadState(DISABLE);

	/* Roll Back */
	SRAM_Shift(0x7fffffff, SRAM_READ_UP);

	/* Enable interupts for read buttons */
	NVIC_EnableIRQ(TIM2_IRQn);	   // разрешение прерываний таймера опроса кнопок
}


/*******************************************************************************
* Function Name  : Set_Trigger
* Description    : устанавливаем режим триггера/синхронизации в регистрах ПЛИС Trigger_level_A, Trigger_level_B, cnfPin
*				 :
* Input          : None
* Return         : None
*******************************************************************************/
void Set_Trigger(uint8_t NewSyncMode)
{    
	int16_t *pCursor; 
	int16_t trgLevel_H = 0, trgLevel_L = 0;
	uint8_t Height_Pos, Low_Pos;
	
	uint8_t sync_IN_OUT_WIN_flag = 0;
	
	/* if sync is a analog */
	if(pnt_gOSC_MODE->SyncSourse != CHANNEL_DIGIT)
	{
		EPM570_Write_Register(cnfPin, EPM570_cnfPin_DATA & ~LA_OR_OSC_TRIGG);

		/* select current sync sourse mode */
		if(pnt_gOSC_MODE->SyncSourse == CHANNEL_A)
		{
			pCursor = (int16_t*)&INFO_A.Position;
			EPM570_Write_Register(cnfPin, EPM570_cnfPin_DATA & ~Sync_channel_sel);
		}
		else if(pnt_gOSC_MODE->SyncSourse == CHANNEL_B)
		{ 
			pCursor = (int16_t*)&INFO_B.Position;
			EPM570_Write_Register(cnfPin, EPM570_cnfPin_DATA | Sync_channel_sel);
		}
		else return;
	
		/* Host mode */
		if(HostMode != DISABLE)
		{
			Height_Pos = Height_Y_cursor.Position;
			Low_Pos = Low_Y_cursor.Position;
		}
		else	/* Autonome mode */
		{
			Height_Pos = (uint8_t)(~(Height_Y_cursor.Position - (*pCursor)) + 128);
			Low_Pos = (uint8_t)(~(Low_Y_cursor.Position - (*pCursor)) + 128);
		}

		/* select current sync mode */
		if(NewSyncMode == Sync_Rise)
		{
			trgLevel_L = Height_Pos;
			trgLevel_H = 255;
			sync_IN_OUT_WIN_flag = 0;
		}
		else if(NewSyncMode == Sync_Fall)
		{
			trgLevel_H = Height_Pos;
			trgLevel_L = 0;         
			sync_IN_OUT_WIN_flag = 0;
		}
		else
		{
			trgLevel_H = Low_Pos;
			trgLevel_L = Height_Pos;

			if(NewSyncMode == Sync_IN_WIN) sync_IN_OUT_WIN_flag = 1;
			else sync_IN_OUT_WIN_flag = 0;
		}
	   
		if(sync_IN_OUT_WIN_flag == 0) EPM570_Write_Register(cnfPin, EPM570_cnfPin_DATA & ~Sync_IN_OUT_WIN);
		else EPM570_Write_Register(cnfPin, EPM570_cnfPin_DATA | Sync_IN_OUT_WIN); 
		
	}
	else if (pnt_gOSC_MODE->SyncSourse == CHANNEL_DIGIT)   /* else if sync is a digital */
	{
		/* select current sync mode */
		switch(NewSyncMode)
		{
			case Sync_Condition:
			{
				trgLevel_H = ConditionState_Sync_Var;
				trgLevel_L = 0;
				EPM570_Write_Register(cnfPin, EPM570_cnfPin_DATA | AND_OR_LA_TRIGG);
			}
			break;
			case Sync_Different:
			{
				if((DifferentState_Sync_Rise == ENABLE) && (DifferentState_Sync_Fall == DISABLE))
				{
					trgLevel_H = DifferentState_Sync_Var;
					trgLevel_L = DifferentState_Sync_Var;
					EPM570_Write_Register(cnfPin, EPM570_cnfPin_DATA | AND_OR_LA_TRIGG);
				}
				else if((DifferentState_Sync_Rise == DISABLE) && (DifferentState_Sync_Fall == ENABLE))
				{
					trgLevel_H = DifferentState_Sync_Var;
					trgLevel_L = 0xFF & ~(1 << DifferentState_Sync_Var);
					EPM570_Write_Register(cnfPin, EPM570_cnfPin_DATA | AND_OR_LA_TRIGG);
				}
				else if(((DifferentState_Sync_Rise == DISABLE) && (DifferentState_Sync_Fall == DISABLE)) ||
						((DifferentState_Sync_Rise == ENABLE) && (DifferentState_Sync_Fall == ENABLE)))
				{
					return;
				}
			}
			break;
			case Sync_C_or_D:
			{
				trgLevel_H = ~ConditionState_Sync_Var;
				trgLevel_L = DifferentState_Sync_Var;
				EPM570_Write_Register(cnfPin, EPM570_cnfPin_DATA & ~AND_OR_LA_TRIGG);
			}
			break;
			case Sync_C_and_D:
			{
				trgLevel_H = ~ConditionState_Sync_Var;
				trgLevel_L = DifferentState_Sync_Var;
				EPM570_Write_Register(cnfPin, EPM570_cnfPin_DATA | AND_OR_LA_TRIGG);
			}
			break;
		}

		EPM570_Write_Register(cnfPin, EPM570_cnfPin_DATA | LA_OR_OSC_TRIGG);
	}
	else return;   // else ERROR, return


	/* Write Trigg levels registers */
	EPM570_Write_Register(Trigger_UP, trgLevel_H);
	EPM570_Write_Register(Trigger_Down, trgLevel_L);

	EPM570_cnfPin_DATA = EPM570_Read_Register(cnfPin);
}


/*******************************************************************************
* Function Name  : EPM570_Sync
* Description    : включить/выключить синхронизацию
* Input          : uint8_t NewState - ON/OFF включить/выключить соответсвенно
* Output         : None
* Return         : None
*******************************************************************************/
void EPM570_Sync(uint8_t NewState)
{
   if(NewState == ON)
   {
      EPM570_Write_Register(cnfPin, EPM570_cnfPin_DATA | Sync_ON);
      pnt_gOSC_MODE->oscPreHistory = 1;
   }
   else if(NewState == OFF)
   {
	  EPM570_Write_Register(cnfPin, EPM570_cnfPin_DATA & ~Sync_ON);
	  pnt_gOSC_MODE->oscPreHistory = 0;
   }
   else return;
   	     
   /* Update sync registers */
   Set_Trigger(pnt_gOSC_MODE->AnalogSyncType);
}


/*******************************************************************************
* Function Name  : Set_Input
* Description    : установить открытый или закрытый вход
* Input          : gChannel_MODE *Channel
* Output         : None
* Return         : None
*******************************************************************************/
void Set_Input(uint8_t Channel, Channel_AC_DC_TypeDef AC_DC_NewState)
{
	uint8_t Open_IN = Open_IN_A, Closed_IN = Closed_IN_A;
	
	if(Channel == CHANNEL_B){ Open_IN = Open_IN_B; Closed_IN = Closed_IN_B; }
	
	if(AC_DC_NewState == RUN_AC) EPM570_Write_Register(extPin_reg_0, EPM570_extPin_0_DATA & Closed_IN);
	else if(AC_DC_NewState == RUN_DC) EPM570_Write_Register(extPin_reg_0, EPM570_extPin_0_DATA | Open_IN);
}


/*******************************************************************************
* Function Name  : EPM570_ChangeBackLight
* Description    : Изменить яркость подсветки
* Input          : bckLight_Val - BackLight_MAX / BackLight_MIN
* Output         : None
* Return         : None
*******************************************************************************/
void EPM570_ChangeBackLight(uint8_t bckLight_Val)
{
	if(bckLight_Val == 1)EPM570_Write_Register(extPin_reg_1, BackLight_MAX);
	else if(bckLight_Val == 0)EPM570_Write_Register(extPin_reg_1, BackLight_MIN);
}


/*******************************************************************************
* Function Name  : Get_EPM570_Register_Operate_Status
* Description    : чтение статуса операций над регистрами ПЛИС, если 1 то в текущий момент идет запись или чтение
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
uint8_t Get_EPM570_Register_Operate_Status(void)
{
	return EPM570_Register_Process;
}




