/**
  ******************************************************************************
  * @file	 	Host.c
  * @author  	Left Radio
  * @version 	1.5.6
  * @date
  * @brief		NeilScope3 Host sourse
  ******************************************************************************
**/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "defines.h"
#include "Host.h"
#include "main.h"
#include "init.h"
#include "systick.h"
#include "Settings.h"
#include "User_Interface.h"
#include "Analog.h"
#include "Processing_and_output.h"
#include "EPM570.h"
#include "gInterface_MENU.h"
#include "IQueue.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Host commands defines */
#define ANALOG_IN_CMD					0x10
#define ANALOG_DIV_CMD					0x11
#define CALIBRATE_ZERO_CMD				0x12

#define SYNC_MODE_CMD					0x14
#define TRIG_SOURSE_CMD					0x15
#define TRIG_MODE_CMD					0x16
#define TRIG_LEVEL_UP_CMD				0x17
#define TRIG_LEVEL_DOWN_CMD				0x18
#define TRIG_CURSOR_X_CMD				0x19

#define DECIMATION_CMD					0x25
#define DATA_COLLECTION_MODE_CMD		0x27			// Standart or MIN/MAX mode
#define DATA_REQUEST_CMD				0x30

#define BATT_V_CMD						0xA0
#define SAVE_ALL_EEPROM_CMD				0xEE

#define HOST_MODE_CMD					0x81
#define BOOTLOADER_CMD					0xB0
#define FIRMWARE_VERSION_CMD			0x00

//#define TERMINATE_CMD					0x50
#define EXIT_HOST_MODE_CMD				0xFC

/* Error type and codes defines */
#define ERROR_RESPOND					0x7F
#define ErrorCRC						0x01
#define ErrorData						0x02
#define ErrorBusy						0x03

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static const uint8_t HostModeRequest[4] = {HOST_MODE_CMD, 0x02, 0x86, 0x93};
static const uint8_t ExitHostModeRequest[4] = {EXIT_HOST_MODE_CMD, 0x02, 0x86, 0x93};

uint8_t CommandData[CMD_MAX_SIZE];
uint8_t OUT_HostData[OUT_CMD_MAX_SIZE];
uint8_t CMD_Length = 0;

FunctionalState HostMode = DISABLE;
uint8_t ResponseCode;
__IO HostRequest_TypeDef gHostRequest = { DISABLE, NO_Request, 0 };
Channel_ID_TypeDef CalibrateChannel;

#ifdef __HOST_DEBUG__
	char OLD_TransmitString[40] = {0};
	char TransmitString[40] = {0};
#endif

/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void Switch_To_HostMode(void);
void Decoding_Command(void);
uint8_t CRC8_Buff(uint8_t *pBuff, uint16_t NumBytes);
uint8_t CRC8(uint8_t Byte, uint8_t crc);

static __inline void Host_RequestProcessing(void);

/* Private Functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : Recive_Host_Data
* Description    :
* Input          : None
* Return         : None
*******************************************************************************/
void Recive_Host_Data(uint8_t IQueueIndex)
{
	uint8_t i;
	uint8_t Control_CRC = 0;
	IQueue_TypeDef *IQueue = Host_GetIQueue(IQueueIndex);

	CMD_Length = IQueue->CMD_Length;

#ifdef __HOST_DEBUG__
	char OLD_RecivedString[40] = {0};
	char RecivedString[40] = {0};

	for(i = 0; i < CMD_Length + 2; i++)
	{
		sprintf(&RecivedString[i*5], "0x%02X", CommandData[i]);
		strcat(RecivedString, " ");
	}
#endif

	Control_CRC = CRC8_Buff((uint8_t*)IQueue->Data, CMD_Length);
	if(Control_CRC == 0)
	{
		for(i = 0; i < CMD_Length - 2; i++){
			CommandData[i] = IQueue->Data[i + 1];
		}

		if(HostMode == DISABLE)
		{
			if(memcmp(CommandData, HostModeRequest, 4) == 0) Switch_To_HostMode();
			else if((CommandData[0] == BOOTLOADER_CMD) && (CommandData[2] == 0x0B))
			{
				gHostRequest.RequestDataLen = 0;
				gHostRequest.State = ENABLE;
				gHostRequest.Request = Bootoader_Request;
			}
			else return;
		}
		else if((gHostRequest.State == ENABLE) && (IQueue_CommandStatus != TRUE))
		{
			ResponseCode = ErrorBusy;
			Transmit_To_Host(ERROR_RESPOND, &ResponseCode, 1);
		}
		else Decoding_Command();
	}
	else
	{
		if(HostMode == ENABLE)
		{
			ResponseCode = ErrorCRC;
			Transmit_To_Host(ERROR_RESPOND, &ResponseCode, 1);
		}
		else return;
	}

//#ifdef __VAR_DEBUG__
//		char cnt[4] = {' ', ' ', dCommandCounter + 48, 0};
//		LCD_SetTextColor(White);
//		LCD_ClearArea(10, 70, 20, 80, globalBackColor);
//		LCD_PutStrig(10, 70, 0, cnt);
//#endif

#ifdef __HOST_DEBUG__
	if(HostMode == ENABLE)
	{
		__disable_irq();

		LCD_SetTextColor(globalBackColor);
		LCD_PutStrig(115, 150, 0, OLD_RecivedString);
		LCD_PutStrig(5, 110, 0, OLD_TransmitString);

		LCD_SetTextColor(White);
		LCD_PutStrig(115, 150, 0, RecivedString);
		LCD_PutStrig(5, 110, 0, TransmitString);

		if(gHostRequest.State == ENABLE) LCD_PutColorStrig(150, 90, 0, "ENABLE", LighGreen);
		else if(gHostRequest.State == DISABLE) LCD_PutColorStrig(150, 90, 0, "DISABLE", Red);

		memcpy(OLD_RecivedString, RecivedString, 40);
		memcpy(OLD_TransmitString, TransmitString, 40);

		__enable_irq();
	}
#endif

	/* Request Processing */
	if(gHostRequest.State == ENABLE)
	{
		Host_RequestProcessing();
	}
}


/*******************************************************************************
* Function Name  : Transmit_To_Host
* Description    : Передача данных в хост
* Input          : uint8_t *pData - указатель  на массив с данными
* 				   uint8_t DataLen - количество пересылаемых байт
* Return         : None
*******************************************************************************/
void Transmit_To_Host(uint8_t Respond_CMD, uint8_t *pData, uint8_t DataLen)
{
	uint8_t ResponseHeader[3] = {0x5B, Respond_CMD, DataLen};
	uint8_t Control_CRC = 0;
	uint16_t i;

	for(i = 0; i < 3; i++)
	{
		USART1->DR = ResponseHeader[i];
		while((USART1->SR & USART_FLAG_TXE) == (uint16_t)RESET);
		Control_CRC = CRC8(ResponseHeader[i], Control_CRC);
#ifdef __HOST_DEBUG__
		sprintf(&TransmitString[i*5], "0x%02X", ResponseHeader[i]);
		strcat(TransmitString, " ");
#endif
	}

	for(i = 0; i < DataLen; i++)
	{
		USART1->DR = *(pData+i);
		while((USART1->SR & USART_FLAG_TXE) == (uint16_t)RESET);
		Control_CRC = CRC8(*(pData+i), Control_CRC);
#ifdef __HOST_DEBUG__
		if(i < 5)
		{
			sprintf(&TransmitString[(i*5)+15], "0x%02X", *(pData+i));
			strcat(TransmitString, " ");
		}
#endif
	}

	Control_CRC = CRC8(0, Control_CRC);
	USART1->DR = Control_CRC;
	while((USART1->SR & USART_FLAG_TXE) == (uint16_t)RESET);

	/* Если режим однократной развертки то останавливаемся до перезапуска */
	if((pnt_gOSC_MODE->oscSync == Sync_SINGL) && (pnt_gOSC_MODE->State == RUN) && (SRAM_GetWriteState() == COMPLETE))
	{
		pnt_gOSC_MODE->State = STOP;
	}
#ifdef __HOST_DEBUG__
	if(i < 5) sprintf(&TransmitString[(DataLen*5)+15], "0x%02X", Control_CRC);
	else sprintf(&TransmitString[45], "0x%02X", Control_CRC);
#endif

}


/*******************************************************************************
* Function Name  : Transmit_DataBuf_To_Host
* Description    : Передача данных SRAM в хост
* Input          : None
* Return         : None
*******************************************************************************/
void Transmit_DataBuf_To_Host(void)
{
	uint8_t ResponseDataBufHeader[8] = {0x5B, 0x70, 0x04, 0x00, 0x00, 0x00, 0x00, 0xFF};

	uint8_t Data_A, Data_B, *pData;
	uint8_t Control_CRC = 0;
	uint32_t i;//, Timeout;
	uint32_t FullPacked = 0, DataCnt = 64000;

	/* select Channel */
	if((gOSC_MODE.Interleave == TRUE) || (pINFO == &INFO_A)){ pData = &Data_A;	ResponseDataBufHeader[6] = CHANNEL_A; }
	else{ pData = &Data_B; ResponseDataBufHeader[6] = CHANNEL_B; }

	if(Get_AutoDivider_State(pINFO->Mode.ID) == ENABLE)
	{
		ResponseDataBufHeader[7] = pINFO->AD_Type.Analog.Div;

	}

	while(gHostRequest.RequestDataLen >= 64000){ gHostRequest.RequestDataLen -= 64000; FullPacked++; }
	while((FullPacked > 0) || (gHostRequest.RequestDataLen > 0))
	{
		if(FullPacked == 0){ DataCnt = gHostRequest.RequestDataLen; gHostRequest.RequestDataLen = 0; }
		else{ DataCnt = 64000; FullPacked--; }

		TIM2->DIER &= ~TIM_DIER_UIE;

		Control_CRC = 0;
		ResponseDataBufHeader[3] = (uint8_t)(DataCnt >> 10);
		ResponseDataBufHeader[4] = (uint8_t)((DataCnt & 0x03FC) >> 2);
		ResponseDataBufHeader[5] = (uint8_t)((DataCnt & 0x03) << 6);

		/* sending header */
		for(i = 0; i < 8; i++)
		{
			USART1->DR = ResponseDataBufHeader[i];
			while((USART1->SR & USART_FLAG_TXE) == (uint16_t)RESET);
			Control_CRC = CRC8(ResponseDataBufHeader[i], Control_CRC);
		}

		/* Prepare SRAM to read, read and send data to Host */
		SRAM_ReadDirection(SRAM_READ_DOWN);

		/* Enable read state for capture data */
		SRAM_ReadState(ENABLE);

		for(i = 0; i < DataCnt; i++)
		{
			/* If a Terminate command recived */
			if(IQueue_CommandStatus == FALSE){ break; }

			if(gOSC_MODE.Interleave == TRUE)
			{
				Set_EPM570_RS(SET);
				Data_A = ~(GPIOB->IDR >> 8);

				USART1->DR = *pData;
				while((USART1->SR & USART_FLAG_TXE) == (uint16_t)RESET);
				Control_CRC = CRC8(*pData, Control_CRC);

				Set_EPM570_RS(RESET);
				Data_A = ~(GPIOB->IDR >> 8);
				Data_A -= InterliveCorrectionCoeff;

				i++;
			}
			else
			{
				Set_EPM570_RS(SET);
				Data_A = ~(GPIOB->IDR >> 8);

				Set_EPM570_RS(RESET);
				Data_B = ~(GPIOB->IDR >> 8);

				if((ActiveMode != &IntMIN_MAX) && (pnt_gOSC_MODE->oscSweep != 0))
				{
					Set_EPM570_RS(SET);
					Set_EPM570_RS(RESET);
				}
			}

			USART1->DR = *pData;
			while((USART1->SR & USART_FLAG_TXE) == (uint16_t)RESET);
			Control_CRC = CRC8(*pData, Control_CRC);


			if(i < 600)
			{
				pINFO->DATA[i] = (*pData) - 127;
			}

			/* small delay to avoid CP2102 buffer overflow */
			if((i != 0) && ((i%300) == 0))
			{
				delay_ms(15);
			}
		}

		SRAM_ReadState(DISABLE);

		/* send CRC to Host */
		Control_CRC = CRC8(0, Control_CRC);
		USART1->DR = Control_CRC;
		while((USART1->SR & USART_FLAG_TXE) == (uint16_t)RESET);

		TIM2->DIER |= TIM_DIER_UIE;
	}
}


/*******************************************************************************
* Function Name  : Decoding_Command
* Description    :
* Input          :
* Return         :
*******************************************************************************/
void Decoding_Command(void)
{
	uint8_t trDataLen;
	ErrorStatus HostDataCorrect = SUCCESS;
	uint8_t ErrorCode = ErrorData;

	/* Switch recived command from host */
	switch(CommandData[0])
	{
		/* --- Analog inputs mode - Open or Closed --- */
		case ANALOG_IN_CMD:
		{
			if((CommandData[2] <= 3) && (CommandData[3] <= 3))
			{
				if(CommandData[2] < 3)
				{
					if(CommandData[2] == 0){ INFO_A.Mode.EN = STOP; CommandData[2] = RUN_AC; }
					else INFO_A.Mode.EN = RUN;
					INFO_A.Mode.AC_DC = CommandData[2];
					Set_Input(CHANNEL_A, INFO_A.Mode.AC_DC);
				}

				if(CommandData[3] < 3)
				{
					if(CommandData[3] == 0){ INFO_B.Mode.EN = STOP; CommandData[3] = RUN_AC; }
					else INFO_B.Mode.EN = RUN;
					INFO_B.Mode.AC_DC = CommandData[3];
					Set_Input(CHANNEL_B, INFO_B.Mode.AC_DC);
				}
			}
			else{ HostDataCorrect = ERROR; }
		}
		break;

		/* --- Analog attenuator divider --- */
		case ANALOG_DIV_CMD:
		{
				if(CommandData[2] == 0xAA)
				{
					Set_AutoDivider_State(CHANNEL_A, ENABLE);
				}
				else if(CommandData[2] <= 0x0B)
				{
					if(Get_AutoDivider_State(CHANNEL_A) == ENABLE) Set_AutoDivider_State(CHANNEL_A, DISABLE);

					INFO_A.AD_Type.Analog.Div = CommandData[2];
					*(INFO_A.AD_Type.Analog.corrZ) = INFO_A.AD_Type.Analog.Zero_PWM_values[INFO_A.AD_Type.Analog.Div];
					Change_AnalogDivider(CHANNEL_A, INFO_A.AD_Type.Analog.Div);
				}

				if(CommandData[3] == 0xAA)
				{
					Set_AutoDivider_State(CHANNEL_B, ENABLE);
				}
				else if(CommandData[3] <= 0x0B)
				{
					if(Get_AutoDivider_State(CHANNEL_B) == ENABLE) Set_AutoDivider_State(CHANNEL_B, DISABLE);

					INFO_B.AD_Type.Analog.Div = CommandData[3];
					*(INFO_B.AD_Type.Analog.corrZ) = INFO_B.AD_Type.Analog.Zero_PWM_values[INFO_B.AD_Type.Analog.Div];
					Change_AnalogDivider(CHANNEL_B, INFO_B.AD_Type.Analog.Div);
				}
		}
		break;

		/* --- Calibrate zero for analog channe request --- */
		case CALIBRATE_ZERO_CMD:
		{
			if((CommandData[2] == CHANNEL_A) || (CommandData[2] == CHANNEL_B))
			{
				CalibrateChannel = CommandData[2];
				gHostRequest.State = TRUE;
				gHostRequest.Request = Calibrate_Request;
			}
			else { HostDataCorrect = ERROR; }
		}
		break;

		/* --- Synchronization mode command --- */
		case SYNC_MODE_CMD:
		{
			if(CommandData[2] <= 3)
			{
				if(CommandData[2] == 0) pnt_gOSC_MODE->oscSync = Sync_NONE;
				else { pnt_gOSC_MODE->oscSync = (SyncMode_TypeDef)CommandData[2]; }

				/* Update synchronization */
				Set_Trigger(pnt_gOSC_MODE->AnalogSyncType);			// обновляем регистры синхронизации ПЛИС
				EPM570_Sync(pnt_gOSC_MODE->oscSync);
			}
			else HostDataCorrect = ERROR;
		}
		break;

		/* --- Trigger sourse command --- */
		case TRIG_SOURSE_CMD:
		{
			if((CommandData[2] <= 2))
			{
				pnt_gOSC_MODE->SyncSourse = CommandData[2];

				/* Update synchronization */
				Set_Trigger(pnt_gOSC_MODE->AnalogSyncType);			// обновляем регистры синхронизации ПЛИС
				EPM570_Sync(pnt_gOSC_MODE->oscSync);
			}
			else{ HostDataCorrect = ERROR; }
		}
		break;

		/* --- Synchronization mode command --- */
		case TRIG_MODE_CMD:
		{
			if(CommandData[2] <= 3)
			{
				pnt_gOSC_MODE->AnalogSyncType = CommandData[2];
				Set_Trigger(pnt_gOSC_MODE->AnalogSyncType);	/* обновляем регистры синхронизации ПЛИС */
				EPM570_Sync(pnt_gOSC_MODE->oscSync);
			}
			else{ HostDataCorrect = ERROR; }
		}
		break;

		/* --- Synchronization level "UP" cursor command --- */
		case TRIG_LEVEL_UP_CMD:
		{
			Height_Y_cursor.Position = CommandData[2];
			Set_Trigger(pnt_gOSC_MODE->AnalogSyncType);	/* обновляем регистры синхронизации ПЛИС */
			EPM570_Sync(pnt_gOSC_MODE->oscSync);
		}
		break;

		/* --- Synchronization level "DOWN" cursor command --- */
		case TRIG_LEVEL_DOWN_CMD:
		{
			Low_Y_cursor.Position = CommandData[2];
			Set_Trigger(pnt_gOSC_MODE->AnalogSyncType);	/* обновляем регистры синхронизации ПЛИС */
			EPM570_Sync(pnt_gOSC_MODE->oscSync);
		}
		break;

		/* --- Synchronization X cursor position command --- */
		case TRIG_CURSOR_X_CMD:
		{
			trigPosX_cursor.Position = ((uint32_t)(CommandData[2])<<16) | ((uint32_t)(CommandData[3])<<8) | (CommandData[4]);
			//((uint16_t)CommandData[2] << 8) | (uint16_t)CommandData[3];
			Set_numPoints(pnt_gOSC_MODE->oscNumPoints);	   /* обновляем количество точек */
			EPM570_Sync(pnt_gOSC_MODE->oscSync);
		}
		break;


		/* --- Decimation --- */
		case DECIMATION_CMD:
		{
			if(CommandData[2] <= 0x14)
			{
				pnt_gOSC_MODE->oscSweep = sweep_coff[CommandData[2]] - 1;
				Set_Decimation(pnt_gOSC_MODE->oscSweep);
			}
			else{ HostDataCorrect = ERROR; }
		}
		break;

		/* --- Switch standart or MIN/MAX mode --- */
		case DATA_COLLECTION_MODE_CMD:
		{
			if(CommandData[2] == 2)
			{
				Inerlive_Cmd(ENABLE);
				changeInterpolation((InterpolationMode_TypeDef*)InterpModes[1]);
				Set_numPoints(pnt_gOSC_MODE->oscNumPoints); /* обновляем регистры ПЛИС */
			}
			else if((CommandData[2] < 1) || (pnt_gOSC_MODE->oscSweep > 1))
			{
				Inerlive_Cmd(DISABLE);
				changeInterpolation((InterpolationMode_TypeDef*)InterpModes[CommandData[2] + 1]);
				Set_numPoints(pnt_gOSC_MODE->oscNumPoints); /* обновляем регистры ПЛИС */
			}
			else{ HostDataCorrect = ERROR; }
		}
		break;

		/* --- Samples data request --- */
		case DATA_REQUEST_CMD:
		{
			gHostRequest.RequestDataLen = ((uint32_t)(CommandData[2])<<10) + ((uint32_t)(CommandData[3])<<2) + (CommandData[4]>>6);
			if((gHostRequest.RequestDataLen != 0) && (CommandData[5] <= 1))
			{
				Set_CH_TypeINFO((Channel_ID_TypeDef)CommandData[5]);
				if(ActiveMode == &IntMIN_MAX) gHostRequest.RequestDataLen *= 2;
				if(gOSC_MODE.oscNumPoints != gHostRequest.RequestDataLen)
				{
					gOSC_MODE.oscNumPoints = gHostRequest.RequestDataLen;
					Set_numPoints(gHostRequest.RequestDataLen);
				}

				gHostRequest.State = ENABLE;
				gHostRequest.Request = Data_Request;
				return;
			}
			else{ HostDataCorrect = ERROR;	}
		}
		break;

		/* --- Save all settings to EEPROM --- */
		case BATT_V_CMD:
		{
			if((CommandData[2] == 0xA0))
			{
				CommandData[2] = (uint8_t)ADC_VbattPrecent;
			}
//			else if(show_ADC_flag != SET)
//			{
//				HostDataCorrect = ERROR;
//				ErrorCode = ErrorBusy;
//			}
			else HostDataCorrect = ERROR;
		}
		break;


		/* --- Save all settings to EEPROM --- */
		case SAVE_ALL_EEPROM_CMD:
		{
			if(CommandData[2] == 0xEE)
			{
				SavePreference();
				HostDataCorrect = SUCCESS;
			}
			else { HostDataCorrect = ERROR; }
		}
		break;


		case BOOTLOADER_CMD:
		{
			if(CommandData[2] == 0x0B)
			{
				gHostRequest.RequestDataLen = 0;
				gHostRequest.State = ENABLE;
				gHostRequest.Request = Bootoader_Request;

				HostDataCorrect = SUCCESS;
			}
			else { HostDataCorrect = ERROR; }
		}
		break;


		case EXIT_HOST_MODE_CMD:
		{
			if(memcmp(CommandData, ExitHostModeRequest, 4) == 0)
			{
				Switch_To_AutoMode();
				return;
			}
			else{ HostDataCorrect = ERROR; }
		}
		break;


		case FIRMWARE_VERSION_CMD:
		{
			if(CommandData[2] == 0xFF)
			{
				CommandData[2] = __FIRMWARE_VERSION__;
			}
			else{ HostDataCorrect = ERROR; }
		}
		break;

		default: { HostDataCorrect = ERROR; }
	}


	memcpy(OUT_HostData, &CommandData[2], CMD_Length - 4);

	if(HostDataCorrect == SUCCESS)
	{
		ResponseCode = CommandData[0] + 0x40;
		trDataLen = CMD_Length - 4;
	}
	else
	{
		ResponseCode = ERROR_RESPOND;
		OUT_HostData[0] = ErrorCode;
		trDataLen = 1;
	}

	Transmit_To_Host(ResponseCode, OUT_HostData, trDataLen);
	//memset(CommandData, 0, 30);
}


/*******************************************************************************
* Function Name  : Switch_To_HostMode
* Description    :
* Input          : None
* Return         : None
*******************************************************************************/
void Switch_To_HostMode(void)
{
	NVIC_DisableIRQ(TIM2_IRQn);	  // запрет прерываний таймера опроса кнопок
	NVIC_DisableIRQ(RTC_IRQn);	  // запрет прерываний RTC

	Beep_Start();
	LCD_FillScreen(Black);
	LCD_SetBackColor(Black);

	/* Set Font and print message */
	LCD_SetFont(&arialUnicodeMS_16ptFontInfo);
	LCD_PutColorStrig(155, 200, 0, "PC MODE", LightGreen);
	LCD_SetFont(&timesNewRoman12ptFontInfo);
	LCD_SetTextColor(Gray);
	LCD_PutStrig(5, 170, 0, "Communicated with Neil Scope 3 Software");
	LCD_PutStrig(5, 10, 0, "Big sanks Ildar aka 'Muha'");

	Beep_Start();

	Transmit_To_Host(HostModeRequest[0] | 0x40, (uint8_t*)&HostModeRequest[2], 2);
	HostMode = ENABLE;
}


/*******************************************************************************
* Function Name  : Switch_To_AutoMode
* Description    :
* Input          : None
* Return         : None
*******************************************************************************/
void Switch_To_AutoMode(void)
{
	NVIC_EnableIRQ(RTC_IRQn);

	Beep_Start();
	pnt_gOSC_MODE->oscSync = Sync_NONE;
	External_Peripheral_Init();		// Инициализация и проверка внешней периферии
	LoadPreference();				// Чтение сохраненных настроек из EEPROM
	Draw_Interface();				// Нарисовать интерфейс
	Sweep_Mode(TRUE);

	GPIOC->BRR = GPIO_Pin_13;

//	/* Configurate interlive control GPIO */
//	Init_Inerlive_GPIO();

	HostMode = DISABLE;
}


/*******************************************************************************
 * Function Name  : HostRequestProcessing
 * Description    :
 * Input          : None
 * Return         : None
 *******************************************************************************/
static __inline void Host_RequestProcessing(void)
{
	if(gHostRequest.Request == Data_Request)
	{
		if(Write_SRAM() == COMPLETE)
		{
			Transmit_DataBuf_To_Host();
			Analog_AutodividerMain();
		}
	}
	else if(gHostRequest.Request == Calibrate_Request)
	{
		Auto_CorrectZ_CH(CalibrateChannel);
	}
	else if(gHostRequest.Request == Bootoader_Request)
	{
		Start_Bootloader();
	}

	if(gHostRequest.State == ENABLE) Host_RequestReset();
}


/*******************************************************************************
 * Function Name  : Host_RequestReset
 * Description    :
 * Input          : None
 * Return         : None
 *******************************************************************************/
void Host_RequestReset(void)
{
	gHostRequest.State = DISABLE;
	gHostRequest.Request = NO_Request;
	gHostRequest.RequestDataLen = 0;
}


/*******************************************************************************
* Function Name  : Get_CorrectionTerminate
* Description    :
* Input          : None
* Output         : None
* Return         : uint16_t sum
*******************************************************************************/
FlagStatus Host_GetTerminateCmd(void)
{
	if(IQueue_CommandStatus == FALSE)
	{
		Host_RequestReset();
		return SET;		// TERMINATE_CMD
	}
	else return RESET;
}


/*******************************************************************************
* Function Name  : CRC8 Buff
* Description    :
* Input          :
* Return         :
*******************************************************************************/
uint8_t CRC8_Buff(uint8_t *pBuff, uint16_t NumBytes)
{
   uint16_t i;
   uint8_t crcBuff = 0x00;

   for(i = 0; i < NumBytes; i++) crcBuff = CRC8(*(pBuff + i), crcBuff);

   return (crcBuff);
}


/*******************************************************************************
* Function Name  : CRC8
* Description    :
* Input          :
* Return         :
*******************************************************************************/
uint8_t CRC8(uint8_t Byte, uint8_t crc)
{
   uint8_t i;

   for ( i = 0; i < 8 ; i++)
   {
      if (crc & 0x80)
      {
         crc <<= 1;
         if (Byte & 0x80) crc |= 0x01;
         else crc &= 0xFE;
         crc ^= 0x85;
      }
      else
      {
        crc <<= 1;
        if (Byte & 0x80) crc |= 0x01;
        else crc &= 0xFE;
      }
      Byte <<= 1;
   }
   return (crc);
}



