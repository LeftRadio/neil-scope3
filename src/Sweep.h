/**
  ******************************************************************************
  * @file	 	Sweep.h
  * @author  	Neil Lab :: Left Radio
  * @version 	v1.0.0
  * @date
  * @brief		header
  ******************************************************************************
**/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SWEEP_H
#define __SWEEP_H

/* Includes ------------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported typedef ----------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
extern const uint32_t sweep_coff[];

extern uint8_t SweepIndex, ScaleIndex;
extern uint8_t *SweepScale;
extern const uint8_t SweepScaleCoff[4];
extern const uint8_t IntrlSweepScaleCoff[4];

/* Exported function ---------------------------------------------------------*/
void Sweep_UpdateState(void);





#endif /* __SWEEP_H */
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
