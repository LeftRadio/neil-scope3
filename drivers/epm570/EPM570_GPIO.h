/**
  ******************************************************************************
  * @file	 	EPM570_GPIO.h
  * @author  	Neil Lab :: Left Radio
  * @version 	v1.0.0
  * @date
  * @brief		header
  ******************************************************************************
**/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __EPM570_GPIO_H
#define __EPM570_GPIO_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_gpio.h"

/* Exported define -----------------------------------------------------------*/
#define EPM570_WR_set     				GPIOB->BSRR=GPIO_BSRR_BS1
#define EPM570_WR_clr     				GPIOB->BSRR=GPIO_BSRR_BR1
#define EPM570_RD_set     				GPIOC->BSRR=GPIO_BSRR_BS7
#define EPM570_RD_clr     				GPIOC->BSRR=GPIO_BSRR_BR7
#define EPM570_RS_set     				GPIOC->BSRR=GPIO_BSRR_BS6
#define EPM570_RS_clr     				GPIOC->BSRR=GPIO_BSRR_BR6

#define GPIO_Pin_7_0      				(uint16_t)0x00FF
#define GPIO_Pin_8_15     				(uint16_t)0xFF00

#define EPM570_portMask   				(uint16_t)0x00FF
#define	SetEPM570Reg(address)          	{ GPIOB->ODR = (GPIOB->IDR & EPM570_portMask) | (address << 8); }

#define EPM570_Write_Ready              (GPIOC->IDR & GPIO_Pin_5)


/* Exported macro ------------------------------------------------------------*/
/* Exported typedef ----------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported function ---------------------------------------------------------*/
void EPM570_GPIO_Configuration(void);

void EPM570_GPIO_RS(FlagStatus NewState);
void EPM570_GPIO_WR(FlagStatus NewState);
void EPM570_GPIO_RD(FlagStatus NewState);
void EPM570_GPIO_ResetCntrlSignals(void);
void EPM570_GPIO_SetPortState(GPIOMode_TypeDef GPIO_Mode);

void EPM570_GPIO_SetPortData(uint16_t data);
void EPM570_GPIO_WriteStrobe(void);
void EPM570_GPIO_AddrWriteStrobe(void);



#endif /* __EPM570_GPIO_H */
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
