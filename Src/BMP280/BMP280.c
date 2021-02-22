/*! \file
*               Author: mstewart
*   \brief      BMP280 module
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
#include "HAL_BRD.h"
#include "HAL_I2C.h"
#include "SYSTICK_MGR.h"
#include "BMP280.h"




/***************************************************************************************************
**                              Data declarations and definitions                                 **
***************************************************************************************************/
BMP280_calib_st      BMP280_calib_s;
s32_t                BMP280_pres_s;
s32_t                BMP280_temp_s;
BMP280_error_type_et BMP280_error_s;




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
void BMP280_init( void )
{
	BMP280_pres_s = 0;
	BMP280_temp_s = 0;
	BMP280_error_s = BMP280_NO_ERR;
	STDC_memset( &BMP280_calib_s, 0x00u, sizeof( BMP280_calib_s ) );
}



/*!
****************************************************************************************************
*
*   \brief         Configure the BMP280 sensor
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
void BMP280_setup_default_config( void )
{
	u8_t register_data;
	u8_t data_readback;

	register_data = BMP280_MEAS_BIT_MASK;
	HAL_I2C1_write_single_register( BMP280_I2C_ADDR, BMP280_CTRL_MEAS, &register_data );

	register_data = BMP280_CONFIG_BIT_MASK;
	HAL_I2C1_write_single_register( BMP280_I2C_ADDR, BMP280_CONFIG, &register_data );

	HAL_I2C1_read_single_register( BMP280_I2C_ADDR, BMP280_CONFIG,    &data_readback );
	HAL_I2C1_read_single_register( BMP280_I2C_ADDR, BMP280_CTRL_MEAS, &data_readback );
	HAL_I2C1_read_single_register( BMP280_I2C_ADDR, BMP280_STATUS,    &data_readback );

	BMP280_read_calib_values();
}




/*!
****************************************************************************************************
*
*   \brief         Read the ID of the BMP280 sensor
*
*   \author        MS
*
*   \return        u8_t ID
*
*   \note
*
***************************************************************************************************/
u8_t BMP280_read_id( void )
{
	u8_t id;
	
	HAL_I2C1_read_single_register( BMP280_I2C_ADDR, BMP280_ID, &id );

	return ( id );
}



/*!
****************************************************************************************************
*
*   \brief         Reset the BMP280 sensor
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
void BMP280_reset( void )
{
	u8_t register_data = BMP280_RESET_CMD;

	HAL_I2C1_write_single_register( BMP280_I2C_ADDR, BMP280_RESET, &register_data );
}



/*!
****************************************************************************************************
*
*   \brief         Read the status of the BMP280 sensor
*
*   \author        MS
*
*   \return        u8_t status
*
*   \note
*
***************************************************************************************************/
u8_t BMP280_read_status( void )
{
	u8_t status;

	HAL_I2C1_read_single_register( BMP280_I2C_ADDR, BMP280_STATUS, &status );

	return( status );
}



/*!
****************************************************************************************************
*
*   \brief         Read the calibration values of the BMP280 sensor
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
void BMP280_read_calib_values( void )
{
	u8_t calib_data[24];

	HAL_I2C1_read_multiple_registers( BMP280_I2C_ADDR, BMP280_CALIB_START, calib_data, sizeof( calib_data ) );

	BMP280_calib_s.calib_T1 = ( ( calib_data[1] << 8u ) | calib_data[0] );
	BMP280_calib_s.calib_T2 = ( ( calib_data[3] << 8u ) | calib_data[2] );
	BMP280_calib_s.calib_T3 = ( ( calib_data[5] << 8u ) | calib_data[4] );

	BMP280_calib_s.calib_P1 = ( ( calib_data[7] << 8u ) | calib_data[6] );
	BMP280_calib_s.calib_P2 = ( ( calib_data[9] << 8u ) | calib_data[8] );
	BMP280_calib_s.calib_P3 = ( ( calib_data[11] << 8u ) | calib_data[10] );
	BMP280_calib_s.calib_P4 = ( ( calib_data[13] << 8u ) | calib_data[12] );
	BMP280_calib_s.calib_P5 = ( ( calib_data[15] << 8u ) | calib_data[14] );
	BMP280_calib_s.calib_P6 = ( ( calib_data[17] << 8u ) | calib_data[16] );
	BMP280_calib_s.calib_P7 = ( ( calib_data[19] << 8u ) | calib_data[18] );
	BMP280_calib_s.calib_P8 = ( ( calib_data[21] << 8u ) | calib_data[20] );
	BMP280_calib_s.calib_P9 = ( ( calib_data[23] << 8u ) | calib_data[22] );
}



/*!
****************************************************************************************************
*
*   \brief         Read the raw counts for pressure of the BMP280 sensor
*
*   \author        MS
*
*   \return        u32_t raw counts for pressure and 
*
*   \note
*
***************************************************************************************************/
u32_t BMP280_read_raw_pressure_counts( void )
{
	u8_t pressure_data[3];
	u32_t pressure_value;

	HAL_I2C1_read_multiple_registers( BMP280_I2C_ADDR, BMP280_PRESS_MSB, pressure_data, sizeof( pressure_data ) );

	pressure_value = ( ( pressure_data[0] << 16u ) | ( pressure_data[1] << 8 ) | pressure_data[2] );

	return ( pressure_value >> 4u );
}



/*!
****************************************************************************************************
*
*   \brief         Read the raw counts for temperature of the BMP280 sensor
*
*   \author        MS
*
*   \return        u32_t raw counts for temperature and 
*
*   \note
*
***************************************************************************************************/
u32_t BMP280_read_raw_temp_counts( void )
{
	u8_t temperature_data[3];
	u32_t temperature_value;

	HAL_I2C1_read_multiple_registers( BMP280_I2C_ADDR, BMP280_TEMP_MSB, temperature_data, sizeof( temperature_data ) );

	temperature_value = ( ( temperature_data[0] << 16u ) | ( temperature_data[1] << 8 ) | temperature_data[2] );

	return ( temperature_value >> 4u );
}




/*!
****************************************************************************************************
*
*   \brief         Run the conversion algorithm
*
*   \author        MS
*
*   \return        none
*
*   \note		   Formula copied froman online resource
*
***************************************************************************************************/
void BMP280_convert( u32_t* temperature, u32_t* pressure)
{
  unsigned long adc_T;
  unsigned long adc_P;

  adc_P = BMP280_read_raw_pressure_counts();
  adc_T = BMP280_read_raw_temp_counts();

  double var1, var2, p, t_fine;
  var1 = (((double)adc_T)/16384.0 - ((double)BMP280_calib_s.calib_T1)/1024.0) * ((double)BMP280_calib_s.calib_T2);
  var2 = ((((double)adc_T)/131072.0 - ((double)BMP280_calib_s.calib_T1)/8192.0) * (((double)adc_T)/131072.0 - ((double) BMP280_calib_s.calib_T1)/8192.0)) * ((double)BMP280_calib_s.calib_T3);
  t_fine = (var1 + var2);
  *temperature = (int32_t) (t_fine*10 / 5120.0);

  var1 = ((double)t_fine/2.0) - 64000.0;
  var2 = var1 * var1 * ((double)BMP280_calib_s.calib_P6) / 32768.0;
  var2 = var2 + var1 * ((double)BMP280_calib_s.calib_P5) * 2.0;
  var2 = (var2/4.0)+(((double)BMP280_calib_s.calib_P4) * 65536.0);
  var1 = (((double)BMP280_calib_s.calib_P3) * var1 * var1 / 524288.0 + ((double)BMP280_calib_s.calib_P2) * var1) / 524288.0;
  var1 = (1.0 + var1 / 32768.0)*((double)BMP280_calib_s.calib_P1);
  if (var1 == 0.0)
  {
    return; // avoid exception caused by division by zero
  }
  p = 1048576.0 - (double)adc_P;
  p = (p - (var2 / 4096.0)) * 6250.0 / var1;
  var1 = ((double)BMP280_calib_s.calib_P9) * p * p / 2147483648.0;
  var2 = p * ((double)BMP280_calib_s.calib_P8) / 32768.0;
  p = (p + (var1 + var2 + ((double)BMP280_calib_s.calib_P7)) / 16.0);

  *pressure = (long)p;
}




/*!
****************************************************************************************************
*
*   \brief         Trigger the measurement of the BMP280 sensor
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
void BMP280_trigger_meas( void )
{
	u8_t  status = 0u;
	u8_t  id = 0u;
	u16_t timeout = 0u;
	u16_t retry = 0u;

	/* Try and read the id of the sensor so that we know it is operational */
	id = BMP280_read_id();

 	if( id != BMP280_DEVICE_ID )
	{
		do
		{
			/* Delay a litle and have another go */
			SYSTICK_MGR_delay_us( 5000u );
			id = BMP280_read_id();
			timeout++;
		} 
		while ( ( id != BMP280_DEVICE_ID ) && ( timeout < BMP280_READ_TIMEOUT ) );
	}	

	if( timeout == BMP280_READ_TIMEOUT )
	{
		/* We cant even read the ID so somethig is wrong */
		STDC_assert(1);
		BMP280_error_s = BMP280_ID_ERR;
	}
	else
	{
		do
		{
			/* Setup the config and set "forced mode" */
			BMP280_setup_default_config();
			BMP280_set_mode( BMP280_FORCED_MODE );

			/* Reset the timeout counter */
			SYSTICK_MGR_delay_us( 20000u );
			timeout = 0u;

			status = BMP280_read_status();

			if( ( status & BMP280_MEASURING ) == BMP280_MEASURING )
			{
				do
				{
					SYSTICK_MGR_delay_us( 1000u );
					status = BMP280_read_status();
					timeout++;
				}
				while( ( ( status & BMP280_MEASURING ) == BMP280_MEASURING ) && ( timeout < BMP280_READ_TIMEOUT ) );
			}

			if( timeout == BMP280_READ_TIMEOUT )
			{
				/* The status is wrong */
				STDC_assert(1);
				BMP280_error_s = BMP280_STATUS_ERR;
			}
			else
			{
				/* Do the conversion */
				BMP280_convert( &BMP280_temp_s, &BMP280_pres_s );
			}
			retry++;
		}
		/* This should retry the temp measurement until we get a valid result */
		while( 	( FAIL == BMP280_validate_temperature( &BMP280_temp_s ) && retry < 5u ) );

		/* Back to sleep mode, this is probably done automatically by the sensor */
		BMP280_set_mode( BMP280_SLEEP_MODE );
	}
}




/*!
****************************************************************************************************
*
*   \brief         Grabs the error of the BMP280 sensor
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
BMP280_error_type_et BMP280_get_error_status ( void )
{
	return( BMP280_error_s );
}




/*!
****************************************************************************************************
*
*   \brief         Sets the mode of the BMP280 sensor
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
void BMP280_set_mode ( BMP280_operating_modes_et mode )
{
  u8_t register_data = 0u;
  HAL_I2C1_read_single_register( BMP280_I2C_ADDR, BMP280_CTRL_MEAS, &register_data );

  register_data &= ~( 0x03 );
  register_data |= mode;

  HAL_I2C1_write_single_register( BMP280_I2C_ADDR, BMP280_CTRL_MEAS, &register_data );
}




/*!
****************************************************************************************************
*
*   \brief         API to get the converted temperature
*
*   \author        MS
*
*   \return        s32_t Actual temperatureraw counts for pressure
*
*   \note
*
***************************************************************************************************/
s32_t BMP280_get_temperature( void )
{
	return ( BMP280_temp_s );
}




/*!
****************************************************************************************************
*
*   \brief         Validates the sensor reading
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
pass_fail_et BMP280_validate_temperature( s32_t * val )
{
	pass_fail_et result = FAIL;
	BMP280_error_s = BMP280_RANGE_ERR;

	if( *val < BMP280_TEMP_MIN )
	{
		STDC_assert(1);
		
	}
	else if( *val > BMP280_TEMP_MAX )
	{
		STDC_assert(1);
	}
	else
	{
		/* Leave as is */
		BMP280_error_s =  BMP280_NO_ERR;
		result = PASS;
	}

	return( result );
}


///****************************** END OF FILE *******************************************************/
