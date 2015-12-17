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
#include "stm32f10x.h"
#include "Host.h"
#include "HostCommands.h"
#include "ReceiveStateMachine.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO ReceivedStateMachine_TypeDef RStateMaschine;

/* Private function prototypes -----------------------------------------------*/
/* Extern function ----------------------------------------------------------*/
/* Functions -----------------------------------------------------------------*/

/**
  * @brief  ReceivedStateMachine_Reset
  * @param
  * @retval
  */
void ReceivedStateMachine_Reset(void)
{
    uint8_t i;

    RStateMaschine.Stage = MSG_START;
    RStateMaschine.Complite = FALSE;
    RStateMaschine.Command_Index = 255;
    RStateMaschine.Data_Cnt = 0;

    for(i = 0; i < 10; i++) {
        RStateMaschine.Data[i] = 0;
    }
}


/**
  * @brief  ReceivedStateMachine_Reset
  * @param
  * @retval
  */
void ReceivedStateMachine_Event(uint8_t byte)
{
	uint8_t i;
	uint8_t control_crc;
	ReceivedStateMachine_TypeDef* Machine = (ReceivedStateMachine_TypeDef*)&RStateMaschine;


	switch(Machine->Stage) {

	/* Start frame, 0x5B expected byte */
	case MSG_START:
		if (byte == 0x5B) {
			Machine->Stage = MSG_CODE;
		}
		else {
			ReceivedStateMachine_Reset();
			return;
		}
		break;

	/* Message code byte */
	case MSG_CODE:

		for(i = 0; i < HOST_CMD_CNT; i++) {
			if (byte == Host_Commands[i].Command[0]) {
				Machine->Command_Index = i;
				Machine->Stage = MSG_LEN;
				break;
			}
		}

		if (Machine->Stage != MSG_LEN) {
			ReceivedStateMachine_Reset();
			return;
		}
		break;


	/* Message lenght byte */
	case MSG_LEN:
		i = Machine->Command_Index;
		if ( (i != 255) && (byte == Host_Commands[i].Command[1]) ) {
			Machine->Stage = MSG_DATA;
		}
		else {
			ReceivedStateMachine_Reset();
			return;
		}
		break;


	/* Message data */
	case MSG_DATA:

		if (Machine->Data_Cnt >= Machine->Data[2] + 2) {
			Machine->Stage = MSG_END;
		}
		break;


	/* Message end, recive CRC byte */
	case MSG_END:

		/* crc for all recived bytes and 1 byte with 0x00 value */
		control_crc =  CRC8_Buff( (uint8_t*)&Machine->Data[0], Machine->Data_Cnt + 1 );
		if (control_crc == byte) {
			Machine->Stage = MSG_COMPLETE;
		}
		else {
			ReceivedStateMachine_Reset();
			return;
		}
		break;


	/* Undefined stage, reset and return */
	default:
		ReceivedStateMachine_Reset();
		return;
	}


	Machine->Data[Machine->Data_Cnt] = byte;
	Machine->Data_Cnt += 1;

	/* Host command recived successfull */
	if (Machine->Stage == MSG_COMPLETE) {
		ReceivedStateMachine_CompleteCallBack(Machine->Command_Index, Machine->Data);
		ReceivedStateMachine_Reset();
	}
}





/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
