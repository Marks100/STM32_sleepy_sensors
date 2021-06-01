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

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "C_defs.h"
#include "STDC.h"
#include "main.h"
#include "HAL_BRD.h"
#include "NVM.h"
#include "COMPILER_defs.h"
#include "HAL_I2C.h"
#include "SEN_MGR.h"
#include "CLI_MGR.h"
#include "RTC.h"


extern NVM_info_st NVM_info_s;

/***************************************************************************************************
**                              Data declarations and definitions                                 **
***************************************************************************************************/
STATIC pass_fail_et  RTC_failure_status_s;
STATIC RTC_time_st   RTC_time_s;
STATIC false_true_et RTC_alarm_set_s;

STATIC const RTC_EXT_reg_config_st RTC_EXT_SST_config_c[] =
{
	{ Control_status_1, 0x00 },
	{ Control_status_2, 0x11 },
	{ CLKOUT_control,   0x83 },
	{ Timer_control,    0x82 },
};


STATIC const RTC_EXT_config_table_st RTC_EXT_SST_config_table_c[] =
{
	{ RTC_EXT_SST_config_c, 4u }
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
void RTC_set_configuration( RTC_EXT_configuration_et config )
{
	RTC_EXT_reg_config_st* data_p;
	u8_t len;
	u8_t i;

	len = RTC_EXT_SST_config_table_c[config].config_len;
	data_p = RTC_EXT_SST_config_table_c[config].dataset;

	for( i=0; i<len; i++ )
	{
		HAL_I2C1_write_registers( RTC_EXT_I2C_ADDRESS, data_p->reg, &data_p->data, 1u );
		data_p += 1;
	}

	RTC_set_wakeup_time( NVM_info_s.NVM_generic_data_blk_s.wakeup_period_sec );
	RTC_update_current_rtc_time();
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
	HAL_I2C1_read_registers(RTC_EXT_I2C_ADDRESS, Control_status_2, &data, 1u );

	/* Clear the inerrupt active bit */
	data &= !RTC_EXT_TIMER_INT_ACTIVE_BIT;

	/* Write the data back down again :) */
	HAL_I2C1_write_registers( RTC_EXT_I2C_ADDRESS, Control_status_2, &data, 1u );
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
	HAL_I2C1_read_registers( RTC_EXT_I2C_ADDRESS, Timer_control, &data, 1u );

	/* Clear the old values */
	data &= ~RTC_EXT_ALARM_BIT_MASK;

	if( seconds > U8_T_MAX )
	{
		/* Typically the timer is ticked at 1hz and the reg is 8bit so we can hold 255 secs here,
		If we want to go beyond that we can divide the clock for the timer by 60 giving us 60 times longer durations
		eg 255 * 60 = 15300 secs */
		data |= RTC_EXT_ALARM_1_OVER60HZ;
	
		/* Now need to work out what value to set the timer to, given that it wil now be 60 times slower,
		This does lead to some resolution issues but we will round up or down as best we can to find the 
		nearest setting that we can get so that its closest to the requested value*/
		
		/* round to nearest 60*/
		set_val = ((seconds+60)/60u);
	}
	else
	{
		/* set the correct hz bit*/
		data |= RTC_EXT_ALARM_1HZ;
		
		/* We have chosen a time value of less than 255 so we can directly use that value */
		set_val = seconds;
	}

	/* Now write down the configured settings */
	HAL_I2C1_write_registers( RTC_EXT_I2C_ADDRESS, Timer_control, &data, 1u );
	HAL_I2C1_write_registers( RTC_EXT_I2C_ADDRESS, Timer, &set_val, 1u );

	NVM_info_s.NVM_generic_data_blk_s.wakeup_period_sec = seconds;
	NVM_request_flush();
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
	u8_t data_readback[RTC_TIME_ARRAY_SIZE];
	
	HAL_I2C1_read_registers( RTC_EXT_I2C_ADDRESS, VL_seconds, data_readback, RTC_TIME_ARRAY_SIZE );

	/* Update the time local struct while we are here */
	RTC_time_s.seconds = RTC_bcd_to_int( data_readback[0u] & RTC_VL_SECS_MASK );
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
	u8_t data[RTC_TIME_ARRAY_SIZE] = { 0u, 0u, 0u, 0u, 0u, 0u, 0u };

	/* Reset the time structure */
	RTC_time_s.seconds = 0u;
	RTC_time_s.days    = 0u;
	RTC_time_s.hours   = 0u;
	RTC_time_s.days    = 0u;

	/* And write that down to the RTC */
	HAL_I2C1_write_registers( RTC_EXT_I2C_ADDRESS, VL_seconds, data, sizeof( data ) );
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
