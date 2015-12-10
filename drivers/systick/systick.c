/**
  ******************************************************************************
  * @file	 	systick.c
  * @author  	Left Radio
  * @version 	1.2.0
  * @date
  * @brief		systick sourse
  ******************************************************************************
**/

/* Includes ------------------------------------------------------------------*/
#include "systick.h"
#include "stm32f10x_systick.h"
#include "core_cm3.h"
#include "main.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/	 
static uint8_t  delay_fac_us = 0;
static uint16_t delay_fac_ms = 0;
static FlagStatus  Status;

extern ButtonsPush_TypeDef ButtonPush;


/* Private function prototypes -----------------------------------------------*/
/* Private Functions --------------------------------------------------------*/

/**
 * @brief  delay_init
 * @param  None
 * @retval None
 */
void delay_init(void)
{
    RCC_ClocksTypeDef RCC_ClocksStatus;

    RCC_GetClocksFreq(&RCC_ClocksStatus);
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	SysTick_ITConfig(DISABLE);
    delay_fac_us = RCC_ClocksStatus.HCLK_Frequency / 8000000;
    delay_fac_ms = RCC_ClocksStatus.HCLK_Frequency / 8000;      
}
					

/**
 * @brief  delay_us
 * @param  u32 Nus
 * @retval None
 */
void delay_us(u32 us)
{
	uint32_t temp = delay_fac_us * us;
    int16_t tick = us/1864135;

    if (temp > 0x00ffffff) temp = 0x00ffffff;

	do
	{
		SysTick_SetReload(temp);
		SysTick_CounterCmd(SysTick_Counter_Clear);
		SysTick_CounterCmd(SysTick_Counter_Enable);

		do
		{
			Status = SysTick_GetFlagStatus(SysTick_FLAG_COUNT);

		} while ((Status != SET) && (ButtonPush == B_RESET));

		SysTick_CounterCmd(SysTick_Counter_Disable);
		SysTick_CounterCmd(SysTick_Counter_Clear);

	} while((tick-- > 0) && (ButtonPush == B_RESET));
}


/**
 * @brief  delay_ms
 * @param  uint16_t nms
 * @retval None
 */
void delay_ms(uint16_t nms)
{    
    uint32_t temp = delay_fac_ms * nms;

    if (temp > 0x00ffffff) temp = 0x00ffffff;

    SysTick_SetReload(temp);
    SysTick_CounterCmd(SysTick_Counter_Clear);
    SysTick_CounterCmd(SysTick_Counter_Enable);

    do
    {
        Status = SysTick_GetFlagStatus(SysTick_FLAG_COUNT);

    } while ((Status != SET) && (ButtonPush == B_RESET));

    SysTick_CounterCmd(SysTick_Counter_Disable);
	SysTick_CounterCmd(SysTick_Counter_Clear);
}








/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
