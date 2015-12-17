/**
  ******************************************************************************
  * @file       ReceivedStateMachine.h
  * @author     Left Radio
  * @version
  * @date
  * @brief      header
  ******************************************************************************
**/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _RECIVEIVE_STATE__H
#define _RECIVEIVE_STATE__H

/* Includes ------------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported typedef ----------------------------------------------------------*/
typedef enum { MSG_START, MSG_CODE, MSG_LEN, MSG_DATA, MSG_END, MSG_COMPLETE } ReceivedStates_TypeDef;
typedef struct
{
    ReceivedStates_TypeDef Stage;
    uint8_t Data[10];
    uint8_t Data_Cnt;
    uint8_t Command_Index;
    Boolean Complite;
} ReceivedStateMachine_TypeDef;

/* Exported variables --------------------------------------------------------*/
/* Exported function ---------------------------------------------------------*/
void ReceivedStateMachine_Reset(void);
void ReceivedStateMachine_Event(uint8_t byte);

__attribute__((weak)) void ReceivedStateMachine_CompleteCallBack(uint8_t command_index, uint8_t* command_data);




#endif /* _RECIVEIVE_STATE__H */
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
