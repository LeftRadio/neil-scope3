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
#include "AutoCorrectCH.h"
#include "Processing_and_output.h"
#include "EPM570.h"
#include "EPM570_Registers.h"
#include "EPM570_GPIO.h"
#include "Synchronization.h"
#include "Sweep.h"
#include "gInterface_MENU.h"
#include "IQueue.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Host commands defines */
#define OSC_LA_CMD						0x09
#define ANALOG_IN_CMD					0x10
#define ANALOG_DIV_CMD					0x11
#define CALIBRATE_ZERO_CMD				0x12

#define SYNC_MODE_CMD					0x14
#define TRIG_SOURSE_CMD					0x15
#define TRIG_TYPE_CMD					0x16
#define TRIG_LEVEL_UP_CMD				0x17
#define TRIG_LEVEL_DOWN_CMD				0x18
#define TRIG_CURSOR_X_CMD				0x19
#define TRIG_MASK_DIFF_CMD				0x20
#define TRIG_MASK_COND_CMD				0x21

#define DECIMATION_CMD					0x25
#define DATA_COLLECTION_MODE_CMD		0x27			// STD, MIN/MAX, INTRL, RLE
#define DATA_REQUEST_CMD				0x30

#define BATT_V_CMD						0xA0
#define SAVE_ALL_EEPROM_CMD				0xEE

#define HOST_MODE_CMD					0x81
#define BOOTLOADER_CMD					0xB0
#define FIRMWARE_VERSION_CMD			0x00
#define SOFTWARE_VERSION_CMD			0x01

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
Boolean RLE_CodeSend = FALSE;

FunctionalState HostMode = DISABLE;
uint8_t ResponseCode;
volatile HostRequest_TypeDef gHostRequest = { DISABLE, NO_Request, 0 };
Channel_ID_TypeDef CalibrateChannel;

char SoftwarwVersion[20] = "";
const char* SoftwareName[3] = {
		"Connect 'NeilScope Software' ver",
		"Connect 'NeilLogic Analyzer' ver",
		"Connect 'NS test utility' ver"
};

#ifdef __HOST_DEBUG__
	char OLD_TransmitString[40] = {0};
	char TransmitString[40] = {0};
#endif

/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void Switch_To_HostMode(void);
void Decoding_Command(void);
static __inline void Host_RequestProcessing(void);
static void host_delay(volatile uint32_t delay_cnt);


/* Private Functions ---------------------------------------------------------*/

/**
  * @brief  Recive_Host_Data
  * @param  IQueueIndex - actived queue work index
  * @retval None
  */
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
				gHostRequest.DataLen = 0;
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


/**
  * @brief  Transmit_To_Host
  * @param  *pData - pointer to transmited data
  * 		DataLen - num bytes to transmit
  * @retval None
  */
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
	if((gSyncState.Mode == Sync_SINGL) && (gOSC_MODE.State == RUN) && (EPM570_SRAM_GetWriteState() == COMPLETE))
	{
		gOSC_MODE.State = STOP;
	}
#ifdef __HOST_DEBUG__
	if(i < 5) sprintf(&TransmitString[(DataLen*5)+15], "0x%02X", Control_CRC);
	else sprintf(&TransmitString[45], "0x%02X", Control_CRC);
#endif

}


/**
  * @brief  Transmit_DataBuf_To_Host
  * @param  None
  * @retval None
  */
void Transmit_DataBuf_To_Host(void)
{
	uint8_t ResponseDataBufHeader[8] = { 0x5B, 0x70, 0x04, 0x00, 0x00, 0x00, 0x00, 0xFF };
	uint8_t Data_A, Data_B, *pData;
	uint8_t Control_CRC = 0;
	uint32_t i;
	uint32_t FullPacked = 0, DataCnt = 0;
	uint32_t DataLen = 0;

	/* select Channel */
	if((gOSC_MODE.Interleave == TRUE) || (pINFO == &INFO_A))
	{
		pData = &Data_A;
		ResponseDataBufHeader[6] = CHANNEL_A;
	}
	else if (pINFO == &INFO_B)
	{
		pData = &Data_B;
		ResponseDataBufHeader[6] = CHANNEL_B;
	}
	else
	{
		if (RLE_CodeSend == TRUE) pData = &Data_B;
		else pData = &Data_A;
		ResponseDataBufHeader[6] = CHANNEL_DIGIT;
	}

	if(Get_AutoDivider_State(pINFO->Mode.ID) == ENABLE) ResponseDataBufHeader[7] = pINFO->AD_Type.Analog.Div;

	/* Roll back to start read data point */
	EPM570_SRAM_ReadState(DISABLE);
	EPM570_SRAM_Shift(0x7fffffff, SRAM_READ_DOWN);

	/* Prepare SRAM to read, read and send data to Host */
	EPM570_SRAM_ReadState(ENABLE);


	DataLen = gHostRequest.DataLen;
	while(gHostRequest.DataLen > 64000){ gHostRequest.DataLen -= 64000; FullPacked++; }
	while((FullPacked > 0) || (gHostRequest.DataLen > 0))
	{
		if(FullPacked == 0)
		{
			DataCnt = gHostRequest.DataLen;
			gHostRequest.DataLen = 0;
		}
		else
		{
			DataCnt = 64000;
			FullPacked--;
		}

		if((DataLen > 64000) && (DataCnt != 0)) host_delay(2000000);

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

		for(i = 0; i < DataCnt; i++)
		{
			/* If a Terminate command recived */
			if(IQueue_CommandStatus == FALSE){ break; }

			if( (gOSC_MODE.Mode == OSC_MODE) && (gOSC_MODE.Interleave == TRUE) )
			{
				EPM570_GPIO_RS(SET);
				Data_A = ~(GPIOB->IDR >> 8);

				USART1->DR = *pData;
				while((USART1->SR & USART_FLAG_TXE) == (uint16_t)RESET);
				Control_CRC = CRC8(*pData, Control_CRC);

				EPM570_GPIO_RS(RESET);
				Data_A = ~(GPIOB->IDR >> 8);
				Data_A -= InterliveCorrectionCoeff;

				i++;
			}
			else if(gOSC_MODE.Mode != LA_MODE)
			{
				EPM570_GPIO_RS(SET);
				Data_A = ~(GPIOB->IDR >> 8);

				EPM570_GPIO_RS(RESET);
				Data_B = ~(GPIOB->IDR >> 8);

				if( (ActiveMode != &IntMIN_MAX) && (gSamplesWin.Sweep != 0) )
				{
					EPM570_GPIO_RS(SET);
					EPM570_GPIO_RS(RESET);
				}
			}
			else
			{
				EPM570_GPIO_RS(SET);
				Data_A = ~(GPIOB->IDR >> 8);

				EPM570_GPIO_RS(RESET);
				Data_B = (GPIOB->IDR >> 8);
			}

			USART1->DR = *pData;
			while((USART1->SR & USART_FLAG_TXE) == (uint16_t)RESET);
			Control_CRC = CRC8(*pData, Control_CRC);


			if(i < 600)
			{
				pINFO->DATA[i] = (*pData) - 127;
			}
		}

		/* send CRC to Host */
		Control_CRC = CRC8(0, Control_CRC);
		USART1->DR = Control_CRC;
		while((USART1->SR & USART_FLAG_TXE) == (uint16_t)RESET);

		TIM2->DIER |= TIM_DIER_UIE;
	}

	/* Disable read state for SRAM */
	EPM570_SRAM_ReadState(DISABLE);
}


/**
  * @brief  Decoding_Command
  * @param  None
  * @retval None
  */
void Decoding_Command(void)
{
	uint8_t trDataLen;
	ErrorStatus HostDataCorrect = SUCCESS;
	uint8_t ErrorCode = ErrorData;

	/* Switch recived command from host */
	switch(CommandData[0])
	{
		/* ReConnect command */
		case HOST_MODE_CMD:
		{
			// do nothing
		}
		break;

		/* --- Oscilloscope or logic analyzer mode command --- */
		case OSC_LA_CMD:
		{
			if(CommandData[2] <= 0x01)
			{
				gOSC_MODE.Mode = (OSC_LA_Mode_Typedef)CommandData[2];

				/* Sync and data sourses */
				EPM570_Set_AnalogDigital_DataInput(gOSC_MODE.Mode);
				gSyncState.Sourse = CHANNEL_DIGIT;
				pINFO = &DINFO_A;

				/* info text */
				LCD_ClearArea(50, 200, 350, 220, 0x00);
				LCD_SetFont(&arialUnicodeMS_16ptFontInfo);
				if(gOSC_MODE.Mode == LA_MODE) LCD_PutColorStrig(135, 200, 0, "PC LA MODE", M256_Colors[180]);
				else LCD_PutColorStrig(125, 200, 0, "PC OSC MODE", M256_Colors[230]);
			}
			else{ HostDataCorrect = ERROR; }
		}
		break;

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
					Analog_SetInput_ACDC(CHANNEL_A, INFO_A.Mode.AC_DC);
				}

				if(CommandData[3] < 3)
				{
					if(CommandData[3] == 0){ INFO_B.Mode.EN = STOP; CommandData[3] = RUN_AC; }
					else INFO_B.Mode.EN = RUN;
					INFO_B.Mode.AC_DC = CommandData[3];
					Analog_SetInput_ACDC(CHANNEL_B, INFO_B.Mode.AC_DC);
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
				/* Set new sync mode */
				if(gOSC_MODE.Mode == LA_MODE)
				{
					gSyncState.Sourse = CHANNEL_DIGIT;
					gSyncState.Mode = Sync_NORM;
				}
				else
				{
					gSyncState.Mode = (SyncMode_TypeDef)CommandData[2];
				}

				gSyncState.foops->StateUpdate();
				gSyncState.foops->SetTrigg_X(gSyncState.Cursor_X);
			}
			else HostDataCorrect = ERROR;
		}
		break;

		/* --- Trigger sourse command --- */
		case TRIG_SOURSE_CMD:
		{
			if(CommandData[2] <= 0x02)
			{
				gSyncState.Sourse = (Channel_ID_TypeDef)(CommandData[2]);

				/* Update EPM570 sync and  num points registers */
				gSyncState.foops->StateUpdate();
				gSyncState.foops->SetTrigg_X(gSyncState.Cursor_X);
			}
			else{ HostDataCorrect = ERROR; }
		}
		break;

		/* --- Trigg mode command --- */
		case TRIG_TYPE_CMD:
		{
			/* analog, oscillocsope and logic analyzer modes */
			if( ((CommandData[2] <= 3) && (gOSC_MODE.Mode == OSC_MODE)) || \
				((CommandData[2] - 3 <= 4) && (gOSC_MODE.Mode == LA_MODE)) )
			{
				/* Set new sync mode */
				gSyncState.Type = (SyncAType_TypeDef)CommandData[2];

				gSyncState.foops->StateUpdate();
				gSyncState.foops->SetTrigg_X(gSyncState.Cursor_X);
			}
			else{ HostDataCorrect = ERROR; }
		}
		break;

		/* --- Synchronization level "UP" cursor command --- */
		case TRIG_LEVEL_UP_CMD:
		{
			if(gOSC_MODE.Mode == LA_MODE) EPM570_Register_LA_CND_DAT.data = CommandData[2];
			else Height_Y_cursor.Position = CommandData[2];

			/* Update EPM570 sync registers */
			gSyncState.foops->StateUpdate();
//			gSyncState.foops->SetTrigg_X(gSyncState.Cursor_X);
		}
		break;

		/* --- Synchronization level "DOWN" cursor command --- */
		case TRIG_LEVEL_DOWN_CMD:
		{
			if(gOSC_MODE.Mode == LA_MODE) EPM570_Register_LA_DIFF_DAT.data = CommandData[2];
			else Low_Y_cursor.Position = CommandData[2];

			/* Update EPM570 sync registers */
			gSyncState.foops->StateUpdate();
//			gSyncState.foops->SetTrigg_X(gSyncState.Cursor_X);
		}
		break;

		/* --- Synchronization X cursor position command --- */
		case TRIG_CURSOR_X_CMD:
		{
			/* Set new position for trig X, update numPoints */
			gSyncState.foops->StateUpdate();
			gSyncState.foops->SetTrigg_X(((uint32_t)(CommandData[2])<<16) | ((uint32_t)(CommandData[3])<<8) | (CommandData[4]));
		}
		break;


		/* --- Synchronization MASK different command --- */
		case TRIG_MASK_DIFF_CMD:
		{
			EPM570_Register_LA_DIFF_MSK.data = CommandData[2];

			if((EPM570_Register_LA_DIFF_MSK.data | EPM570_Register_LA_CND_MSK.data) != 0) gSyncState.Mode = Sync_NORM;
			else gSyncState.Mode = Sync_NONE;

			/* Update EPM570 sync registers */
			gSyncState.foops->StateUpdate();
		}
		break;

		/* --- Synchronization MASK condition command --- */
		case TRIG_MASK_COND_CMD:
		{
			EPM570_Register_LA_CND_MSK.data = CommandData[2];

			if((EPM570_Register_LA_DIFF_MSK.data | EPM570_Register_LA_CND_MSK.data) != 0) gSyncState.Mode = Sync_NORM;
			else gSyncState.Mode = Sync_NONE;

			/* Update EPM570 sync registers */
			gSyncState.foops->StateUpdate();
		}
		break;

		/* --- Decimation --- */
		case DECIMATION_CMD:
		{
			if(CommandData[2] <= 0x14)
			{
				gSamplesWin.Sweep = sweep_coff[CommandData[2]] - 1;
				EPM570_Set_Decimation(gSamplesWin.Sweep);
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
				EPM570_Set_numPoints(gOSC_MODE.oscNumPoints); /* обновляем регистры ПЛИС */
			}
			else if((CommandData[2] == 0x03) && (gOSC_MODE.Mode == LA_MODE))
			{
				EPM570_Set_LA_RLE_State(ENABLE);
			}
			else if((CommandData[2] == 0) || (gSamplesWin.Sweep > 1))
			{
				if(gOSC_MODE.Mode == LA_MODE)
				{
					EPM570_Set_LA_RLE_State(DISABLE);
				}
				else
				{
					Inerlive_Cmd(DISABLE);
					changeInterpolation((InterpolationMode_TypeDef*)InterpModes[CommandData[2] + 1]);
					EPM570_Set_numPoints(gOSC_MODE.oscNumPoints); /* обновляем регистры ПЛИС */
				}
			}

			else{ HostDataCorrect = ERROR; }
		}
		break;

		/* --- Samples data request --- */
		case DATA_REQUEST_CMD:
		{
			gHostRequest.DataLen = ((uint32_t)(CommandData[2])<<10) + ((uint32_t)(CommandData[3])<<2) + (CommandData[4]>>6);

			if(gHostRequest.DataLen != 0)
			{
				if(CommandData[5] <= 1)				/* OSC mode */
				{
					Set_CH_TypeINFO((Channel_ID_TypeDef)CommandData[5]);
					if(ActiveMode == &IntMIN_MAX) gHostRequest.DataLen *= 2;
				}
				else if(CommandData[5] == 2)		/* LA mode */
				{
					Set_CH_TypeINFO(CHANNEL_DIGIT);
					if(EPM570_Get_LA_RLE_State() == ENABLE) gHostRequest.DataLen = 256000;
				}

				if(gOSC_MODE.oscNumPoints != gHostRequest.DataLen)
				{
					gOSC_MODE.oscNumPoints = gHostRequest.DataLen;
					EPM570_Set_numPoints(gHostRequest.DataLen);
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
			else HostDataCorrect = ERROR;
		}
		break;


		/* --- Save all settings to EEPROM --- */
		case SAVE_ALL_EEPROM_CMD:
		{
			if(CommandData[2] == 0xEE)
			{
				if(SavePreference() != 0)
				{
					HostDataCorrect = ERROR;
				}
			}
			else { HostDataCorrect = ERROR; }
		}
		break;


		case BOOTLOADER_CMD:
		{
			if(CommandData[2] == 0x0B)	{
				gHostRequest.DataLen = 0;
				gHostRequest.State = ENABLE;
				gHostRequest.Request = Bootoader_Request;
			}
			else HostDataCorrect = ERROR;
		}
		break;


		case EXIT_HOST_MODE_CMD:
		{
			if(memcmp(CommandData, ExitHostModeRequest, 4) == 0) {
				gHostRequest.DataLen = 0;
				gHostRequest.State = ENABLE;
				gHostRequest.Request = Disconnect;
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

		case SOFTWARE_VERSION_CMD:
		{
			memset(SoftwarwVersion, 0, 20);
			sprintf(&SoftwarwVersion[0], "%d", CommandData[2]);
			SoftwarwVersion[strlen(SoftwarwVersion)] = '.';
			sprintf(&SoftwarwVersion[strlen(SoftwarwVersion)], "%d", CommandData[3]);

			LCD_SetFont(&timesNewRoman12ptFontInfo);
			LCD_SetTextColor(LighGreen);
			uint16_t nx = LCD_PutStrig(10, 165, 0, (char*)SoftwareName[CommandData[4]]);
			LCD_PutStrig(nx + 5, 165, 0, SoftwarwVersion);
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

	delay_ms(5);
	Transmit_To_Host(ResponseCode, OUT_HostData, trDataLen);
	//memset(CommandData, 0, 30);
}


/**
  * @brief  Switch_To_HostMode
  * @param  None
  * @retval None
  */
void Switch_To_HostMode(void)
{
	uint16_t tX = 0;
	char str[4] = {0};
	char* mcu_rev_str;
	char mcu_dev_str[12] = "";
	__IO unsigned int REV_ID, DEV_ID;

	NVIC_DisableIRQ(TIM2_IRQn);
	NVIC_DisableIRQ(RTC_IRQn);

	gSyncState.foops->StateReset();

	Beep_Start();
	LCD_FillScreen(Black);
	LCD_SetBackColor(Black);

	REV_ID = DBGMCU_GetREVID();
	DEV_ID = DBGMCU_GetDEVID();

	/* Device revision ID and device ID */
	if (REV_ID == 0x0000) mcu_rev_str = "A";
	else if (REV_ID == 0x2000) mcu_rev_str = "B";
	else if (REV_ID == 0x2001) mcu_rev_str = "Z";
	else if (REV_ID == 0x2003) mcu_rev_str = "Y/1/2/X";
	else mcu_rev_str = "-";
	sprintf(mcu_dev_str, "0x%08X", DEV_ID);

	/* Set Font and print message */
	LCD_SetFont(&timesNewRoman12ptFontInfo);

	ConvertToString(__FIRMWARE_VERSION__, str, 2);
	tX = LCD_PutColorStrig(10, 145, 0, "MCU Firmware ver", Gray);
	tX = LCD_PutColorStrig(tX, 145, 0, str, Gray);
	tX = LCD_PutColorStrig(tX, 145, 0, "  rev", Gray);
	tX = LCD_PutColorStrig(tX, 145, 0, __FIRMWARE_REVISION__, Gray);;

	tX = LCD_PutColorStrig(10, 125, 0, "MCU REV ID: ", Gray);
	tX = LCD_PutColorStrig(tX, 125, 0, mcu_rev_str, Gray);
	tX = LCD_PutColorStrig(tX, 125, 0, "  DEV ID: ", Gray);
	LCD_PutColorStrig(tX, 125, 0, mcu_dev_str, Gray);

	LCD_PutColorStrig(10, 10, 0, "Big sanks Ildar aka 'Muha'", White);

	Beep_Start();

	Transmit_To_Host(HostModeRequest[0] | 0x40, (uint8_t*)&HostModeRequest[2], 2);
	HostMode = ENABLE;
}


/**
  * @brief  Host_RequestReset
  * @param  None
  * @retval None
  */
static __inline void Host_RequestProcessing(void)
{
	if(gHostRequest.Request == Data_Request)
	{
		if(pINFO == &DINFO_A)
		{
			EPM570_SRAM_Write();

			RLE_CodeSend = FALSE;
			Transmit_DataBuf_To_Host();

			if(EPM570_Get_LA_RLE_State() == ENABLE)
			{
				gHostRequest.DataLen = 256000;
				RLE_CodeSend = TRUE;
				Transmit_DataBuf_To_Host();
			}
		}
		else
		{
			/* if channel A actived */
			if(INFO_A.Mode.EN == RUN)
			{
				/* if request for channel A start write to SRAM
				 * if request for channel B write not necessary, data for ch B already in SRAM */
				if(pINFO == &INFO_A) EPM570_SRAM_Write();
			}
			/* else if only channel B actived also start write to SRAM */
			else if(INFO_B.Mode.EN == RUN)
			{
				EPM570_SRAM_Write();
			}

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
	else if (gHostRequest.Request == Disconnect)
	{
		/* Reset (default start in automode) */
		delay_ms(100);
		NVIC_SystemReset();
//		Switch_To_AutoMode();
	}
	if(gHostRequest.State == ENABLE) Host_RequestReset();
}


/**
  * @brief  Host_RequestReset
  * @param  None
  * @retval None
  */
void Host_RequestReset(void)
{
	gHostRequest.State = DISABLE;
	gHostRequest.Request = NO_Request;
	gHostRequest.DataLen = 0;
}


/**
  * @brief  Get_CorrectionTerminate
  * @param  None
  * @retval Host terminate command state
  */
FlagStatus Host_GetTerminateCmd(void)
{
	if(IQueue_CommandStatus == FALSE)
	{
		Host_RequestReset();
		return SET;		// TERMINATE_CMD
	}
	else return RESET;
}


/**
  * @brief  Switch_To_AutoMode
  * @param  None
  * @retval None
  */
void Switch_To_AutoMode(void)
{
	Beep_Start();
	External_Peripheral_Init();		// Инициализация и проверка внешней периферии
	LoadPreference();				// Чтение сохраненных настроек из EEPROM
	Draw_Interface();				// Нарисовать интерфейс

	EPM570_Set_LA_RLE_State(DISABLE);
	GPIOC->BRR = GPIO_Pin_13;
	HostMode = DISABLE;

	NVIC_EnableIRQ(RTC_IRQn);
}


/**
  * @brief  host_delay
  * @param  delay cnt value
  * @retval None
  */
static void host_delay(volatile uint32_t delay_cnt)
{
	while(delay_cnt > 0){ delay_cnt--; }
}


/**
  * @brief  CRC8_Buff
  * @param  pointer to buff data and num bytes to calcul CRC
  * @retval CRC value for buff
  */
uint8_t CRC8_Buff(uint8_t *pBuff, uint16_t NumBytes)
{
   uint16_t i;
   uint8_t *pbuff = pBuff;
   uint8_t crcBuff = 0x00;

   for(i = 0; i < NumBytes; i++)
   {
	   crcBuff = CRC8(*pbuff, crcBuff);
	   pbuff++;
   }

   return (crcBuff);
}


/**
  * @brief  CRC8
  * @param  byte and prev CRC
  * @retval CRC
  */
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



