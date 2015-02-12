/*************************************************************************************
*
Description :  NeilScope3 EPM570 Registers
Version     :  1.0.0
Date        :  25.04.2012
Author      :  Left Radio                          
Comments    :  
*
**************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "EPM570_Registers.h"
#include "systick.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
volatile uint8_t EPM570_cnfPin_DATA = 0;           // переменная данных регистра ПЛИС cnfPin
volatile uint8_t EPM570_extPin_0_DATA = 0;		   // переменная данных регистра ПЛИС extPin_0
volatile uint8_t EPM570_extPin_1_DATA = 0;		   // переменная данных регистра ПЛИС extPin_1
volatile uint8_t EPM570_Write_Control_DATA = 0;	   // переменная данных регистра ПЛИС Write_Control
volatile uint8_t ExtPin_B_Data = 0;

volatile uint8_t EPM570_actual_Register;           // переменная для хранения адреса "активного" регистра ПЛИС
volatile uint8_t EPM570_Register_Process = 0;      // флаг операций над регистром ПЛИС

/* Private function prototypes -----------------------------------------------*/
__inline static void Write_Reg_Data(uint8_t data);


/* Private function  ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : ResetEPM570_cntrlSignals
* Description    : Сброс в 0 управляющих сигналов
* Input          : None
* Return         : None
*******************************************************************************/
void ResetEPM570_cntrlSignals(void)
{
	EPM570_WR_clr;
	EPM570_RD_clr;  
	EPM570_RS_clr;
}


/*******************************************************************************
* Function Name  : Set_EPM570_Port
* Description    : Настройк порта вход/выход
* Input          : GPIOMode_TypeDef GPIO_Mode
* Return         : None
*******************************************************************************/
void Set_EPM570_Port(GPIOMode_TypeDef GPIO_Mode)
{
	GPIO_InitTypeDef EPM570_GPIO_InitStructure;

	if((GPIO_Mode != GPIO_Mode_IPU) && (GPIO_Mode != GPIO_Mode_Out_PP)) return;

	/* EPM570 signals Q0-Q7 */  
	EPM570_GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_8_15;	 
	EPM570_GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;
	EPM570_GPIO_InitStructure.GPIO_Mode   = GPIO_Mode;
	GPIO_Init(GPIOB, &EPM570_GPIO_InitStructure);
}


/*******************************************************************************
* Function Name  : Write_Reg_Address
* Description    : Пишем адрес регистра ПЛИС
* Input          : address
* Return         : None
*******************************************************************************/
void Write_Reg_Address(uint8_t address)
{
    SetEPM570Reg(address);  	 // Set Register address
	
	EPM570_RS_set;				 // Set RS to 1 for write register adress
	
	/* Write register adress */
	EPM570_WR_set; 
	EPM570_WR_clr;
	
	EPM570_RS_clr;               // Set RS to 0 for end write register adress
}


/*******************************************************************************
* Function Name  : Write_Reg_Data
* Description    : Пишем данные в регистр
* Input          : data
* Return         : None
*******************************************************************************/
__inline static void Write_Reg_Data(uint8_t data)
{
    SetEPM570Reg(data);          //	Set Register data
	
	/* Write register data */
	EPM570_WR_set; 
	EPM570_WR_clr;
}


/*******************************************************************************
* Function Name  : Read_Reg_Data
* Description    : Читаем данные из регистра
* Input          : None
* Return         : uint8_t Register_Data
*******************************************************************************/
static uint8_t Read_Reg_Data(void)
{
    uint8_t Register_Data = 0;

	EPM570_RD_set;
	delay_us(100);  
	Register_Data = GPIOB->IDR >> 8;
	EPM570_RD_clr;

	EPM570_Register_Process = 0;
	return Register_Data;
}


/*******************************************************************************
* Function Name  : EPM570_Write_Register
* Description    : Запись регистра EPM570 по адресу
* Input          : uint8_t address, uint8_t data
* Return         : None
*******************************************************************************/
void EPM570_Write_Register(uint8_t address, uint8_t data)
{
	EPM570_Register_Process = 1;

	EPM570_actual_Register = address;
	
	ResetEPM570_cntrlSignals();           // Reset control signals 
	Set_EPM570_Port(OUT);                 // Data port to out
	           
	Write_Reg_Address(address);		
	Write_Reg_Data(data);                 // Пишем данные в регистр	
	
	if(address == cnfPin)EPM570_cnfPin_DATA = data;
	else if(address == extPin_reg_0)EPM570_extPin_0_DATA = data;
	else if(address == extPin_reg_1)EPM570_extPin_1_DATA = data;
	else if(address == Write_Control)EPM570_Write_Control_DATA = data;

	EPM570_Register_Process = 0;
}
	
	
/*******************************************************************************
* Function Name  : EPM570_Read_Register
* Description    : Чтение регистра EPM570 по адресу
* Input          : uint8_t address
* Return         : uint8_t RegData
*******************************************************************************/
uint8_t EPM570_Read_Register(uint8_t address)
{
	EPM570_Register_Process = 1;

	EPM570_actual_Register = address; 	
	
	ResetEPM570_cntrlSignals();			  // Reset control signals
	Set_EPM570_Port(OUT);                 // Data port to out
	
	Write_Reg_Address(address);           // Пишем адрес регистра	
	
	Set_EPM570_Port(IN);		          // Data port to in
							   
    return Read_Reg_Data();               // Read and return register data	
}



