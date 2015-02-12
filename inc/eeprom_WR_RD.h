/**
  ******************************************************************************
  * @file	 	eeprom_WR_RD.h
  * @author  	Left Radio
  * @version 	1.5.6
  * @date
  * @brief		NeilScope3 EEPROM Write/Read header
  ******************************************************************************
**/

#ifndef _EEPROM_WR_RD__H
#define _EEPROM_WR_RD__H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
/* адреса хранения настроек в EEPROM */
#define Saved_Settings	                 1
#define CorrectZero_A_Address	         (Saved_Settings + 1)
#define CorrectZero_B_Address			 (CorrectZero_A_Address + 24)
#define Position_A_Address			     (CorrectZero_B_Address + 24)
#define Position_B_Address			     (Position_A_Address + 1)
#define Interpolation_Address			 (Position_B_Address + 1)
#define Sweep_Address					 (Interpolation_Address + 1)
#define Analog_Divider_A_Address		 (Sweep_Address + 1)
#define Analog_Divider_B_Address		 (Analog_Divider_A_Address + 1)
#define oscSweep_Address				 (Analog_Divider_B_Address + 1)
#define Saved_check_Sum_Addr			 (oscSweep_Address + 1)


/* Exported variables --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void EEPROM_Read(uint8_t *prefMassive, uint16_t start_address, uint8_t numValues);
void EEPROM_Write(uint8_t *prefMassive, uint16_t start_address, uint8_t numValues);



#endif /* _EEPROM_WR_RD__H */
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
