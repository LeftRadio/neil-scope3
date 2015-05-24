/**
  ******************************************************************************
  * @file	 	EPM570_Registers.c
  * @author  	Neil Lab :: Left Radio
  * @version 	v1.5.0
  * @date		25.04.2015
  * @brief		sourse
  ******************************************************************************
**/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "EPM570_GPIO.h"
#include "EPM570_Registers.h"
#include "init.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/

/* Var = { reg data bits union, reg addr } */
volatile EPM570_HWRegisterSingle_TypeDef EPM570_Register_TRG_UP = { 0x00, 0x03 };
volatile EPM570_HWRegisterSingle_TypeDef EPM570_Register_TRG_DWN = { 0x00, 0x04 };
volatile EPM570_HWRegisterSingle_TypeDef EPM570_Register_LA_CND_DAT = { 0x00, 0x03 };
volatile EPM570_HWRegisterSingle_TypeDef EPM570_Register_LA_DIFF_DAT = { 0x00, 0x04 };
volatile EPM570_HWRegisterSingle_TypeDef EPM570_Register_LA_CND_MSK = { 0x00, 0x11 };
volatile EPM570_HWRegisterSingle_TypeDef EPM570_Register_LA_DIFF_MSK = { 0x00, 0x12 };
volatile EPM570_HWRegisterSingle_TypeDef EPM570_Register_InKeys = { 0x00, 0x09 };

/* Var = { reg union u32 w data[4], reg addr[4], num_bytes } */
volatile EPM570_HWRegisters_TypeDef EPM570_Register_Decim = { {{0x00, 0x00, 0x00, 0x00}}, {0x00, 0x01, 0x02, 0x00}, 3 };
volatile EPM570_HWRegisters_TypeDef EPM570_Register_WIN = { {{0x00, 0x00, 0x00, 0x00}}, {0x05, 0x06, 0x07, 0x00}, 3 };

/* Var = { reg data bits union, reg addr } */
volatile EPM570_HWIntCnf_Register_TypeDef EPM570_Register_cnfPin = { {{0, 0, 0, 0, 0, 0, 0, 0}}, 0x08 };
volatile EPM570_HWIntCnf_B_Register_TypeDef EPM570_Register_cnfPin_B = { {{0, 0, 0}}, 0x10 };
volatile EPM570_HWIntEX_0_Register_TypeDef EPM570_Register_extPin0 = { {{0, 0, 0, 0, 0}}, 0x0B };
volatile EPM570_HWIntEX_1_Register_TypeDef EPM570_Register_extPin_1 = { {{0, 0, 0, 0, 0, 0, 0, 1}}, 0x0C };
volatile EPM570_HWWriteCntrl_Register_TypeDef EPM570_Register_WriteControl = { {{0, 0, 0}}, 0x0D };


volatile FlagStatus EPM570_Register_Process = RESET;



/* Private function prototypes -----------------------------------------------*/
/* Private function  ---------------------------------------------------------*/

/**
  * @brief  ResetEPM570_cntrlSignals
  * 		reset to 0 all control signals
  * @param  None
  * @retval None
  */
__inline static void EPM570_WriteReg_Address(uint8_t address)
{
	EPM570_GPIO_SetPortData(address);
	EPM570_GPIO_AddrWriteStrobe();
}


/**
  * @brief  EPM570_WriteReg_Data
  * 		write data to epm570 register
  * @param  None
  * @retval None
  */
__inline static void EPM570_WriteReg_Data(uint8_t data)
{
	EPM570_GPIO_SetPortData(data);
	EPM570_GPIO_WriteStrobe();
}


/**
  * @brief  EPM570_ReadReg_Data
  * 		read data from epm570 register
  * @param  None
  * @retval None
  */
 __inline static uint8_t EPM570_ReadReg_Data(void)
{
    uint8_t Register_Data;

    EPM570_GPIO_RD(SET);
    EPM570_GPIO_RD(SET);
//    delay_us(100);
	Register_Data = GPIOB->IDR >> 8;
	EPM570_GPIO_RD(RESET);

	return Register_Data;
}



/**
  * @brief  EPM570_Write_Register
  *
  * @param  None
  * @retval None
  */
void EPM570_Registers_WriteReg(uint8_t address, uint8_t data)
{
	EPM570_Register_Process = 1;
	
	EPM570_GPIO_ResetCntrlSignals();
	EPM570_GPIO_SetPortState(GPIO_Mode_Out_PP);

	EPM570_WriteReg_Address(address);
	EPM570_WriteReg_Data(data);

	EPM570_Register_Process = 0;
}
	

/**
  * @brief  EPM570_Read_Register
  * @param  None
  * @retval None
  */
uint8_t EPM570_Registers_ReadReg(uint8_t address)
{
	uint8_t read_data = 0x00;

	EPM570_Register_Process = 1;
	
	EPM570_GPIO_ResetCntrlSignals();
	EPM570_GPIO_SetPortState(GPIO_Mode_Out_PP);
	
	/* Write reading register address */
	EPM570_WriteReg_Address(address);
	
	/* Reconfig port to IN, read and return register data */
	EPM570_GPIO_SetPortState(GPIO_Mode_IPU);
	read_data = EPM570_ReadReg_Data();

	EPM570_Register_Process = 0;

	return read_data;
}


/**
  * @brief  EPM570_Write_MultByteRegister
  * @param  None
  * @retval None
  */
void EPM570_Registers_WriteMultByte(volatile EPM570_HWRegisters_TypeDef* Register)
{
	uint8_t i;

	for(i = 0; i < Register->num_bytes; i++)
	{
		EPM570_Registers_WriteReg(Register->address[i], Register->data[i]);
	}
}


/**
  * @brief  EPM570_Read_MultByteRegister
  * @param  None
  * @retval None
  */
uint32_t EPM570_Registers_ReadMultByte(volatile EPM570_HWRegisters_TypeDef* Register)
{
	uint8_t i;

	for(i = 0; i < Register->num_bytes; i++)
	{
		Register->data[i] = EPM570_Registers_ReadReg(Register->address[i]);
	}

	return Register->u32_data;
}


/**
  * @brief  EPM570_Register_Operate_Status
  * 		if EPM570 busy return SET
  * @param  None
  * @retval None
  */
uint8_t EPM570_Registers_GetOperateStatus(void)
{
	return EPM570_Register_Process;
}





/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
