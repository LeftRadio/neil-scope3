/**
  ******************************************************************************
  * @file	 	eeprom_WR_RD.c
  * @author  	Left Radio
  * @version 	1.5.6
  * @date
  * @brief		NeilScope3 EEPROM Write/Read sourse
  ******************************************************************************
**/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "eeprom_WR_RD.h"
#include "i2c.h"
#include "Settings.h"
#include "User_Interface.h"
#include "systick.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define EEPROM_I2C_ADDRESS			0xA0

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
 * @brief  EEPROM read
 * @param  None
 * @retval None
 */
void EEPROM_Read(uint8_t *prefMassive, uint16_t start_address, uint8_t numValues)
{    
	uint8_t EE_cnt;
	uint8_t i2c_data[2];

	__disable_irq ();			// запрещаем все прерывания

	/* Читаем данные из EEPROM */
	for(EE_cnt = 0; EE_cnt < numValues; EE_cnt++)
	{
		i2c_data[0] = (uint8_t)(((start_address + EE_cnt) & 0xFF00) >> 8);
		i2c_data[1] = (uint8_t)((start_address + EE_cnt) & 0x00FF);

		I2C_Write_Bytes(i2c_data, 2, EEPROM_I2C_ADDRESS);
		I2C_Read_Bytes((prefMassive + EE_cnt), 1, EEPROM_I2C_ADDRESS);

		delay_ms(10);
	}
	
	__enable_irq ();
}

/**
 * @brief  EEPROM write
 * @param  None
 * @retval None
 */
void EEPROM_Write(uint8_t *prefMassive, uint16_t start_address, uint8_t numValues)
{
	uint8_t EE_cnt;
	uint8_t i2c_data[3];

	__disable_irq ();			// запрещаем все прерывания
	ButtonPush = B_RESET;		// костыль :(

	/* Пишем данные в EEPROM */
	for(EE_cnt = 0; EE_cnt < numValues; EE_cnt++)
	{
		i2c_data[0] = (uint8_t)(((start_address + EE_cnt) & 0xFF00) >> 8);
		i2c_data[1] = (uint8_t)((start_address + EE_cnt) & 0x00FF);

		i2c_data[2] = *(prefMassive + EE_cnt);

		I2C_Write_Bytes(i2c_data, 3, EEPROM_I2C_ADDRESS);
		delay_ms(10);
	}
	
	__enable_irq ();
}




/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
