/*! \file
*               Author: mstewart
*   \brief      DBG_LOG_MGR module
*/
/***************************************************************************************************
**                              Includes                                                          **
***************************************************************************************************/
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
#include "DBG_LOG_MGR.h"
#include "time.h"


FILE* DBG_LOG_MGR_file_s;
time_t DBG_LOG_MGR_time_S;

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
void DBG_LOG_MGR_init( void )
{
#if ( defined TEST )
    DBG_LOG_MGR_file_s = fopen("LOGGING.txt", "a");
#endif
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
void DBG_LOG_MGR_LOG( char* file, u16_t line, u8_t* message )
{
    char output_buf[250];
    STDC_memset( output_buf, 0x20, sizeof( output_buf ) );

    sprintf( output_buf, "File: %s, Line: %u, Message: %s", file, line, message );

#if ( defined TEST )
    time ( &DBG_LOG_MGR_time_S );
    fprintf(DBG_LOG_MGR_file_s,"%s",ctime(&DBG_LOG_MGR_time_S));
    fprintf(DBG_LOG_MGR_file_s, "%s\n", output_buf);
    puts(output_buf);
#else
    #if( DBG_LOG_ENABLED==1 )
        HAL_USART1_send_data( output_buf, strlen( output_buf ) );
    #endif
#endif
}
 

