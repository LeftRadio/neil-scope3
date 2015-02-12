/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               systick.c
** Descriptions:            К№УГSysTickµДЖХНЁјЖКэДЈКЅ¶ФСУіЩЅшРР№ЬАн
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-10-30
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             
** Modified date:           
** Version:                 
** Descriptions:            
**
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "systick.h"
#include "stm32f10x_systick.h"
#include "core_cm3.h"

/* Private variables ---------------------------------------------------------*/	 
static uint8_t  delay_fac_us = 0;  /* usСУК±±¶іЛКэ */
static uint16_t delay_fac_ms = 0;  /* msСУК±±¶іЛКэ */
static FlagStatus  Status;


/*******************************************************************************
* Function Name  : delay_init
* Description    : іхКј»ЇСУіЩєЇКэ 
* Input          : None
* Output         : None
* Return         : None
* Attention		 : SYSTICKµДК±ЦУ№М¶ЁОЄHCLKК±ЦУµД1/8
*******************************************************************************/
void delay_init(void)
{
    RCC_ClocksTypeDef RCC_ClocksStatus;

    RCC_GetClocksFreq(&RCC_ClocksStatus);
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);  /*СЎФсНвІїК±ЦУ  HCLK/8 */
	SysTick_ITConfig(DISABLE);
    delay_fac_us = RCC_ClocksStatus.HCLK_Frequency / 8000000;
    delay_fac_ms = RCC_ClocksStatus.HCLK_Frequency / 8000;      
}
					
/*******************************************************************************
* Function Name  : delay_us
* Description    : іхКј»ЇСУіЩєЇКэ 
* Input          : - Nus: СУК±us
* Output         : None
* Return         : None
* Attention		 : ІОКэЧоґуЦµОЄ 0xffffff / (HCLK / 8000000)
*******************************************************************************/            
void delay_us(u32 Nus)
{ 
    uint8_t tmp_IRQ_TIM2 = NVIC_GetStatusIRQ(TIM2_IRQn);
	uint32_t temp = delay_fac_us * Nus;
    uint8_t sec = Nus/1864135;
    NVIC_DisableIRQ(TIM2_IRQn);	  // запрет прерываний таймера опроса кнопок

	do
	{
		if (temp > 0x00ffffff) temp = 0x00ffffff;

		SysTick_SetReload(temp);
		SysTick_CounterCmd(SysTick_Counter_Clear);
		SysTick_CounterCmd(SysTick_Counter_Enable);
		do
		{
			Status = SysTick_GetFlagStatus(SysTick_FLAG_COUNT);
		} while (Status != SET);
		SysTick_CounterCmd(SysTick_Counter_Disable);
		SysTick_CounterCmd(SysTick_Counter_Clear);
	} while(sec-- != 0);

	if(tmp_IRQ_TIM2 == 1) NVIC_EnableIRQ(TIM2_IRQn);	  // разрешение прерываний таймера опроса кнопок   
}


/*******************************************************************************
* Function Name  : delay_ms
* Description    : іхКј»ЇСУіЩєЇКэ 
* Input          : - nms: СУК±ms
* Output         : None
* Return         : None
* Attention		 : ІОКэЧоґуЦµ nms<=0xffffff*8*1000/SYSCLK ¶Ф72MМхјюПВ,nms<=1864 
*******************************************************************************/  
void delay_ms(uint16_t nms)
{    
    uint32_t temp = delay_fac_ms * nms;
	uint8_t tmp_IRQ_TIM2 = NVIC_GetStatusIRQ(TIM2_IRQn);

	NVIC_DisableIRQ(TIM2_IRQn);	  // запрет прерываний таймера опроса кнопок

    if (temp > 0x00ffffff)
    {
        temp = 0x00ffffff;
    }
    SysTick_SetReload(temp);		             /* К±јдјУФШ */
    SysTick_CounterCmd(SysTick_Counter_Clear);	 /* ЗеїХјЖКэЖч */
    SysTick_CounterCmd(SysTick_Counter_Enable);	 /* їЄКјµ№Кэ */ 
    do
    {
        Status = SysTick_GetFlagStatus(SysTick_FLAG_COUNT);
    }while (Status != SET);				         /* µИґэК±јдµЅґп */
    SysTick_CounterCmd(SysTick_Counter_Disable); /* №Ш±ХјЖКэЖч */
	SysTick_CounterCmd(SysTick_Counter_Clear);	 /* ЗеїХјЖКэЖч */

	if(tmp_IRQ_TIM2 == 1) NVIC_EnableIRQ(TIM2_IRQn);	  // разрешение прерываний таймера опроса кнопок	
}



/*******************************************************************************
* Function Name  : delay_us
* Description    : запуск таймера для таймаута
* Input          : None
* Output         : None
* Return         : None
* Attention		 : 0xffffff / (HCLK / 8000000)
*******************************************************************************/            
void time_out(uint16_t nms)
{    
    SysTick_SetReload(delay_fac_ms * nms);
	SysTick_CounterCmd(SysTick_Counter_Clear);		
    SysTick_CounterCmd(SysTick_Counter_Enable);		 	
    do
    {
        Status = SysTick_GetFlagStatus(SysTick_FLAG_COUNT);
    }while (Status != SET);
								
    SysTick_CounterCmd(SysTick_Counter_Disable);    
	SysTick_CounterCmd(SysTick_Counter_Clear);	  
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
















