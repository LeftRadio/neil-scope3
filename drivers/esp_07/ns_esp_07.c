/**
  ******************************************************************************
  * @file	 	ns_esp_07.c
  * @author  	Left Radio
  * @version 	1.0.0
  * @date
  * @brief		NeilScope3 esp07 module comunication sourse
  ******************************************************************************
**/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "defines.h"
#include "ns_esp_07.h"
#include "systick.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Extern function -----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private Functions --------------------------------------------------------*/

/**
 * @brief  main cycle
 * @param  None
 * @retval None
 */
int ESP_Configuration(void)
{
	return 0;
}


/**
 * @brief  ESP_State_Host_MCU
 * @param  None
 * @retval None
 */
int ESP_State_Host_MCU(void)
{
	/* Boot OFF*/
	I2CIO_Write_Pin(GPIO_Pin_0, Bit_SET);

	/* Revert ESP USART RX/TX, ESP_TX->MCU_RX, ESP_RX<-MCU_TX */
	I2CIO_Write_Pin(GPIO_Pin_2, Bit_SET);

	delay_ms(100);

	/* Power ON */
	I2CIO_Write_Pin(GPIO_Pin_1, Bit_SET);

	return 0;
}

/**
 * @brief  ESP_State_Interconnect_CP2102
 * @param  None
 * @retval None
 */
int ESP_State_Interconnect_CP2102(void)
{
	/* Boot OFF*/
	I2CIO_Write_Pin(GPIO_Pin_0, Bit_SET);

	/* ESP USART RX/TX, ESP_TX->CP2102_RX, ESP_RX<-CP2102_TX */
	I2CIO_Write_Pin(GPIO_Pin_2, Bit_RESET);

	delay_ms(100);

	/* Power ON */
	I2CIO_Write_Pin(GPIO_Pin_1, Bit_SET);

	return 0;
}

/**
 * @brief  ESP_State_Bootloader
 * @param  None
 * @retval None
 */
int ESP_State_Bootloader(void)
{
	/* Boot ON */
	I2CIO_Write_Pin(GPIO_Pin_0, Bit_RESET);

	/* ESP USART RX/TX, ESP_TX->CP2102_RX, ESP_RX<-CP2102_TX */
	I2CIO_Write_Pin(GPIO_Pin_2, Bit_RESET);

	delay_ms(100);

	/* Power ON */
	I2CIO_Write_Pin(GPIO_Pin_1, Bit_SET);

	return 0;
}


/**
 * @brief  ESP_State_OFF
 * @param  None
 * @retval None
 */
int ESP_State_OFF(void)
{
	/* Power OFF */
	I2CIO_Write_Pin(GPIO_Pin_1, Bit_RESET);

	/* Boot OFF */
	I2CIO_Write_Pin(GPIO_Pin_0, Bit_SET);

	/* ESP USART RX/TX, ESP_TX->CP2102_RX, ESP_RX<-CP2102_TX */
	I2CIO_Write_Pin(GPIO_Pin_2, Bit_RESET);

	return 0;
}


