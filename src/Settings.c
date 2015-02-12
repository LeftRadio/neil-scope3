/**
  ******************************************************************************
  * @file	 	Settings.c
  * @author  	Left Radio
  * @version 	1.5.6
  * @date
  * @brief		NeilScope3 Settings sourse
  ******************************************************************************
**/

/* Includes ------------------------------------------------------------------*/
#include "Settings.h"
#include "Processing_and_output.h"
#include "EPM570.h"
#include "eeprom_WR_RD.h"
#include "gInterface_MENU.h"
#include "Analog.h"
#include "User_Interface.h"
#include "Quick_Menu.h"
#include "systick.h"
#include "inscriptions_Buttons.h"
#include "Measurments.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define numVar			77

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint8_t DataMassive[numVar] = {0};
FunctionalState BeepState = 0;

const uint32_t sweep_coff[] =
{
	1, 2, 4, 8, 20, 40, 80, 200, 400, 800, 2000, 4000,
	8000, 20000, 40000, 80000, 200000, 400000, 800000, 2000000, 4000000
};


/* Private function prototypes -----------------------------------------------*/
uint8_t Save_Pref_CheckSum(void);
uint8_t CheckSum(uint8_t *pData, uint8_t numValues);


/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : Init_DATA_Massivs
* Description    : инициализация массива начальными значениями 
* Input          : None
* Return         : None
*******************************************************************************/
static void Init_DATA_Massivs(void)
{
   memset(&INFO_A.visDATA[0], 120, 800);
   memset(&INFO_B.visDATA[0], 120, 800);
}


/*******************************************************************************
* Function Name  : LoadPreference
* Description    : чтение сохранненых настроек
* Input          : None
* Return         : None
*******************************************************************************/
void LoadPreference(void)
{  
	uint8_t cntrl_num = 0;
	uint8_t ControlCheckSum = 0, ReadCheckSum = 0;
	uint8_t *INFO_Zero;

	err_I2C_flag = 0;

	__enable_irq();					// разрешить все прерывания

	/* изменяем шрифт на timesNewRoman12 */
    LCD_SetFont(&timesNewRoman12ptFontInfo);

	LCD_SetTextColor(0xF100);   
	LCD_PutStrig(20, 20, 0, "For skip load settings push DOWN");
	delay_ms(1000);
	if(ButtonsCode == DOWN)
	{
		LCD_SetTextColor(Black);   
		LCD_PutStrig(20, 20, 0, "For skip load settings push DOWN");

		LCD_SetTextColor(0xF100);   
		LCD_PutStrig(20, 20, 0, "Load Preference SKIPED");
		delay_ms(300);
	}
	else
	{
		/* чтение всех значений и проверка контрольной суммы, проверка контрольного числа */
		EEPROM_Read(&DataMassive[0], 2, numVar);
		EEPROM_Read(&cntrl_num, Saved_Settings, 1);
		EEPROM_Read(&ReadCheckSum, numVar + 2, 1);

		ControlCheckSum = CheckSum(&DataMassive[0], numVar);
		ControlCheckSum += cntrl_num;

		if((cntrl_num == (uint8_t)73) && (ControlCheckSum == ReadCheckSum))
		{
			/* чтение значений ШИМ, коррекция нуля */
			INFO_Zero = (uint8_t*)(&INFO_A.AD_Type.Analog.Zero_PWM_values[0]);
			memcpy(INFO_Zero, &DataMassive[0], 24);

			INFO_Zero = (uint8_t*)(&INFO_B.AD_Type.Analog.Zero_PWM_values[0]);
			memcpy(INFO_Zero, &DataMassive[24], 24);

			/* чтение смещения */
			INFO_A.Position = DataMassive[48];
			INFO_B.Position = DataMassive[49];

			/* вид интерполяции */
			ActiveMode = (InterpolationMode_TypeDef*)InterpModes[DataMassive[50]];	//
			if(ActiveMode == &FFT_MODE)
			{
				pnt_gOSC_MODE->autoMeasurments = ON;
				mModeActive = (MeasMode_TypeDef*)&MeasurmentsMode[2];
			}

			/* чтение аналоговых делителей */
			INFO_A.AD_Type.Analog.Div = DataMassive[51];
			INFO_B.AD_Type.Analog.Div = DataMassive[52];

			/* чтение коэффициента развертки */
			SweepIndex = DataMassive[53];
			pnt_gOSC_MODE->oscSweep = sweep_coff[SweepIndex] - 1;

			/* чтение значения времени автоотключения */
			if(DataMassive[54] != 255)
			{
				pnt_gOSC_MODE->OFF_Struct.State = ENABLE;
				pnt_gOSC_MODE->OFF_Struct.ResetTime = RTC_GetCounter();
				pnt_gOSC_MODE->OFF_Struct.Work_Minutes = DataMassive[54];

				sprintf (&AutoPowerOFF_btnTxt[10], "%d", pnt_gOSC_MODE->OFF_Struct.Work_Minutes);
				AutoPowerOFF.Text = AutoPowerOFF_btnTxt;
			}

			/* чтение состояния автоделителя */
			if(DataMassive[55] == BOTH_CHANNEL){ Set_AutoDivider_State(CHANNEL_A, ENABLE); Set_AutoDivider_State(CHANNEL_B, ENABLE); }
			else if(DataMassive[55] == CHANNEL_A) Set_AutoDivider_State(CHANNEL_A, ENABLE);
			else if(DataMassive[55] == CHANNEL_B) Set_AutoDivider_State(CHANNEL_B, ENABLE);

			/* чтение состояния синхронизации */
			pnt_gOSC_MODE->oscSync = DataMassive[56];
			gInterfaceMenu.Buttons[4]->Text = (char*)&sweepMODE_text[pnt_gOSC_MODE->oscSync];

			/* Источник и режим синхронизации */
			pnt_gOSC_MODE->SyncSourse = DataMassive[57];
			pnt_gOSC_MODE->AnalogSyncType = DataMassive[58];

			Sync_Sourse_Texts();
			TrigMenu.Buttons[4]->Text = TriggShowInfo.triggType_Name[pnt_gOSC_MODE->AnalogSyncType];
			TrigMenu.Buttons[5]->Text = (char*)&Trigg_Sourse_Sync_Text[pnt_gOSC_MODE->SyncSourse][0];

			/* Уровени синхронизации */
			Height_Y_cursor.Position = DataMassive[59];
			Low_Y_cursor.Position = DataMassive[60];
			trigPosX_cursor.Position = DataMassive[61];
			TriggShowInfo.Status = (FunctionalState)DataMassive[62];
			Set_Trigger(pnt_gOSC_MODE->AnalogSyncType);							/* обновляем регистры синхронизации ПЛИС */

			/* Read colors */
			indxColorA = DataMassive[63];
			indxColorB = DataMassive[64];
			indxColorButtons = DataMassive[65];
			indxTextColorA = DataMassive[66];
			indxTextColorB = DataMassive[67];
			indxColorGrid = DataMassive[68];

			/* Read Beeper state */
			BeepState = DataMassive[69];
			if(BeepState != DISABLE) BeepEN.Text = "Beep ON";
			else BeepEN.Text = "Beep OFF";

			/* чтение параметров открытый/закрытый вход каналов */
			INFO_A.Mode.AC_DC = DataMassive[70];	setActiveButton(gInterfaceMenu.Buttons[8]);	ON_OFF_CHANNEL_A();
			INFO_B.Mode.AC_DC = DataMassive[71];	setActiveButton(gInterfaceMenu.Buttons[9]); ON_OFF_CHANNEL_B();

			/* чтение параметрa отображения частоты FFT и коррекции интерлива */
			gShowFFTFreq = (Boolean)DataMassive[72];
			InterliveCorrectionCoeff = DataMassive[73];

			/* Read semples num points */
			gOSC_MODE.oscNumPoints = 0;
			gOSC_MODE.oscNumPoints = (uint32_t)DataMassive[74] << 16;
			gOSC_MODE.oscNumPoints |= (uint32_t)DataMassive[75] << 8;
			gOSC_MODE.oscNumPoints |= (uint32_t)DataMassive[76];
			if(gOSC_MODE.oscNumPoints == 0) gOSC_MODE.oscNumPoints = 388;

			/* --------------------------------------------------------------------------------------------------- */
			/* установка ШИМ в соответсвии с прочитанными значениями */
			*(INFO_A.AD_Type.Analog.corrZ) = INFO_A.AD_Type.Analog.Zero_PWM_values[INFO_A.AD_Type.Analog.Div];
			*(INFO_B.AD_Type.Analog.corrZ) = INFO_B.AD_Type.Analog.Zero_PWM_values[INFO_B.AD_Type.Analog.Div];

			/* смена делителей и изменение надписей на кнопках на соответсвующие делителям */
			gInterfaceMenu.Buttons[1]->Text = Change_AnalogDivider(CHANNEL_A, INFO_A.AD_Type.Analog.Div);		// &btnVDIV_A
			gInterfaceMenu.Buttons[2]->Text = Change_AnalogDivider(CHANNEL_B, INFO_B.AD_Type.Analog.Div);		// &btnVDIV_B

			/* изменение надписи на кнопке btnSWEEP */
			gInterfaceMenu.Buttons[3]->Text = (char *)&sweep_text[SweepIndex];		// &btnSWEEP

			/* обновляем регистры ПЛИС - Decimation */
			Set_Decimation(pnt_gOSC_MODE->oscSweep);

			/* установка интерполяции и обновление надписи на кнопке */
			changeInterpolation(ActiveMode);
			Interpolation.Text = (char*)ActiveMode->Text;		// &Interpolation
		}
		else
		{
			LCD_SetTextColor(Black);
			LCD_PutStrig(20, 20, 0, "For skip load settings push DOWN");

			LCD_SetTextColor(0xF100);
			LCD_PutStrig(20, 20, 0, "Saved Data in EEPROM is NOT Correct!");
			delay_ms(1000);
		}
	}

	/* Set channels colors */
	INFO_A.Color = Color_ChangeBrightness(M256_Colors[indxColorA*2], 2);;
	INFO_B.Color = Color_ChangeBrightness(M256_Colors[indxColorB*2], 2);;

	/* Set interface buttons colors */
	mSet_AllButtons_Color(M256_Colors[indxColorButtons*2]);

	/* Set channels buttons colors */
	gInterfaceMenu.Buttons[1]->Color = Color_ChangeBrightness(M256_Colors[indxColorA*2], -24);
	gInterfaceMenu.Buttons[8]->Color = Color_ChangeBrightness(M256_Colors[indxColorA*2], -24);
	gInterfaceMenu.Buttons[2]->Color = Color_ChangeBrightness(M256_Colors[indxColorB*2], -24);
	gInterfaceMenu.Buttons[9]->Color = Color_ChangeBrightness(M256_Colors[indxColorB*2], -24);

	/* Set channels fonts colors */
	gInterfaceMenu.Buttons[1]->Active_FontColor = grayScalle[indxTextColorA*2];
	gInterfaceMenu.Buttons[8]->Active_FontColor = grayScalle[indxTextColorA*2];
	gInterfaceMenu.Buttons[2]->Active_FontColor = grayScalle[indxTextColorB*2];
	gInterfaceMenu.Buttons[9]->Active_FontColor = grayScalle[indxTextColorB*2];

	/* Set Grid color */
	activeAreaGrid.Color = grayScalle[indxColorGrid*2];


	Init_DATA_Massivs();  // инициализируем массивы данных для первоначального вывода на экран

	/* обновляем регистры ПЛИС */
	Set_numPoints(pnt_gOSC_MODE->oscNumPoints);
}


/*******************************************************************************
* Function Name  : SavePreference
* Description    : сохраннение настроек
* Input          : None
* Return         : None
*******************************************************************************/
void SavePreference(void)
{
	const uint8_t verify_const = 73;
	uint8_t check_Sum = 0;
	uint8_t *INFO_Zero;

	err_I2C_flag = 0;

	/* сохраняем значения ШИМ, коррекция нуля */
	INFO_Zero = (uint8_t*)(&INFO_A.AD_Type.Analog.Zero_PWM_values[0]);
	memcpy(&DataMassive[0], INFO_Zero, 24);

	INFO_Zero = (uint8_t*)(&INFO_B.AD_Type.Analog.Zero_PWM_values[0]);
	memcpy(&DataMassive[24], INFO_Zero, 24);
		
	/* сохраняем смещение */
	DataMassive[48] = INFO_A.Position;
	DataMassive[49] = INFO_B.Position;

	/* сохраняем вид интерполяции */
	DataMassive[50] = ActiveMode->Indx;

	/* сохраняем аналоговые делители */
	DataMassive[51] = INFO_A.AD_Type.Analog.Div;
	DataMassive[52] = INFO_B.AD_Type.Analog.Div;

	/* сохраняем коэффициент развертки */
	DataMassive[53] = SweepIndex;

	/* значения времени автоотключения */
	if(pnt_gOSC_MODE->OFF_Struct.State == ENABLE) DataMassive[54] = pnt_gOSC_MODE->OFF_Struct.Work_Minutes;
	else DataMassive[54] = 255;

	/* состояниe автоделителя */
	if((Get_AutoDivider_State(CHANNEL_A) == ENABLE) && (Get_AutoDivider_State(CHANNEL_B) == ENABLE)) DataMassive[55] = BOTH_CHANNEL;
	else if(Get_AutoDivider_State(CHANNEL_A) == ENABLE) DataMassive[55] = CHANNEL_A;
	else if(Get_AutoDivider_State(CHANNEL_B) == ENABLE) DataMassive[55] = CHANNEL_B;
	else DataMassive[55] = 255;

	/* состояния синхронизации */
	DataMassive[56] = pnt_gOSC_MODE->oscSync;
	DataMassive[57] = pnt_gOSC_MODE->SyncSourse;
	DataMassive[58] = pnt_gOSC_MODE->AnalogSyncType;

	DataMassive[59] = Height_Y_cursor.Position;
	DataMassive[60] = Low_Y_cursor.Position;
	DataMassive[61] = trigPosX_cursor.Position;
	DataMassive[62] = (uint8_t)TriggShowInfo.Status;

	/* сохраняем цветa */
	DataMassive[63] = indxColorA;
	DataMassive[64] = indxColorB;
	DataMassive[65] = indxColorButtons;
	DataMassive[66] = indxTextColorA;
	DataMassive[67] = indxTextColorB;
	DataMassive[68] = indxColorGrid;

	/* сохраняем включен ли бипер */
	DataMassive[69]	= BeepState;

	/* сохраняем параметр открытый/закрытый вход каналов */
	DataMassive[70] = INFO_A.Mode.AC_DC;
	DataMassive[71] = INFO_B.Mode.AC_DC;

	/* сохраняем параметр отображения частоты FFT и коррекции интерлива */
	DataMassive[72] = (uint8_t)gShowFFTFreq;
	DataMassive[73] = InterliveCorrectionCoeff;

	/* Num samples */
	DataMassive[74] = (uint8_t)(gOSC_MODE.oscNumPoints >> 16);
	DataMassive[75] = (uint8_t)(gOSC_MODE.oscNumPoints >> 8);
	DataMassive[76] = (uint8_t)(gOSC_MODE.oscNumPoints & 0x000000FF);

	/* Считаем контрольную сумму */
	check_Sum = CheckSum(&DataMassive[0], numVar);
	check_Sum += verify_const;

	EEPROM_Write(&DataMassive[0], 2, numVar); 					// запись
	EEPROM_Write((uint8_t*)&verify_const, Saved_Settings, 1);	// запись в 1 ячейку контрольного числа
	EEPROM_Write(&check_Sum, numVar + 2, 1);					// запись контрольной суммы

	if(err_I2C_flag != 0){ LCD_SetTextColor(0xF800); Show_Message("ERROR when save Preference"); }
	else{ LCD_SetTextColor(0xFFFF);	Show_Message("Preference SUCCESSFUL Saved"); }
}


/*******************************************************************************
* Function Name  : CheckSum	8 bit
* Description    : 
* Input          : None
* Return         : None
*******************************************************************************/
uint8_t CheckSum(uint8_t *pData, uint8_t numValues)
{
	uint8_t cSum = 0;

	while(numValues--){ cSum = cSum + *(pData + numValues); }
	
	return cSum;
}


