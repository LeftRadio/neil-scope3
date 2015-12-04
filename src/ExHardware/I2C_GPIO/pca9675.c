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
#include "pca9675.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void configuration(void);
static void write_pin(uint32_t pin, uint8_t state);
static void write_port(uint32_t val);
static uint8_t read_pin(uint32_t pin);
static void read_port(void *data);

/* Private variables ---------------------------------------------------------*/
NS_I2C_GPIO_TypeDef pca9675 = {
		0x42,

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
static void configuration(void)
{
	write_port(0x00000000);

	/* no other configurate needed for pca9675 */
}

/**
 * @brief  write_pin
 * @param  None
 * @retval None
 */
static void write_pin(uint32_t pin, uint8_t state)
{
	uint8_t write_data[3];
	uint16_t read_data;

	read_port(&read_data);

	read_data &= ~pin;
	read_data |= state;

	write_data[0] = (uint8_t)read_data;
	write_data[1] = (uint8_t)((read_data & 0xFF00) >> 8);

	I2C_Write_Bytes(write_data, 2, pca9675.i2c_address);
}

/**
 * @brief  write_port
 * @param  None
 * @retval None
 */
static void write_port(uint32_t val)
{
	uint8_t write_data[3];

	write_data[0] = (uint8_t)val;
	write_data[1] = (uint8_t)((val & 0xFF00) >> 8);

	I2C_Write_Bytes(write_data, 2, pca9675.i2c_address);
}

/**
 * @brief  read_pin
 * @param  None
 * @retval None
 */
static uint8_t read_pin(uint32_t pin)
{
	uint16_t read_data;
	read_port(&read_data);

	return (read_data & pin);
}

/**
 * @brief  read_port
 * @param  None
 * @retval None
 */
static void read_port(void *data)
{
	I2C_Read_Bytes((uint8_t*)data, 2, pca9675.i2c_address);
}
