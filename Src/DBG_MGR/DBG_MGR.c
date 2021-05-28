/*! \file
*               Author: mstewart
*   \brief      DBG_LOG_MGR module
*/
/***************************************************************************************************
**                              Includes                                                          **
***************************************************************************************************/
#include "stm32f10x.h"
#include "core_cm3.h"
#include "stm32f10x_rcc.h"
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "COMPILER_defs.h"
#include "C_defs.h"
#include "PROJ_config.h"
#include "STDC.h"
#include "HAL_UART.h"
#include "DBG_MGR.h"
#include "time.h"


FILE* DBG_MGR_LOG_file_s;
time_t DBG_MGR_LOG_time_S;

DBG_MGR_dwt_st DBG_MGR_dwt_s;
RCC_ClocksTypeDef RCC_Clocks;
	


/*******************************************************************************
**                      Local Data                                            **
*******************************************************************************/


/*!
*******************************************************************************
*
*   \brief
*
*   \author         MS
*
*   \return         none
*
*******************************************************************************
*/
void DBG_MGR_init( void )
{
    u8_t i;

#if ( defined TEST )
    DBG_MGR_LOG_file_s = fopen("LOGGING.txt", "a");
#endif

    RCC_GetClocksFreq (&RCC_Clocks);    
    DBG_MGR_dwt_s.cycle_time_us  = ( ( 1.0 / (float)RCC_Clocks.SYSCLK_Frequency ) * 1000000.0 );

    for( i=0u; i<DBG_MGR_TIMING_WINDOWS; i++ )
    {
        DBG_MGR_dwt_s.windows[i].cycle_stop  = 0u;
        DBG_MGR_dwt_s.windows[i].cycle_start = 0u;
        DBG_MGR_dwt_s.windows[i].cycle_delta = 0u;
        DBG_MGR_dwt_s.windows[i].elapsed_time_us = 0.0;
    }

    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    ITM->LAR = 0xC5ACCE55; 
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}



/*!
*******************************************************************************
*
*   \brief
*
*   \author         MS
*
*   \return         none
*
*******************************************************************************
*/
void DBG_MGR_log_msg( char* file, u16_t line, u8_t* message )
{
    char output_buf[250u];
    STDC_memset( output_buf, 0x20, sizeof( output_buf ) );

    sprintf( output_buf, "File: %s, Line: %u, Message: %s", file, line, message );

#if ( defined TEST )
    puts(output_buf);
    time ( &DBG_MGR_LOG_time_S );
    sprintf( output_buf, "%s",ctime(&DBG_MGR_LOG_time_S));
    puts(output_buf);
#else
    #if( DBG_LOG_ENABLED==1 )
        HAL_USART1_send_data( output_buf, strlen( output_buf ) );
    #endif
#endif
}
 


/*!
*******************************************************************************
*
*   \brief          starts the DWT timer
*
*   \author         MS
*
*   \return         none
*
******************************************************************************/
void DBG_MGR_start_time_log( u8_t window )
{
    DBG_MGR_dwt_s.windows[window].cycle_start = DWT->CYCCNT;
}


/*!
*******************************************************************************
*
*   \brief          Stops te DWT timer
*
*   \author         MS
*
*   \return         none
*
******************************************************************************/
u32_t DBG_MGR_stop_time_log( u8_t window )
{
    DBG_MGR_dwt_s.windows[window].cycle_stop = DWT->CYCCNT;

    if( DBG_MGR_dwt_s.windows[window].cycle_stop < DBG_MGR_dwt_s.windows[window].cycle_start )
    {
        /* Rollover */
        DBG_MGR_dwt_s.windows[window].cycle_delta = ( U32_T_MAX - DBG_MGR_dwt_s.windows[window].cycle_stop );
    }
    else
    {
        DBG_MGR_dwt_s.windows[window].cycle_delta = ( DBG_MGR_dwt_s.windows[window].cycle_stop - DBG_MGR_dwt_s.windows[window].cycle_start );
    }

    DBG_MGR_dwt_s.windows[window].elapsed_time_us = ( (float)DBG_MGR_dwt_s.windows[window].cycle_delta * DBG_MGR_dwt_s.cycle_time_us );

    return( (u32_t)DBG_MGR_dwt_s.windows[window].elapsed_time_us );
}
