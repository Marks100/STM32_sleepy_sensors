/*! \file
*               Author: mstewart
*   \brief      SYSTICK_MGR module
*/
/***************************************************************************************************
**                              Includes                                                          **
***************************************************************************************************/
#include "stm32f10x_rcc.h"
#include "misc.h"

#include "STDC.h"
#include "C_defs.h"
#include "HAL_BRD.h"



/***************************************************************************************************
**                              Data declarations and definitions                                 **
***************************************************************************************************/
STATIC u16_t             SYSTICK_MGR_cnt_s;
STATIC RCC_ClocksTypeDef RCC_Clocks_s;


/***************************************************************************************************
**                              Public Functions                                                  **
***************************************************************************************************/
/*!
****************************************************************************************************
*
*   \brief         Module (re-)initialisation function
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
void SYSTICK_MGR_init( void )
{
	RCC_GetClocksFreq(&RCC_Clocks_s);

	/* Trigger an interrupt every 1ms */
	SysTick_Config(RCC_Clocks_s.SYSCLK_Frequency/1000u);

    SYSTICK_MGR_cnt_s = 0u;
}


/*!
****************************************************************************************************
*
*   \brief         Microsecond delay API
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
void __attribute__((optimize("O3"))) SYSTICK_MGR_delay_us(u32_t us)
{
	u32_t i;
	u32_t j;

	for( j=0u; j<us; j++ )
	{
		for( i=0u; i<(RCC_Clocks_s.SYSCLK_Frequency/10000000u); i++ )
		{
			#ifdef TEST
				/* #if this out for testing as its not needed */
			#else
				asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
			#endif
		}
	}
}


/*!
****************************************************************************************************
*
*   \brief        ISR for systick handler
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
void SysTick_Handler( void )
{
	SYSTICK_MGR_cnt_s ++;

	if( SYSTICK_MGR_cnt_s > 4u )
	{
		SYSTICK_MGR_cnt_s = 0u;
	}
}

/***************************************************************************************************
**                              Private Functions                                                 **
***************************************************************************************************/


/****************************** END OF FILE *******************************************************/
