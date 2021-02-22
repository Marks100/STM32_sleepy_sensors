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
/* None */
STATIC u16_t             SYSTICK_MGR_delay_s;
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

    SYSTICK_MGR_delay_s = 0u;
}


/*!
****************************************************************************************************
*
*   \brief         Milisecond delay API
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
void SYSTICK_MGR_delay_ms(u16_t ms)
{
	SYSTICK_MGR_delay_s = ms;
    
	while(SYSTICK_MGR_delay_s != 0u)
    {
        /* Wait for time to elapse */
        /* TODO Perhaps LPM */
    }
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
void SYSTICK_MGR_delay_us(u16_t us)
{
	/* If we are just delaying time then we want to do this as efficiently as possible,
	 * Lets reduce the clock speed down as low as we can to do this ( /256 ), this means we are 256
	 * times slower than what we originally were running at */

	asm volatile (	"MOV R0,%[loops]\n\t"\
			"1: \n\t"\
			"SUB R0, #1\n\t"\
			"CMP R0, #0\n\t"\
			"BNE 1b \n\t" : : [loops] "r" (7*us) : "memory"\
		      );
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
    if( SYSTICK_MGR_delay_s > 0u )
    {
	    SYSTICK_MGR_delay_s--;
    }
}

/***************************************************************************************************
**                              Private Functions                                                 **
***************************************************************************************************/


/****************************** END OF FILE *******************************************************/
