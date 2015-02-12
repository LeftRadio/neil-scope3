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
static __IO IQueue_TypeDef IQueue[IQUEUE_SIZE];
static __IO int8_t IQueue_CommandCount = 0;
__IO Boolean IQueue_CommandStatus = FALSE;


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
	uint8_t i;

	for(i = 0; i < IQUEUE_SIZE; i++)
	{
		IQueue[i].IsEmpty = TRUE;
		IQueue[i].CMD_Length = 0;
		IQueue[i].Data[0] = 0;
//		IQueue[i].WorkDone = SUCCESS;
	}
}


/*******************************************************************************
 * Function Name  : Host_Read_Instruction_From_Queue
 * Description    :
 * Input          : None
 * Return         : None
 *******************************************************************************/
Boolean Host_IQueue_GetReadStatus(uint8_t *WorkIndex)
{
	uint8_t i;

	/* Search */
	for(i = 0; i < IQUEUE_SIZE; i++)
	{
		if((IQueue[i].IsEmpty == FALSE) && (IQueue[i].Data[0] == 0x5B))
		{
			*WorkIndex = i;
			IQueue_CommandStatus = TRUE;
			return TRUE;
		}
	}

	return FALSE;
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
	for(i = 0; i < IQUEUE_SIZE; i++)
	{
		/* Write if cell is empty */
		if(IQueue[i].IsEmpty == TRUE)
		{
			Host_IQueue_SetCommandCount(+1);
			break;
		}
	}

	/* If all cells is full then overwrite the last cell */
	if(i == IQUEUE_SIZE)
	{
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
	IQueue[index].CMD_Length = 0;
	IQueue[index].Data[0] = 0x00;
}


/*******************************************************************************
 * Function Name  : Host_IQueue_GetCommandCount
 * Description    :
 * Input          : None
 * Return         : None
 *******************************************************************************/
int8_t Host_IQueue_Get_CommandsCount(void)
{
	return IQueue_CommandCount;
}


/*******************************************************************************
 * Function Name  : Host_IQueue_SetCommandCount
 * Description    :
 * Input          : None
 * Return         : None
 *******************************************************************************/
void Host_IQueue_SetCommandCount(int8_t sign)
{
	if((IQueue_CommandCount >= 0) && (IQueue_CommandCount < IQUEUE_SIZE))
	{
		IQueue_CommandCount += sign;
	}
}




/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
