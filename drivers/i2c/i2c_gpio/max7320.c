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
#include "i2c.h"
#include "i2c_gpio.h"
#include "max7320.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static int8_t configuration(void);
static int8_t write_pin(uint32_t pin, uint8_t state);
static int8_t write_port(uint32_t val);
static int8_t read_pin(uint32_t pin);
static int8_t read_port(void *data);

/* Private variables ---------------------------------------------------------*/
NS_I2C_GPIO_TypeDef max7320 = {
		0xB0,

		configuration,
		write_pin,
		write_port,
		read_pin,
		read_port
};

/* Extern function -----------------------------------------------------------*/
/* Private Functions --------------------------------------------------------*/

/**
 * @brief  configuration
 * @param  None
 * @retval None
 */
static int8_t configuration(void)
{
	return write_port(0x00000000);

	/* no other configurate needed for pca9675 */
}

/**
 * @brief  write_pin
 * @param  None
 * @retval None
 */
static int8_t write_pin(uint32_t pin, uint8_t state)
{
	uint8_t read_data;

	read_port(&read_data);

	if(state) {
		read_data |= (uint8_t)pin;
	}
	else {
		read_data &= (uint8_t)(~pin);
	}

	return I2C_Write_Bytes(&read_data, 1, max7320.i2c_address);
}

/**
 * @brief  write_port
 * @param  None
 * @retval None
 */
static int8_t write_port(uint32_t val)
{
	uint8_t write_data = (uint8_t)val;
	return I2C_Write_Bytes(&write_data, 1, max7320.i2c_address);
}

/**
 * @brief  read_pin
 * @param  None
 * @retval None
 */
static int8_t read_pin(uint32_t pin)
{
	uint8_t read_data = 0;

	if (read_port(&read_data) == -1) {
		return -1;
	}
	else if ((read_data & pin) != 0) {
		return 1;
	}
	else {
		return 0;
	}
}

/**
 * @brief  read_port
 * @param  None
 * @retval None
 */
static int8_t read_port(void *data)
{
	return I2C_Read_Bytes((uint8_t*)data, 1, max7320.i2c_address);
}
