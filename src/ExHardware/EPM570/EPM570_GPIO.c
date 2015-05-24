/**
  ******************************************************************************
  * @file	 	EPM570_GPIO.c
  * @author  	Neil Lab :: Left Radio
  * @version 	v1.0.0
  * @date
  * @brief
  ******************************************************************************
**/

/* Includes ------------------------------------------------------------------*/
#include "EPM570_GPIO.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/



/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern function -----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  EPM570_GPIO_Init
  * @param  None
  * @retval None
  */
void EPM570_GPIO_Configuration(void)
{
	GPIO_InitTypeDef EPM570_GPIO_InitStructure;

	EPM570_GPIO_SetPortState(GPIO_Mode_IPU);  // Настройк порта, вход

	EPM570_GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_1;	              // WR
	EPM570_GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;
	EPM570_GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &EPM570_GPIO_InitStructure);

	EPM570_GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_6 | GPIO_Pin_7;  // RS, RD
	EPM570_GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;
	EPM570_GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &EPM570_GPIO_InitStructure);

	EPM570_GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_5;		          // WR_SRAM_READY
	EPM570_GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;
	EPM570_GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_IPU;
	GPIO_Init(GPIOC, &EPM570_GPIO_InitStructure);
}


/**
  * @brief  EPM570_GPIO_RS
  * @param  NewState
  * @retval None
  */
void EPM570_GPIO_RS(FlagStatus NewState)
{
	if(NewState != RESET) EPM570_RS_set;
	else EPM570_RS_clr;
}


/**
  * @brief  EPM570_GPIO_WR
  * @param  NewState
  * @retval None
  */
void EPM570_GPIO_WR(FlagStatus NewState)
{
	if(NewState != RESET) EPM570_WR_set;
	else EPM570_WR_clr;
}


/**
  * @brief  EPM570_GPIO_RD
  * @param  NewState
  * @retval None
  */
void EPM570_GPIO_RD(FlagStatus NewState)
{
	if(NewState != RESET) EPM570_RD_set;
	else EPM570_RD_clr;
}


/**
  * @brief  ResetEPM570_cntrlSignals
  * 		reset to 0 all control signals
  * @param  None
  * @retval None
  */
void EPM570_GPIO_ResetCntrlSignals(void)
{
	EPM570_WR_clr;
	EPM570_RD_clr;
	EPM570_RS_clr;
}


/**
  * @brief  EPM570_SetPort
  * 		set to IN or to OUT data port pins
  * @param  None
  * @retval None
  */
void EPM570_GPIO_SetPortState(GPIOMode_TypeDef GPIO_Mode)
{
	GPIO_InitTypeDef EPM570_GPIO_InitStructure;

	if((GPIO_Mode != GPIO_Mode_IPU) && (GPIO_Mode != GPIO_Mode_Out_PP)) return;

	/* EPM570 signals Q0-Q7 */
	EPM570_GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_8_15;
	EPM570_GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;
	EPM570_GPIO_InitStructure.GPIO_Mode   = GPIO_Mode;
	GPIO_Init(GPIOB, &EPM570_GPIO_InitStructure);
}


/**
  * @brief  EPM570_SetPort
  * 		set to IN or to OUT data port pins
  * @param  None
  * @retval None
  */
void EPM570_GPIO_SetPortData(uint16_t data)
{
	SetEPM570Reg(data);
}


/**
  * @brief  EPM570_GPIO_WriteStrobe
  * @param  None
  * @retval None
  */
void EPM570_GPIO_WriteStrobe(void)
{
	EPM570_WR_set;
	EPM570_WR_clr;
}


/**
  * @brief  EPM570_GPIO_AddrWriteStrobe
  * @param  None
  * @retval None
  */
void EPM570_GPIO_AddrWriteStrobe(void)
{
	/* Set RS to 1 for write register adress */
	EPM570_RS_set;

	/* Write strobe for register adress */
	EPM570_GPIO_WriteStrobe();

	/* Set RS to 0 for end write register adress */
	EPM570_RS_clr;
}






/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
