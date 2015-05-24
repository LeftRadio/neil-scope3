/**
  ******************************************************************************
  * @file    init.c 
  * @author  LeftRadio
  * @version V1.0.0
  * @date    
  * @brief   Main Init Peripherals header file.
  ******************************************************************************
**/ 

#ifndef __INIT_H
#define __INIT_H

/* Includes ------------------------------------------------------------------*/
/* Exported typedef ----------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported function ---------------------------------------------------------*/
void User_I2C_Init(void);
void User_I2C_Deinit(void);
void Global_Init(void);
void External_Peripheral_Init(void);
void Start_Bootloader(void);
void Delay_ms (uint16_t mS);
void Init_Inerlive_GPIO(void);




#endif /* __INIT_H */
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
