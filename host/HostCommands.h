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
#ifndef _HOST_COMMANDS__H
#define _HOST_COMMANDS__H

/* Includes ------------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
#define HOST_CMD_CNT        22

/* Exported macro ------------------------------------------------------------*/
/* Exported typedef ----------------------------------------------------------*/
typedef struct
{
    uint8_t Command[10];
    int8_t (*Handler)(uint8_t* data);
//    Boolean Complete;
} Host_Commands_TypeDef;

/* Exported variables --------------------------------------------------------*/
extern const Host_Commands_TypeDef Host_Commands[HOST_CMD_CNT];

/* Exported function ---------------------------------------------------------*/



#endif /* _HOST_COMMANDS__H */
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
