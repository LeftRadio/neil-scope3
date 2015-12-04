/**
  ******************************************************************************
  * @file	 	i2c_gpio.h
  * @author  	Left Radio
  * @version	1.0.0
  * @date
  * @brief		header
  ******************************************************************************
**/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __NS_I2C_GPIO__H
#define __NS_I2C_GPIO__H

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Exported define -----------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported typedef ----------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
extern NS_I2C_GPIO_TypeDef pca9675;

/* Exported function ---------------------------------------------------------*/
void I2CIO_Configuration(NS_I2C_GPIO_TypeDef* gpio);
void I2CIO_Write_Pin(uint32_t pin, uint8_t state);
void I2CIO_Write_Port(uint32_t val);
uint8_t I2CIO_Read_Pin(uint32_t pin);
void I2CIO_Read_Port(uint16_t *data);



#endif /* __NS_I2C_GPIO__H */
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
