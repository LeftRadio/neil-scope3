/**
  ******************************************************************************
  * @file	 	EPM570_RegVar.h
  * @author  	Neil Lab :: Left Radio
  * @version 	v1.0.0
  * @date
  * @brief		header
  ******************************************************************************
**/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __EPM570_REG_VAR_H
#define __EPM570_REG_VAR_H

/* Includes ------------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported typedef ----------------------------------------------------------*/
/* typedef for registers */
typedef struct
{
	uint8_t data;
	const uint16_t address;

} EPM570_HWRegisterSingle_TypeDef;

typedef struct
{
	union {
		uint8_t data[4];
		uint32_t u32_data;
	};
	const uint16_t address[4];
	const uint8_t num_bytes;

} EPM570_HWRegisters_TypeDef;


/* typedefs for controls (work, sync and other) registers */
typedef struct
{
	union {
		struct
		{
			unsigned char ADC_SyncSourse_RLE_Sel 	: 1;
			unsigned char GlobalSync_ON 			: 1;
			unsigned char ADC_SyncWIN 				: 1;
			unsigned char ReadCounterEN 			: 1;
			unsigned char Read_SRAM_UP 				: 1;
			unsigned char OSC_LA_DATA 				: 1;
			unsigned char AND_OR_LA_TRIGG 			: 1;
			unsigned char OSC_LA_TRIGG 				: 1;
		} bit;
		uint8_t bits;
	};
	const uint16_t address;

} EPM570_HWIntCnf_Register_TypeDef;

/* typedefs for controls (work, sync and other) registers */
typedef struct
{
	union {
		struct {
			unsigned char ReadCounter_sLoad 	: 1;
			unsigned char INTRL_0_1 			: 2;
			unsigned char bit3_7 				: 5;
		} bit;
		uint8_t bits;
	};
	const uint16_t address;

} EPM570_HWIntCnf_B_Register_TypeDef;

/* typedefs for external pin controls register */
typedef struct
{
	union {
		struct {
			unsigned char S1 		: 1;
			unsigned char S2 		: 1;
			unsigned char O_C_A 	: 1;
			unsigned char O_C_B 	: 1;
			unsigned char bit4_7 	: 4;
		} bit;
		uint8_t bits;
	};
	const uint16_t address;

} EPM570_HWIntEX_0_Register_TypeDef;

/* typedefs for external pin controls register */
typedef struct
{
	union {
		struct {
			unsigned char A0		: 1;
			unsigned char A1		: 1;
			unsigned char A2		: 1;
			unsigned char B0		: 1;
			unsigned char B1		: 1;
			unsigned char B2		: 1;
			unsigned char OSC_EN	: 1;
			unsigned char BckLght	: 1;
		} bit;
		uint8_t bits;
	};
	const uint16_t address;

} EPM570_HWIntEX_1_Register_TypeDef;

/* typedefs for write control register */
typedef struct
{
	union {
		struct {
			unsigned char Start_Write 	: 1;
			unsigned char Trigger_EN 	: 1;
			unsigned char bit2_7 		: 6;
		} bit;
		uint8_t bits;
	};
	const uint16_t address;

} EPM570_HWWriteCntrl_Register_TypeDef;



/* Exported variables --------------------------------------------------------*/
/* Exported function ---------------------------------------------------------*/








#endif /* __EPM570_REG_VAR_H */
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
