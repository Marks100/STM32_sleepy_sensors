/*! \file
*               Author: mstewart
*   \brief      RTC module
*/
/***************************************************************************************************
**                              Includes                                                          **
***************************************************************************************************/
#include "stm32f10x_rcc.h"
#include "stm32f10x_i2c.h"
#include "stm32f10x_gpio.h"
#include "misc.h"

#include "C_defs.h"
#include "STDC.h"
#include "main.h"
#include "HAL_BRD.h"
#include "NVM.h"
#include "COMPILER_defs.h"
#include "HAL_I2C.h"
#include "SEN_MGR.h"
#include "RTC.h"


extern NVM_info_st NVM_info_s;

/***************************************************************************************************
**                              Data declarations and definitions                                 **
***************************************************************************************************/
STATIC pass_fail_et  RTC_failure_status_s;
STATIC RTC_time_st   RTC_time_s;
STATIC false_true_et RTC_alarm_set_s;

STATIC const u8_t RTC_EXT_default_register_values[ RTC_EXT_MAX_NUM_REGS ] =
{
	0x00,	//Control_status_1
	0x11,	//Control_status_2
	0x00,	//VL_seconds
	0x00,	//Minutes
	0x00,	//Hours
	0x00,	//Days
	0x00,	//Weekdays
	0x00,	//Century_months
	0x00,	//Years
	0x00,	//Minute_alarm
	0x00,	//Hour_alarm
	0x00,	//Day_alarm
	0x00,	//Weekday_alarm
	0x83,	//CLKOUT_control
	0x82,	//Timer_control
	SEN_MGR_DEFAULT_WAKEUP_TIME_SEC
};



/*!
****************************************************************************************************
*
*   \brief         Init the RTC module
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
void RTC_init( void )
{
	/* Alarm is not set yet */
	RTC_alarm_set_s = FALSE;
	STDC_memset( &RTC_time_s, 0x00u, sizeof( RTC_time_st ));
	RTC_failure_status_s = PASS;
}


/*!
****************************************************************************************************
*
*   \brief         Setup the default config of the RTC chip
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
void RTC_setup_default_config( void )
{
	u8_t data_readback[RTC_EXT_MAX_NUM_REGS];

	/* Read back the current register data */
	HAL_I2C1_read_multiple_registers( RTC_EXT_I2C_ADDRESS, Control_status_1, data_readback, sizeof( data_readback ) );

	/* Grab the value for current time on the RTC ( in BCD ) as we will write this down
	 again after the configuration has been set */
	RTC_time_s.seconds = RTC_bcd_to_int( data_readback[VL_seconds] );
	RTC_time_s.minutes = RTC_bcd_to_int( data_readback[Minutes] );
	RTC_time_s.hours   = RTC_bcd_to_int( data_readback[Hours] );
	RTC_time_s.days    = RTC_bcd_to_int( data_readback[Days] );
	RTC_time_s.years   = RTC_bcd_to_int( data_readback[Years] );
	
	/* Now write down the default config */
	HAL_I2C1_write_multiple_registers( RTC_EXT_I2C_ADDRESS, Control_status_1, (u8_t*)RTC_EXT_default_register_values, sizeof( RTC_EXT_default_register_values ) );

	/* Now write the time value down again */
	HAL_I2C1_write_multiple_registers( RTC_EXT_I2C_ADDRESS, VL_seconds, &RTC_time_s.seconds, sizeof( RTC_time_st ) );

	/* Now set the wakeup alarm with the currently stored NVM value */
	RTC_set_wakeup_time( SEN_MGR_get_wakeup_time_sec() );
}






/*!
****************************************************************************************************
*
*   \brief         Sets the RTC clk output on or off
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
void RTC_set_clk_output( disable_enable_et state, RTC_clk_out_setting_et setting )
{
}



/*!
****************************************************************************************************
*
*   \brief         Clears the TRC external interrupt
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
void RTC_ext_clear_int( void )
{
	u8_t data;

	/* Read the register first to get the old value */
	HAL_I2C1_read_single_register(RTC_EXT_I2C_ADDRESS, Control_status_2, &data );

	/* Clear the inerrupt active bit */
	data &= !RTC_EXT_TIMER_INT_ACTIVE_BIT;

	/* Write the data back down again :) */
	HAL_I2C1_write_single_register( RTC_EXT_I2C_ADDRESS, Control_status_2, &data );
}



/*!
****************************************************************************************************
*
*   \brief         Sets the next wakeup time of the RTC
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
void RTC_set_wakeup_time( u32_t seconds )
{
	u8_t data;
	u8_t set_val;

	/* Read the register first to get the old value */
	HAL_I2C1_read_single_register( RTC_EXT_I2C_ADDRESS, Timer_control, &data );

	/* The 1Hz bit is always set for this appication, this gives us wakeup times of up to 255 secs,
	if  we want longer than this we divide the clock by 60 to give us 60 times longer */
	data |= RTC_EXT_ALARM_1HZ_BIT;

	if( seconds > U8_T_MAX )
	{
		/* We have selected a wakeup timer of greater than 60 secs.., so we will set the 1/60Hz
		flag and this will slow the clock by a factor of 60 */
		data |= RTC_EXT_ALARM_1_OVER60HZ_BIT;
	
		/* Now need to work out what value to set the timer to, given that it wil now be 60 times slower,
		This does lead to some resolution issues but we will round up or down as best we can to find the 
		nearest setting that we can get so that its closest to the requested value*/
		set_val = (seconds+(RTC_EXT_ALARM_1_OVER60HZ_VAL/2u)/RTC_EXT_ALARM_1_OVER60HZ_VAL);
	}
	else
	{
		/* We have chosen a time value of less than 255 so we can directly use that setting */
		set_val = seconds;

		/* set the correct hz bit*/
		data &= ~RTC_EXT_ALARM_1_OVER60HZ_BIT;
	}

	/* Now write down the configured settings */
	HAL_I2C1_write_single_register( RTC_EXT_I2C_ADDRESS, Timer_control, &data );
	HAL_I2C1_write_single_register( RTC_EXT_I2C_ADDRESS, Timer, &set_val );
}




/*!
****************************************************************************************************
*
*   \brief         Updates the current RTC time
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
void RTC_update_current_rtc_time( void )
{
	u32_t run_time = 0u;
	u8_t data_readback[RTC_TIME_ARRAY_SIZE];
	
	HAL_I2C1_read_multiple_registers( RTC_EXT_I2C_ADDRESS, VL_seconds, data_readback, RTC_TIME_ARRAY_SIZE );

	/* Update the time local struct while we are here */
	RTC_time_s.seconds = RTC_bcd_to_int( data_readback[0u] );
	RTC_time_s.minutes = RTC_bcd_to_int( data_readback[1u] );
	RTC_time_s.hours   = RTC_bcd_to_int( data_readback[2u] );
	RTC_time_s.days    = RTC_bcd_to_int( data_readback[3u] );
	RTC_time_s.years   = RTC_bcd_to_int( data_readback[6u] );
}



/*!
****************************************************************************************************
*
*   \brief         Grabs the current number of consecutive seconds the RTC has 
*				   been running for
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
u32_t RTC_get_current_running_time_secs( void )
{
	u32_t run_time;

	/* Now convert to total time in secs */
	run_time =  ( RTC_time_s.seconds                         );  /* seconds first */
	run_time += ( RTC_time_s.minutes * RTC_SECONDS_PER_MIN   );  /* Now mins so * 60u */
	run_time += ( RTC_time_s.hours   * RTC_SECONDS_PER_HOUR  );  /* Now hrs so * 3600u */
	run_time += ( RTC_time_s.days    * RTC_SECONDS_PER_DAY   );  /* Now days so * 86400u */
	run_time += ( RTC_time_s.years   * RTC_SECONDS_PER_YEAR  );  /* Now years so * 31536000u */

	return( run_time );
}


/*!
****************************************************************************************************
*
*   \brief         Returns any failures of the RTC
*
*   \author        MS
*
*   \return        none
*
*   \note		   pass_fail_et
*
***************************************************************************************************/
pass_fail_et RTC_get_failure_status( void )
{
	return( RTC_failure_status_s );
}


/*!
****************************************************************************************************
*
*   \brief         Resets the date and time of the RTC
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
void RTC_reset_date_and_time( void )
{
	/* Reset the structure to 0 */
	RTC_time_s.seconds = 0u;
	RTC_time_s.days    = 0u;
	RTC_time_s.hours   = 0u;
	RTC_time_s.days    = 0u;

	/* And write that doen to the RTC */
	HAL_I2C1_write_multiple_registers( RTC_EXT_I2C_ADDRESS, VL_seconds, &RTC_time_s.seconds, 4u );
}




/*!
****************************************************************************************************
*
*   \brief         Converts the RTC BCD to decimal
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
u8_t RTC_bcd_to_int( u8_t input )
{
	u8_t result;
	u8_t frac_1;
	u8_t frac_2;

	frac_1 = (( input & 0xF0 ) >> 4u );
	frac_2 = (  input & 0x0F );

	result = ( ( frac_1 * 10u ) + frac_2 );

	return( result );
}


/*!
****************************************************************************************************
*
*   \brief         RTC alarm triggered
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
void RTC_alarm_triggered( void )
{
	RTC_alarm_set_s = TRUE;
}




/*!
****************************************************************************************************
*
*   \brief         Clears the RTC alarm
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
void RTC_clear_alarm( void )
{
	RTC_alarm_set_s = FALSE;
}


	/*!
****************************************************************************************************
*
*   \brief         Returns the status of the RTC alarm
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
false_true_et RTC_get_alarm_status( void )
{
	return( RTC_alarm_set_s );
}


///****************************** END OF FILE *******************************************************/
