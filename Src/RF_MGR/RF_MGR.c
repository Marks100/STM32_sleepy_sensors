/*! \file
*               Author: mstewart
*   \brief      RF_MGR module
*/
/***************************************************************************************************
**                              Includes                                                          **
***************************************************************************************************/

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "assert.h"

#include "STDC.h"
#include "CHKSUM.h"
#include "HAL_BRD.h"
#include "NVM.h"
#include "MAIN.h"
#include "NRF24.h"
#include "CHKSUM.h"
#include "BMP280.h"
#include "DBG_LOG_MGR.h"
#include "RNG_MGR.h"
#include "RTC.h"
#include "SEN_MGR.h"
#include "MODE_MGR.h"
#include "SYSTICK_MGR.h"
#include "RF_MGR.h"


STATIC u8_t               RF_MGR_rf_frame_s[12u];
STATIC pass_fail_et       RF_MGR_status_s;
STATIC RF_MGR_rf_state_et RF_MGR_rf_state_s;


/***************************************************************************************************
**                              Data declarations and definitions                                 **
***************************************************************************************************/
/* None */



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
void RF_MGR_init( void )
{
    STDC_memset( &RF_MGR_rf_frame_s, 0x00u, sizeof( RF_MGR_rf_frame_s ) );
    RF_MGR_status_s = 0u;
    RF_MGR_rf_state_s = 0u;
}






/*!
****************************************************************************************************
*
*   \brief         RF Module tick
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
void RF_MGR_tick( void )
{
}



/*!
****************************************************************************************************
*
*   \brief         Sets the state of the RF MGR
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
void RF_MGR_set_state( RF_MGR_rf_state_et state )
{
    RF_MGR_rf_state_s = state;
}


/*!
****************************************************************************************************
*
*   \brief         Returns the state of the RF MGR
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
RF_MGR_rf_state_et RF_MGR_get_state( void )
{
    return( RF_MGR_rf_state_s );
}



/*!
****************************************************************************************************
*
*   \brief         Generates an RF frame
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
void RF_MGR_populate_rf_frame( void )
{
	u8_t  data_len = 10u;
	u16_t battery_voltage;

	RF_MGR_rf_frame_s[0u] =  RNG_MGR_gen_random_number();
	RF_MGR_rf_frame_s[1u] =  SEN_MGR_get_sensor_type();
	RF_MGR_rf_frame_s[2u] =  ( ( SEN_MGR_get_sensor_id() & 0xFF00 ) >> 8u );
	RF_MGR_rf_frame_s[3u] =  ( SEN_MGR_get_sensor_id() & 0x00FF );
	RF_MGR_rf_frame_s[4u] =  SEN_MGR_get_packet_type();
	RF_MGR_rf_frame_s[5u] =  MODE_MGR_get_operating_mode();
	RF_MGR_rf_frame_s[6u] =  7u;
	RF_MGR_rf_frame_s[7u] =  ( ( SEN_MGR_get_temperature() & 0xFF00 ) >> 8u );
	RF_MGR_rf_frame_s[8u] =  ( SEN_MGR_get_temperature() & 0x00FF );  //round the first part
	RF_MGR_rf_frame_s[9u] =  ( ( SEN_MGR_get_wakeup_time_sec() & 0xFF00 ) >> 8u );
	RF_MGR_rf_frame_s[10u] = ( SEN_MGR_get_wakeup_time_sec() & 0x00FF );
	RF_MGR_rf_frame_s[11u] = ( ( RTC_get_current_running_time_secs() & 0xFF000000 ) >> 24u );
	RF_MGR_rf_frame_s[12u] = ( ( RTC_get_current_running_time_secs() & 0x00FF0000 ) >> 16u );
	RF_MGR_rf_frame_s[13u] = ( ( RTC_get_current_running_time_secs() & 0x0000FF00 ) >> 8u );
	RF_MGR_rf_frame_s[14u] = ( ( RTC_get_current_running_time_secs() & 0x000000FF ) );
}

hghghghg
jhjjh


/*!
****************************************************************************************************
*
*   \brief         Sends an RF frame
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
void RF_MGR_send_rf_frame( void )
{
	u8_t i = 0u;
	u8_t data_test;

    /* Turn the RF chip on */
    HAL_BRD_set_NRF_power_pin_state( ON );

    /* The NRF chip needs 1.5 ms to wake up from off */
	SYSTICK_MGR_delay_us(1500);

	/* Setup initial register values */
	NRF24_set_configuration( NRF24_DEFAULT_CONFIG );

	/* The NRF chip needs 130us ms to transition to tx mode */
	SYSTICK_MGR_delay_us(300);

	for( i = 0u; i < 5; i++ )
	{
		/* Send the configured payload */
		NRF24_send_payload( RF_MGR_rf_frame_s, sizeof( RF_MGR_rf_frame_s ) );

		SYSTICK_MGR_delay_us( 30000 );
    }

    /* Turn the RF chip back off again */
	HAL_BRD_set_NRF_power_pin_state( OFF );
}


/***************************************************************************************************
**                              Private Functions                                                 **
***************************************************************************************************/


/****************************** END OF FILE *******************************************************/
