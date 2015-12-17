/**
  ******************************************************************************
  * @file    init.c
  * @author  LeftRadio
  * @version V1.0.0
  * @date
  * @brief   Main Init Peripherals.
  ******************************************************************************
**/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "IQueue.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Instruction queue command massive */
__IO IQueue_TypeDef IQueue[IQUEUE_SIZE];
Boolean IQueue_Commands_Empty = TRUE;

/* Private function prototypes -----------------------------------------------*/
/* Extern function ----------------------------------------------------------*/
/* Functions -----------------------------------------------------------------*/

/*******************************************************************************
 * Function Name  : Host_Read_Instruction_From_Queue
 * Description    :
 * Input          : None
 * Return         : None
 *******************************************************************************/
void Host_IQueue_Initialization(void)
{
	Host_IQueue_ClearAll();
}


/*******************************************************************************
 * Function Name  : Host_IQueue_GetReadStatus
 * Description    :
 * Input          : None
 * Return         : None
 *******************************************************************************/
Boolean Host_IQueue_GetReadStatus(uint8_t index)
{
	if( IQueue[index].IsEmpty == FALSE ) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}


/*******************************************************************************
 * Function Name  : HostInstructionQueue
 * Description    :
 * Input          : None
 * Return         : None
 *******************************************************************************/
void Host_SetIQueue(uint8_t* data, uint8_t cmd_index)
{
	uint8_t i;
	uint8_t index = Host_IQueue_GetEmptyIndex();

	for(i = 0; i < CMD_MAX_SIZE; i++) {
		IQueue[index].Data[i] = data[i];
	}
	IQueue[index].CMD_Index = cmd_index;
	IQueue[index].IsEmpty = FALSE;
}

/*******************************************************************************
 * Function Name  : HostInstructionQueue
 * Description    :
 * Input          : None
 * Return         : None
 *******************************************************************************/
IQueue_TypeDef* Host_GetIQueue(uint8_t index)
{
	return (IQueue_TypeDef*)&IQueue[index];
}


/*******************************************************************************
 * Function Name  : HostInstructionQueue
 * Description    :
 * Input          : None
 * Return         : None
 *******************************************************************************/
uint8_t Host_IQueue_GetEmptyIndex(void)
{
	uint8_t i;

	/* Search empty IQueue cell */
	for(i = 0; i < IQUEUE_SIZE; i++) {
		/* Write if cell is empty */
		if(IQueue[i].IsEmpty == TRUE) {
//			Host_IQueue_SetCommandCount(+1);
			break;
		}
	}

	/* If all cells is full then overwrite the last cell */
	if(i == IQUEUE_SIZE) {
		i = IQUEUE_SIZE - 1;
		Host_IQueue_Clear(i);
	}

	return i;
}


/*******************************************************************************
 * Function Name  : Clear_IQueue_Cell
 * Description    :
 * Input          : None
 * Return         : None
 *******************************************************************************/
void Host_IQueue_Clear(uint8_t index)
{
	/* Clear IQueue cell IsEmpty flag, command length and recive first byte data */
	IQueue[index].IsEmpty = TRUE;
	IQueue[index].CMD_Index = 255;
	IQueue[index].Data[0] = 0x00;
}

/*******************************************************************************
 * Function Name  : Clear_IQueue_Cell
 * Description    :
 * Input          : None
 * Return         : None
 *******************************************************************************/
void Host_IQueue_ClearAll(void)
{
	uint8_t i;
	for(i = 0; i < IQUEUE_SIZE; i++) {
		Host_IQueue_Clear(i);
	}
}


/*******************************************************************************
 * Function Name  : Host_IQueue_GetCount
 * Description    :
 * Input          : None
 * Return         : None
 *******************************************************************************/
int8_t Host_IQueue_GetWorkIQueue(void)
{
	uint8_t i;
	int8_t cnt = -1;

	for(i = 0; i < IQUEUE_SIZE; i++) {
		if(IQueue[i].IsEmpty == FALSE) {
			cnt++;
		}
	}

	return cnt;
}





/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
