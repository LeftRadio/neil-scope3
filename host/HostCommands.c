/**
  ******************************************************************************
  * @file    ReceiveStateMachine.c
  * @author  LeftRadio
  * @version V1.0.0
  * @date
  * @brief
  ******************************************************************************
**/

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "defines.h"
#include "Host.h"
#include "HostCommands.h"
#include "Synchronization.h"
#include "User_Interface.h"
#include "EPM570.h"
#include "EPM570_Registers.h"
#include "EPM570_GPIO.h"
#include "Synchronization.h"
#include "Sweep.h"
#include "Analog.h"
#include "AutoCorrectCH.h"
#include "Processing_and_output.h"
#include "Settings.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define _PID0            ((uint8_t)0x86)
#define _PID1            ((uint8_t)0x93)

/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static int8_t Cmd_Connect(uint8_t* data);
static int8_t Cmd_Disconnect(uint8_t* data);
static int8_t Osc_LA_Mode(uint8_t* data);
static int8_t Analog_CH(uint8_t* data);
static int8_t Analog_Div(uint8_t* data);
static int8_t Analog_Calibrate(uint8_t* data);
static int8_t Sync_Mode(uint8_t* data);
static int8_t Sync_Source(uint8_t* data);
static int8_t Sync_Type(uint8_t* data);
static int8_t Trigger_UP(uint8_t* data);
static int8_t Trigger_Down(uint8_t* data);
static int8_t Trigger_X(uint8_t* data);
static int8_t Trigger_Mask_Diff(uint8_t* data);
static int8_t Trigger_Mask_Cond(uint8_t* data);
static int8_t Sweep_Div(uint8_t* data);
static int8_t Sweep_Mode(uint8_t* data);
static int8_t Get_Data(uint8_t* data);
static int8_t Batt(uint8_t* data);
static int8_t Save_Eeprom(uint8_t* data);
static int8_t Bootloader(uint8_t* data);
static int8_t MCU_Firmware_Ver(uint8_t* data);
static int8_t Software_Version(uint8_t* data);


/* Private variables ---------------------------------------------------------*/
const Host_Commands_TypeDef Host_Commands[HOST_CMD_CNT] = {
    { {0x81, 0x02, _PID0, _PID1}, Cmd_Connect },
    { {0xFC, 0x02, _PID0, _PID1}, Cmd_Disconnect },
    { {0x09, 0x01, 0x00}, Osc_LA_Mode },
    { {0x10, 0x02, 0x00, 0x00}, Analog_CH },
    { {0x11, 0x02, 0x0B, 0x0B}, Analog_Div },
    { {0x12, 0x01, 0xFF}, Analog_Calibrate },
    { {0x14, 0x01, 0x00}, Sync_Mode },
    { {0x15, 0x01, 0x01}, Sync_Source },
    { {0x16, 0x01, 0x00}, Sync_Type },
    { {0x17, 0x01, 0x80}, Trigger_UP },
    { {0x18, 0x01, 0x80}, Trigger_Down },
    { {0x19, 0x03, 0x00, 0x00, 0x00}, Trigger_X },
    { {0x20, 0x01, 0xFF}, Trigger_Mask_Diff },
    { {0x21, 0x01, 0xFF}, Trigger_Mask_Cond },
    { {0x25, 0x01, 0x00}, Sweep_Div },
    { {0x27, 0x01, 0x00}, Sweep_Mode },
    { {0x30, 0x04, 0x00, 0x00, 0x00, 0x00}, Get_Data },
    { {0xA0, 0x01, 0xA0}, Batt },
    { {0xEE, 0x01, 0xEE}, Save_Eeprom },
    { {0xB0, 0x01, 0x0B}, Bootloader },
    { {0x00, 0x01, 0xFF}, MCU_Firmware_Ver },
    { {0x01, 0x03, 0x01, 0x01, 0xFF}, Software_Version }
};


/* Extern function ----------------------------------------------------------*/
/* Functions -----------------------------------------------------------------*/

static int8_t Cmd_Connect(uint8_t* data)
{ /* --- Connect command --- */
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
	HostMode = ENABLE;
	return 0;
}

static int8_t Cmd_Disconnect(uint8_t* data)
{ /* --- Disconnect command --- */
  if( (data[0] == _PID0) && (data[1] == _PID1) ) {
    gHostRequest.DataLen = 0;
    gHostRequest.State = ENABLE;
    gHostRequest.Request = Disconnect;
    return 0;
  }
  return -1;
}

static int8_t Osc_LA_Mode(uint8_t* data)
{ /* --- Oscilloscope or logic analyzer mode command --- */
  if(data[0] <= 0x01)
  {
    /* Set new OSC/LA mode */
    gOSC_MODE.Mode = (OSC_LA_Mode_Typedef)data[0];

    /* Sync and data sourses */
    EPM570_Set_AnalogDigital_DataInput(gOSC_MODE.Mode);
    gSyncState.Sourse = CHANNEL_DIGIT;
    pINFO = &DINFO_A;

    /* info text */
    LCD_ClearArea(50, 200, 350, 220, 0x00);
    LCD_SetFont(&arialUnicodeMS_16ptFontInfo);
    if(gOSC_MODE.Mode == LA_MODE) LCD_PutColorStrig(135, 200, 0, "PC LA MODE", M256_Colors[180]);
    else LCD_PutColorStrig(125, 200, 0, "PC OSC MODE", M256_Colors[230]);
    return 0;
  }
  return -1;
}

static int8_t Analog_CH(uint8_t* data)
{ /* --- Analog inputs mode - Open or Closed --- */

  uint8_t CH_A = data[0];
  uint8_t CH_B = data[1];

  if((CH_A <= 3) && (CH_B <= 3)) {

    if(CH_A < 3) {

      if(CH_A == 0) {
        INFO_A.Mode.EN = STOP;
        CH_A = RUN_AC;
      }
      else {
        INFO_A.Mode.EN = RUN;
      }
      INFO_A.Mode.AC_DC = CH_A;
      Analog_SetInput_ACDC(CHANNEL_A, INFO_A.Mode.AC_DC);
    }

    if(CH_B < 3) {
      if(CH_B == 0) {
        INFO_B.Mode.EN = STOP;
        CH_B = RUN_AC;
      }
      else {
        INFO_B.Mode.EN = RUN;
      }
      INFO_B.Mode.AC_DC = CH_B;
      Analog_SetInput_ACDC(CHANNEL_B, INFO_B.Mode.AC_DC);
    }

    return 0;
  }

  return -1;
}

static int8_t Analog_Div(uint8_t* data)
{ /* --- Analog attenuator divider --- */

  uint8_t CH_A = data[0];
  uint8_t CH_B = data[1];

  if(CH_A == 0xAA) {
    Set_AutoDivider_State(CHANNEL_A, ENABLE);
  }
  else if(CH_A <= 0x0B) {

    if(Get_AutoDivider_State(CHANNEL_A) == ENABLE) {
      Set_AutoDivider_State(CHANNEL_A, DISABLE);
    }

    INFO_A.AD_Type.Analog.Div = CH_A;
    *(INFO_A.AD_Type.Analog.corrZ) = INFO_A.AD_Type.Analog.Zero_PWM_values[INFO_A.AD_Type.Analog.Div];
    Change_AnalogDivider(CHANNEL_A, INFO_A.AD_Type.Analog.Div);
  }

  if(CH_B == 0xAA) {
    Set_AutoDivider_State(CHANNEL_B, ENABLE);
  }
  else if(CH_B <= 0x0B) {

    if(Get_AutoDivider_State(CHANNEL_B) == ENABLE) {
      Set_AutoDivider_State(CHANNEL_B, DISABLE);
    }

    INFO_B.AD_Type.Analog.Div = CH_B;
    *(INFO_B.AD_Type.Analog.corrZ) = INFO_B.AD_Type.Analog.Zero_PWM_values[INFO_B.AD_Type.Analog.Div];
    Change_AnalogDivider(CHANNEL_B, INFO_B.AD_Type.Analog.Div);
  }
  return 0;
}

static int8_t Analog_Calibrate(uint8_t* data)
{ /* --- Calibrate zero for analog channe request --- */

  if((data[0] == CHANNEL_A) || (data[0] == CHANNEL_B)) {
	gHostRequest.UserData = data[0];
    gHostRequest.State = TRUE;
    gHostRequest.Request = Calibrate_Request;
    return 0;
  }
  return -1;
}

static int8_t Sync_Mode(uint8_t* data)
{ /* --- Synchronization mode command --- */

  if(data[0] <= 3) {
    /* Set new sync mode */
    if(gOSC_MODE.Mode == LA_MODE) {
      gSyncState.Sourse = CHANNEL_DIGIT;
      gSyncState.Mode = Sync_NORM;
    }
    else {
      gSyncState.Mode = (SyncMode_TypeDef)data[0];
    }

    gSyncState.foops->StateUpdate();
    gSyncState.foops->SetTrigg_X(gSyncState.Cursor_X);
    return 0;
  }
  return -1;
}

static int8_t Sync_Source(uint8_t* data)
{ /* --- Trigger sourse command --- */

  if(data[0] <= 0x02) {
    gSyncState.Sourse = (Channel_ID_TypeDef)(data[0]);

    /* Update EPM570 sync and  num points registers */
    gSyncState.foops->StateUpdate();
    gSyncState.foops->SetTrigg_X(gSyncState.Cursor_X);
    return 0;
  }
  return -1;
}

static int8_t Sync_Type(uint8_t* data)
{ /* --- Trigg mode command --- */

  /* analog, oscillocsope and logic analyzer modes */
  if( ((data[0] <= 3) && (gOSC_MODE.Mode == OSC_MODE)) || \
      ((data[0] - 3 <= 4) && (gOSC_MODE.Mode == LA_MODE)) ) {

    /* Set new sync mode */
    gSyncState.Type = (SyncAType_TypeDef)data[0];
    gSyncState.foops->StateUpdate();
    gSyncState.foops->SetTrigg_X(gSyncState.Cursor_X);
    return 0;
  }
  return -1;
}

static int8_t Trigger_UP(uint8_t* data)
{ /* --- Synchronization level "UP" cursor command --- */

  if(gOSC_MODE.Mode == LA_MODE) {
    EPM570_Register_LA_CND_DAT.data = data[0];
  }
  else {
    Height_Y_cursor.Position = data[0];
  }
  /* Update EPM570 sync registers */
  gSyncState.foops->StateUpdate();
  return 0;
}

static int8_t Trigger_Down(uint8_t* data)
{ /* --- Synchronization level "DOWN" cursor command --- */

  if(gOSC_MODE.Mode == LA_MODE) {
    EPM570_Register_LA_DIFF_DAT.data = data[0];
  }
  else {
    Low_Y_cursor.Position = data[0];
  }
  /* Update EPM570 sync registers */
  gSyncState.foops->StateUpdate();
  return 0;
}

static int8_t Trigger_X(uint8_t* data)
{ /* --- Synchronization X cursor position command --- */
  gSyncState.foops->StateUpdate();
  gSyncState.foops->SetTrigg_X(((uint32_t)(data[0])<<16) | ((uint32_t)(data[1])<<8) | (data[2]));
  return 0;
}

static int8_t Trigger_Mask_Diff(uint8_t* data)
{ /* --- Synchronization MASK different command --- */

  EPM570_Register_LA_DIFF_MSK.data = data[0];

  if((EPM570_Register_LA_DIFF_MSK.data | EPM570_Register_LA_CND_MSK.data) != 0) {
    gSyncState.Mode = Sync_NORM;
  }
  else {
    gSyncState.Mode = Sync_NONE;
  }
  /* Update EPM570 sync registers */
  gSyncState.foops->StateUpdate();
  return 0;
}

static int8_t Trigger_Mask_Cond(uint8_t* data)
{ /* --- Synchronization MASK condition command --- */

  EPM570_Register_LA_CND_MSK.data = data[0];

  if((EPM570_Register_LA_DIFF_MSK.data | EPM570_Register_LA_CND_MSK.data) != 0) gSyncState.Mode = Sync_NORM;
  else gSyncState.Mode = Sync_NONE;

  /* Update EPM570 sync registers */
  gSyncState.foops->StateUpdate();
  return 0;
}

static int8_t Sweep_Div(uint8_t* data)
{ /* --- Decimation --- */
  if(data[0] <= 0x14) {
    gSamplesWin.Sweep = sweep_coff[data[0]] - 1;
    EPM570_Set_Decimation(gSamplesWin.Sweep);
    return 0;
  }
  return -1;
}

static int8_t Sweep_Mode(uint8_t* data)
{ /* --- Switch standart or MIN/MAX mode --- */
  if(data[0] == 0x02) {
    Inerlive_Cmd(ENABLE);
    changeInterpolation((InterpolationMode_TypeDef*)InterpModes[1]);
    EPM570_Set_numPoints(gOSC_MODE.oscNumPoints);
  }
  else if((data[0] == 0x03) && (gOSC_MODE.Mode == LA_MODE)) {
    EPM570_Set_LA_RLE_State(ENABLE);
  }
  else if((data[0] == 0x00) || (gSamplesWin.Sweep > 1)) {

    if(gOSC_MODE.Mode == LA_MODE) {
      EPM570_Set_LA_RLE_State(DISABLE);
    }
    else {
      Inerlive_Cmd(DISABLE);
      changeInterpolation((InterpolationMode_TypeDef*)InterpModes[data[0] + 1]);
      EPM570_Set_numPoints(gOSC_MODE.oscNumPoints);
    }
  }
  else {
    return -1;
  }
  return 0;
}

static int8_t Get_Data(uint8_t* data)
{ /* --- Samples data request --- */
  gHostRequest.DataLen = ((uint32_t)(data[0])<<10) + ((uint32_t)(data[1])<<2) + (data[2]>>6);

  if(gHostRequest.DataLen != 0) {

	  if(gOSC_MODE.Mode == OSC_MODE) {
      Set_CH_TypeINFO((Channel_ID_TypeDef)data[3]);
      if(ActiveMode == &IntMIN_MAX) {
    	  gHostRequest.DataLen *= 2;
      }
    }
    else if(gOSC_MODE.Mode == LA_MODE) {
      Set_CH_TypeINFO(CHANNEL_DIGIT);
      if(EPM570_Get_LA_RLE_State() == ENABLE) {
    	  gHostRequest.DataLen = 256000;
      }
    }
    else {
      return -1;
    }

    if(gOSC_MODE.oscNumPoints != gHostRequest.DataLen) {
      gOSC_MODE.oscNumPoints = gHostRequest.DataLen;
      EPM570_Set_numPoints(gHostRequest.DataLen);
    }

    gHostRequest.State = ENABLE;
    gHostRequest.Request = Data_Request;
    return 0;
  }
    return - 1;
}

static int8_t Batt(uint8_t* data)
{ /* --- Save all settings to EEPROM --- */
  if((data[0] == 0xA0)) {
    data[0] = (uint8_t)ADC_VbattPrecent;
    return 0;
  }
  return -1;
}

static int8_t Save_Eeprom(uint8_t* data)
{ /* --- Save all settings to EEPROM --- */
  if( (data[0] == 0xEE) && (SavePreference() != 0) ) {
    return 0;
  }
  return -1;
}

static int8_t Bootloader(uint8_t* data)
{ /* --- Jump to bootloader command --- */
  if(data[0] == 0x0B) {
    gHostRequest.DataLen = 0;
    gHostRequest.State = ENABLE;
    gHostRequest.Request = Bootoader_Request;
    return 0;
  }
  return -1;
}

static int8_t MCU_Firmware_Ver(uint8_t* data)
{ /* --- Jump to bootloader command --- */
  if(data[0] == 0xFF) {
    data[0] = __FIRMWARE_VERSION__;
    return 0;
  }
  return -1;
}

static int8_t Software_Version(uint8_t* data)
{ /* --- Tell to NS3 connected sofware version --- */
  uint8_t i;
  static char SoftwarwVersion[20] = "";
  static const char* SoftwareName[4] = {
    "Connect 'NeilScope Software' ver",
    "Connect 'NeilLogic Analyzer' ver",
    "Connect 'NS test utility' ver"
    "Undefined sofware :("
  };

  for(i = 0; i < 20; i ++) {
    SoftwarwVersion[i] = 0x00;
  }
  sprintf(&SoftwarwVersion[0], "%d", data[0]);
  SoftwarwVersion[strlen(SoftwarwVersion)] = '.';
  sprintf(&SoftwarwVersion[strlen(SoftwarwVersion)], "%d", data[1]);

  LCD_SetFont(&timesNewRoman12ptFontInfo);
  LCD_SetTextColor(LighGreen);
  if (data[2] >= 4) data[2] = 4;
  uint16_t nx = LCD_PutStrig(10, 165, 0, (char*)SoftwareName[data[2]]);
  LCD_PutStrig(nx + 5, 165, 0, SoftwarwVersion);

  return 0;
}







/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
