/*! \file
*               Author: mstewart
*   \brief      SEN_MGR: Sensor Manager module
*/
/***************************************************************************************************
**                              Includes                                                          **
***************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#include "STDC.h"
#include "C_defs.h"
#include "NVM.h"
#include "HAL_ADC.h"
#include "BMP280.h"
#include "RTC.h"
#include "UID.h"
#include "RNG_MGR.h"
#include "SEN_MGR.h"



/***************************************************************************************************
**                              Data declarations and definitions                                 **
***************************************************************************************************/
SEN_MGR_data_st SEN_MGR_data_s;
 

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
void SEN_MGR_init( void )
{
	SEN_MGR_update_sensor_id();

	SEN_MGR_data_s.sensor_type              = SEN_MGR_SST_SEN_TYPE;
	SEN_MGR_data_s.packet_type              = 5u;
	SEN_MGR_data_s.wakeup_period_sec        = NVM_info_s.NVM_generic_data_blk_s.wakeup_period_sec;
	SEN_MGR_data_s.runtime_secs             = 0u;
	SEN_MGR_data_s.temperature              = 0;

	SEN_MGR_update_stored_params();
}



/*!
****************************************************************************************************
*
*   \brief         
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
void SEN_MGR_update_sensor_id( void )
{
	u32_t id;

	id = NVM_info_s.NVM_generic_data_blk_s.sensor_id;

	/* If we have manually set an id up use that */
	if( ( id != 0xFFFFFFFF ) && ( id != 0u ) )
	{
		SEN_MGR_data_s.sensor_id = id;
	}
	else
	{
		/* Setup an ID if we havent manually set 1 */
		SEN_MGR_data_s.sensor_id = UID_get_unique_id_32();
	}
}



/*!
****************************************************************************************************
*
*   \brief         Sets the sensor id ( manually )  
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
pass_fail_et SEN_MGR_set_sensor_id( u32_t id )
{
	pass_fail_et status = FAIL;

	if( ( id > 0u ) && ( id < U32_T_MAX ) )
	{
		SEN_MGR_data_s.sensor_id = id;
		SEN_MGR_update_stored_params();
		
		status = PASS;
	}
	return ( status );
}


/*!
****************************************************************************************************
*
*   \brief         Update the stored params in memory
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
void SEN_MGR_update_stored_params( void )
{
	NVM_info_s.NVM_generic_data_blk_s.sensor_id   = SEN_MGR_data_s.sensor_id;
	NVM_info_s.NVM_generic_data_blk_s.sensor_type = SEN_MGR_data_s.sensor_type;

	NVM_info_s.NVM_generic_data_blk_s.wakeup_period_sec = SEN_MGR_data_s.wakeup_period_sec;

	NVM_request_flush();
}



/*!
****************************************************************************************************
*
*   \brief         Set the sensor type
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
pass_fail_et SEN_MGR_set_sensor_type( u8_t sensor_type )
{
	pass_fail_et status = FAIL;

	if( sensor_type != 0u )
	{
		SEN_MGR_data_s.sensor_type = sensor_type;
		SEN_MGR_update_stored_params();
		status = PASS;
	}
	return( status );
}


/*!
****************************************************************************************************
*
*   \brief         Get the sensor type
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
u8_t SEN_MGR_get_sensor_type( void )
{
	return ( SEN_MGR_data_s.sensor_type );
}





/*!
****************************************************************************************************
*
*   \brief         Get the sensor ID
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
u32_t SEN_MGR_get_sensor_id( void )
{
	return( SEN_MGR_data_s.sensor_id );
}



/*!
****************************************************************************************************
*
*   \brief         Set the wakeup time of the sensor
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
void SEN_MGR_set_wakeup_time_sec( u16_t wakeup_time )
{
	/* Boundary check the wakeup time ( high and low ) */
	if( wakeup_time > SEN_MGR_MAX_WAKEUP_TIME_SEC )
	{
		wakeup_time = SEN_MGR_MAX_WAKEUP_TIME_SEC;
	}

	if( wakeup_time < 1u )
	{
		wakeup_time = 1u;
	}

	/* Now assign the time */
	SEN_MGR_data_s.wakeup_period_sec = wakeup_time;

	SEN_MGR_update_stored_params();
}



/*!
****************************************************************************************************
*
*   \brief         Get the wakeup time of the sensor
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
u16_t SEN_MGR_get_wakeup_time_sec( void )
{
	return( SEN_MGR_data_s.wakeup_period_sec );
}



/*!
****************************************************************************************************
*
*   \brief         Get the desired packet type of the sensor
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
u8_t SEN_MGR_get_packet_type( void )
{
	/* Just incase 1 sensor is capable of multiple different types of rf packet */
	return( SEN_MGR_data_s.packet_type );
}



/*!
****************************************************************************************************
*
*   \brief         Updates the runtime of the sensor
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
void  SEN_MGR_update_run_time( void )
{
	RTC_update_current_rtc_time();

	/* Work out how long in seconds this sensor has been powerd since it was reset */
	SEN_MGR_data_s.runtime_secs = RTC_get_current_running_time_secs();
}



/*!
****************************************************************************************************
*
*   \brief         Updates the read temperature of the sensor
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
void SEN_MGR_update_temperature( void )
{
	/* Trigger the sensor to read temperature */
	BMP280_trigger_meas();

	SEN_MGR_data_s.temperature = BMP280_get_temperature();
}



/*!
****************************************************************************************************
*
*   \brief         Updates the battery voltage of the sensor
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
void SEN_MGR_update_batery_voltage( void )
{
	SEN_MGR_data_s.battery_mv = HAL_ADC_measure_batt_voltage();
}


/*!
****************************************************************************************************
*
*   \brief         Returns the battery voltage of the sensor
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
u16_t SEN_MGR_get_batery_voltage( void )
{
	return( SEN_MGR_data_s.battery_mv );
}
	


/*!
****************************************************************************************************
*
*   \brief         Gets the temperature
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
float SEN_MGR_get_temperature( void )
{
	return( SEN_MGR_data_s.temperature );
}





	




/***************************************************************************************************
**                              Private Functions                                                 **
***************************************************************************************************/


/****************************** END OF FILE *******************************************************/
