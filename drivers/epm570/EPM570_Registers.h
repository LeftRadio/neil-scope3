/**
  ******************************************************************************
  * @file	 	EPM570_Registers.h
  * @author  	Neil Lab :: Left Radio
  * @version 	v1.5.0
  * @date		26.04.2015
  * @brief		NeilScope3 EPM570 Registers header
  ******************************************************************************
**/

#ifndef __EPM570_REGISTERS_H
#define __EPM570_REGISTERS_H 

/* Includes ------------------------------------------------------------------*/	   
#include "stm32f10x_gpio.h"
#include "EPM570_RegVar.h"

/* Exported typedef -----------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
#define SRAM_DATA        				(uint8_t)15

/* Exported variables --------------------------------------------------------*/
/** All EPM570 registers  **/
extern volatile EPM570_HWRegisters_TypeDef EPM570_Register_Decim;
extern volatile EPM570_HWRegisters_TypeDef EPM570_Register_WIN;
extern volatile EPM570_HWRegisterSingle_TypeDef EPM570_Register_TRG_UP;
extern volatile EPM570_HWRegisterSingle_TypeDef EPM570_Register_TRG_DWN;
extern volatile EPM570_HWRegisterSingle_TypeDef EPM570_Register_LA_CND_DAT;
extern volatile EPM570_HWRegisterSingle_TypeDef EPM570_Register_LA_DIFF_DAT;
extern volatile EPM570_HWIntCnf_Register_TypeDef EPM570_Register_cnfPin;
extern volatile EPM570_HWRegisterSingle_TypeDef EPM570_Register_InKeys;
extern volatile EPM570_HWIntCnf_B_Register_TypeDef EPM570_Register_cnfPin_B;
extern volatile EPM570_HWIntEX_0_Register_TypeDef EPM570_Register_extPin0;
extern volatile EPM570_HWIntEX_1_Register_TypeDef EPM570_Register_extPin_1;
extern volatile EPM570_HWWriteCntrl_Register_TypeDef EPM570_Register_WriteControl;
extern volatile EPM570_HWRegisterSingle_TypeDef EPM570_Register_LA_CND_MSK;
extern volatile EPM570_HWRegisterSingle_TypeDef EPM570_Register_LA_DIFF_MSK;
extern volatile FlagStatus EPM570_Register_Process;


/* Exported function --------------------------------------------------------*/
void EPM570_Registers_WriteReg(uint8_t address, uint8_t data);
uint8_t EPM570_Registers_ReadReg(uint8_t address);
void EPM570_Registers_WriteMultByte(volatile EPM570_HWRegisters_TypeDef* Register);
uint32_t EPM570_Registers_ReadMultByte(volatile EPM570_HWRegisters_TypeDef* Register);
uint8_t EPM570_Registers_GetOperateStatus(void);





#endif /* __EPM570_REGISTERS_H */
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
