/**
  ******************************************************************************
  * @file	 	i2c_gpio.c
  * @author  	Left Radio
  * @version 	1.0.0
  * @date
  * @brief		NeilScope3 i2c_gpio sourse
  ******************************************************************************
**/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "i2c_gpio.h"
#include "pca9675.h"
#include "systick.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
NS_I2C_GPIO_TypeDef *ns_i2c_gpio;

/* Extern function -----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private Functions --------------------------------------------------------*/

/**
 * @brief  I2CIO_Write_Pin
 * @param  None
 * @retval None
 */
int8_t I2CIO_Configuration(NS_I2C_GPIO_TypeDef* gpio)
{
	if(gpio != (void*)0) {
		ns_i2c_gpio = gpio;
		return ns_i2c_gpio->Configuration();
	}
	return -1;
}

/**
 * @brief  I2CIO_Write_Pin
 * @param  None
 * @retval None
 */
void I2CIO_Write_Pin(uint32_t pin, uint8_t state)
{
	if(ns_i2c_gpio != (void*)0) {
		__disable_irq ();
		ButtonPush = B_RESET;

		ns_i2c_gpio->Write_Pin(pin, state);
		delay_ms(10);

		__enable_irq ();
	}
}

/**
 * @brief  I2CIO_Write_Port
 * @param  None
 * @retval None
 */
void I2CIO_Write_Port(uint32_t val)
{
	if(ns_i2c_gpio != (void*)0) {
		__disable_irq ();
		ButtonPush = B_RESET;

		ns_i2c_gpio->Write_Port(val);
		delay_ms(10);

		__enable_irq ();
	}
}

/**
 * @brief  I2CIO_Write_Pin
 * @param  None
 * @retval None
 */
int8_t I2CIO_Read_Pin(uint32_t pin)
{
	int8_t state;

	if(ns_i2c_gpio != (void*)0) {
		__disable_irq ();
		ButtonPush = B_RESET;

		state = ns_i2c_gpio->Read_Pin(pin);
		delay_ms(10);

		__enable_irq ();

		return state;
	}

	return -1;
}

/**
 * @brief  I2CIO_Read_Port
 * @param  None
 * @retval None
 */
void I2CIO_Read_Port(uint16_t *data)
{
	if(ns_i2c_gpio != (void*)0) {
		__disable_irq ();
		ButtonPush = B_RESET;

		ns_i2c_gpio->Read_Port(data);
		delay_ms(10);

		__enable_irq ();
	}
}
