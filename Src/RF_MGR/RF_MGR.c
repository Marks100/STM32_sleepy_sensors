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
#include "NRF24_ll.h"
#include "CHKSUM.h"
#include "BMP280.h"
#include "DBG_MGR.h"
#include "RNG_MGR.h"
#include "RTC.h"
#include "SEN_MGR.h"
#include "MODE_MGR.h"
#include "SYSTICK_MGR.h"
#include "RF_MGR.h"
#include "CLI_MGR.h"


STATIC u8_t               RF_MGR_rf_frame_s[RF_MGR_MAX_PAYLOAD_SIZE];
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
    RF_MGR_rf_state_s = RF_MGR_TX;
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
	float temp      = SEN_MGR_get_temperature();

	STDC_memset( RF_MGR_rf_frame_s, 0x00, sizeof( RF_MGR_rf_frame_s ) );

	STDC_copy_32bit_to_buffer_msb_first( &RF_MGR_rf_frame_s[0u], SEN_MGR_get_sensor_id() );
	RF_MGR_rf_frame_s[4u] =  SEN_MGR_get_packet_type();
	RF_MGR_rf_frame_s[5u] =  MODE_MGR_get_operating_mode();

	STDC_memcpy( &RF_MGR_rf_frame_s[6u], &temp, 4u );

	STDC_copy_16bit_to_buffer_msb_first( &RF_MGR_rf_frame_s[10u], SEN_MGR_get_wakeup_time_sec() );
	STDC_copy_32bit_to_buffer_msb_first( &RF_MGR_rf_frame_s[12u], RTC_get_current_running_time_secs() );
	STDC_copy_16bit_to_buffer_msb_first( &RF_MGR_rf_frame_s[16u], SEN_MGR_get_batery_voltage() );
}



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
	false_true_et tx_complete = FALSE;
	
    /* Turn the RF chip on */
    HAL_BRD_set_NRF_power_pin_state( ON );

    /* The NRF chip needs 1.5 ms to wake up from off */
	SYSTICK_MGR_delay_us(1500u);

	NRF24_set_low_level_mode( NRF_TX_MODE );

	/* The NRF chip needs 130us ms to transition to tx mode */
	SYSTICK_MGR_delay_us(150u);

	/* Setup initial register values */
	if( PASS == NRF24_set_configuration( NRF24_SST_CONFIG ) )
	{
		NRF24_send_payload( RF_MGR_rf_frame_s, sizeof( RF_MGR_rf_frame_s ) );

		do
		{
			SYSTICK_MGR_delay_us( 1000 );

			if( ( NRF24_check_status_mask( RF24_TX_DATA_SENT ) == HIGH )
			 || ( NRF24_check_status_mask( RF24_MAX_RETR_REACHED ) == HIGH ) )
			{
				tx_complete = TRUE;
			}

		} while ( tx_complete == FALSE );
	}
	else
	{
		STDC_assert(1);
	}

    /* Turn the RF chip back off again */
	HAL_BRD_set_NRF_power_pin_state( OFF );
}


/***************************************************************************************************
**                              Private Functions                                                 **
***************************************************************************************************/


/****************************** END OF FILE *******************************************************/
