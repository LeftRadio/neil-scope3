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
#include "Settings.h"
#include "User_Interface.h"
#include "systick.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define EEPROM_I2C_ADDRESS			0xA0

/* Maximum Timeout values */
#define sEE_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define sEE_LONG_TIMEOUT         ((uint32_t)(10 * sEE_FLAG_TIMEOUT))

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t  sEETimeout = sEE_LONG_TIMEOUT;
uint8_t err_I2C_flag = 0;

/* Private function prototypes -----------------------------------------------*/
static void I2C_Write_EEPROM_Cycle(uint8_t *Data, uint16_t address);
static void I2C_Read_EEPROM_Cycle(uint8_t *Data, uint16_t address);
void Error_message(char* message_text);



/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : EEPROM_Read
* Description    : чтение EEPROM
* Input          : *prefMassive  -  указатель куда ложить прочитанные данные
*				 : start_address -  стартовый адрес чтения в EEPROM
*				 : numValues     -  количество байт которые нужно прочитать
* Output         : None
* Return         : None
*******************************************************************************/
void EEPROM_Read(uint8_t *prefMassive, uint16_t start_address, uint8_t numValues)
{    
	uint8_t EE_cnt;

	__disable_irq ();			// запрещаем все прерывания

	/* Читаем данные из EEPROM */
	for(EE_cnt = 0; EE_cnt < numValues; EE_cnt++)
	{
		I2C_Read_EEPROM_Cycle( prefMassive + EE_cnt, start_address + EE_cnt );
		delay_ms(10);
	}
	
	__enable_irq ();
}



/*******************************************************************************
* Function Name  : EEPROM_Write
* Description    : запись EEPROM
* Input          : *prefMassive  -  указатель откуда брать данные для записи
*				 : start_address -  стартовый адрес записи в EEPROM
*				 : numValues     -  количество байт которые нужно записать
* Output         : None
* Return         : None
*******************************************************************************/
void EEPROM_Write(uint8_t *prefMassive, uint16_t start_address, uint8_t numValues)
{
	uint8_t EE_cnt;

	__disable_irq ();			// запрещаем все прерывания
	ButtonPush = B_RESET;		// костыль :(

	/* Пишем данные в EEPROM */
	for(EE_cnt = 0; EE_cnt < numValues; EE_cnt++)
	{
		I2C_Write_EEPROM_Cycle( prefMassive + EE_cnt, start_address + EE_cnt );
		delay_ms(10);
	}
	
	__enable_irq ();
}



/*******************************************************************************
* Function Name  : I2C_Write_EEPROM_Cycle
* Description    : цикл записи одного байта в EEPROM
* Input          : *Data  		-  указатель на байт
*				 : address 		-  адрес записи в EEPROM
* Output         : None
* Return         : None
*******************************************************************************/
static void I2C_Write_EEPROM_Cycle(uint8_t *Data, uint16_t address)
{	
    uint8_t LSB_address = (uint8_t)(address & 0x00FF);
	uint8_t MSB_address = (uint8_t)((address & 0xFF00) >> 8);
	
	sEETimeout = sEE_LONG_TIMEOUT;
	err_I2C_flag = 0;

	//генерация старт на шине
	I2C_GenerateSTART(I2C1, ENABLE);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
	{
    	if((sEETimeout--) == 0)	{ Error_message("I2C Timeout Generate START");	return;	}
  	}
	
	// передача адреса
	I2C_Send7bitAddress(I2C1, EEPROM_I2C_ADDRESS, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED ))
	{
    	if((sEETimeout--) == 0)	{ Error_message("I2C Timeout Send7bitAddress");	return;	}
  	}
	
	// передача старшего байта адресса в EEPROM  
	I2C_SendData(I2C1, MSB_address);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED ))
	{
    	if((sEETimeout--) == 0)	{ Error_message("I2C Timeout Send MSB address");	return;	}
  	}

	// передача младшего байта адресса в EEPROM 
	I2C_SendData(I2C1, LSB_address);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED ))
	{
    	if((sEETimeout--) == 0)	{ Error_message("I2C Timeout Send LSB address");	return;	}
  	}	

	// передача данных  
	I2C_SendData(I2C1, *Data);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED ))
	{
    	if((sEETimeout--) == 0)	{ Error_message("I2C Timeout SendData");	return;	}
  	}
	
	// генерация стоп на шине
	I2C_GenerateSTOP(I2C1, ENABLE);
}


/*******************************************************************************
* Function Name  : I2C_Read_EEPROM_Cycle
* Description    : цикл чтения одного байта из EEPROM
* Input          : *Data  		-  указатель куда ложить прочитанный байт
*				 : address 		-  адрес чтения в EEPROM
* Output         : None
* Return         : None
*******************************************************************************/
static void I2C_Read_EEPROM_Cycle(uint8_t *Data, uint16_t address)
{
	uint8_t LSB_address = (uint8_t)(address & 0x00FF);
	uint8_t MSB_address = (uint8_t)((address & 0xFF00) >> 8);

	sEETimeout = sEE_LONG_TIMEOUT;
	
	//генерация старт на шине
	I2C_GenerateSTART(I2C1, ENABLE);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
	{
    	if((sEETimeout--) == 0)	{ Error_message("I2C Timeout Generate START");	return;	}
  	}
	
	/* передача EEPROM адреса I2C */
	I2C_Send7bitAddress(I2C1, EEPROM_I2C_ADDRESS, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED ))
	{
    	if((sEETimeout--) == 0)	{ Error_message("I2C Timeout Send7bitAddress");	return;	}
  	}
	
	/* передача старшего байта адресса в EEPROM */
	I2C_SendData(I2C1, MSB_address);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED ))
	{
    	if((sEETimeout--) == 0)	{ Error_message("I2C Timeout Send MSB address");	return;	}
  	}

	/* передача младшего байта адресса в EEPROM */
	I2C_SendData(I2C1, LSB_address);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED ))
	{
    	if((sEETimeout--) == 0)	{ Error_message("I2C Timeout Send LSB address");	return;	}
  	}

	//генерация повторного старта на шине
	I2C_GenerateSTART(I2C1, ENABLE);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
	{
    	if((sEETimeout--) == 0)	{ Error_message("I2C Timeout Generate reSTART");	return;	}
  	}
	
	/* передача EEPROM адреса I2C */
	I2C_Send7bitAddress(I2C1, EEPROM_I2C_ADDRESS, I2C_Direction_Receiver);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED ))
	{
    	if((sEETimeout--) == 0)	{ Error_message("I2C Timeout Send7bitAddress");	return;	}
  	}

	/* прием данных */
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED ))
	{
    	if((sEETimeout--) == 0)	{ Error_message("I2C Timeout ReceiveData");	return;	}
  	}
	*Data = I2C_ReceiveData(I2C1);
	
	// генерация стоп на шине
	I2C_GenerateSTOP(I2C1, ENABLE);	
}



/*******************************************************************************
* Function Name  : Error_message
* Description    : вывод сообщения о ошибке записи/чтения EEPROM
* Input          : *message_text  	-  указатель на текст ошибки
* Output         : None
* Return         : None
*******************************************************************************/
void Error_message(char* message_text)
{
	LCD_SetTextColor(0xf800);   
	LCD_PutStrig(20, 180, 0, message_text);
	delay_ms(500);
	LCD_SetTextColor(0x0000);   
	LCD_PutStrig(20, 180, 0, message_text);
	LCD_DrawGrid(&activeAreaGrid, DRAW); // перерисовываем сетку в области осциллограмм

	err_I2C_flag++;
}
