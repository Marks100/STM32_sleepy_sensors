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
float                BMP280_temp_s;
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
void BMP280_setup_config( BMP280_configuration_et config )
{
	u8_t len;
	u8_t i;
	BMP280_reg_config_st* data_p;

	len = BMP280_config_table_c[config].config_len;
	data_p = BMP280_config_table_c[config].dataset;

	for( i=0u; i<len; i++ )
	{
		HAL_I2C1_write_registers( BMP280_I2C_ADDR, (data_p)->reg, &(data_p)->data, 1u);
		data_p += 1;
	}
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
	
	HAL_I2C1_read_registers( BMP280_I2C_ADDR, BMP280_ID, &id, 1u );

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
	HAL_I2C1_write_registers( BMP280_I2C_ADDR, BMP280_RESET, &(u8_t){BMP280_RESET_CMD}, 1u );
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

	HAL_I2C1_read_registers( BMP280_I2C_ADDR, BMP280_STATUS, &status, 1u );

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
	u8_t calib_data[24u];

	HAL_I2C1_read_registers( BMP280_I2C_ADDR, BMP280_CALIB_START, calib_data, sizeof( calib_data ) );

	BMP280_calib_s.calib_T1 = ( ( calib_data[1u] << 8u ) | calib_data[0u] );
	BMP280_calib_s.calib_T2 = ( ( calib_data[3u] << 8u ) | calib_data[2u] );
	BMP280_calib_s.calib_T3 = ( ( calib_data[5u] << 8u ) | calib_data[4u] );

	BMP280_calib_s.calib_P1 = ( ( calib_data[7u] << 8u ) | calib_data[6u] );
	BMP280_calib_s.calib_P2 = ( ( calib_data[9u] << 8u ) | calib_data[8u] );
	BMP280_calib_s.calib_P3 = ( ( calib_data[11u] << 8u ) | calib_data[10u] );
	BMP280_calib_s.calib_P4 = ( ( calib_data[13u] << 8u ) | calib_data[12u] );
	BMP280_calib_s.calib_P5 = ( ( calib_data[15u] << 8u ) | calib_data[14u] );
	BMP280_calib_s.calib_P6 = ( ( calib_data[17u] << 8u ) | calib_data[16u] );
	BMP280_calib_s.calib_P7 = ( ( calib_data[19u] << 8u ) | calib_data[18u] );
	BMP280_calib_s.calib_P8 = ( ( calib_data[21u] << 8u ) | calib_data[20u] );
	BMP280_calib_s.calib_P9 = ( ( calib_data[23u] << 8u ) | calib_data[22u] );
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
	u8_t pressure_data[3u];
	u32_t pressure_value;

	HAL_I2C1_read_registers( BMP280_I2C_ADDR, BMP280_PRESS_MSB, pressure_data, sizeof( pressure_data ) );

	pressure_value = ( ( pressure_data[0u] << 16u ) | ( pressure_data[1u] << 8 ) | pressure_data[2u] );

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
	u8_t temperature_data[3u];
	u32_t temperature_value;

	HAL_I2C1_read_registers( BMP280_I2C_ADDR, BMP280_TEMP_MSB, temperature_data, sizeof( temperature_data ) );

	temperature_value = ( ( temperature_data[0u] << 16u ) | ( temperature_data[1u] << 8 ) | temperature_data[2u] );

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
void BMP280_convert( float* temperature, u32_t* pressure)
{
  unsigned long adc_T;
  unsigned long adc_P;

  adc_P = BMP280_read_raw_pressure_counts();
  SYSTICK_MGR_delay_us(2000u);
  adc_T = BMP280_read_raw_temp_counts();

  double var1, var2, p, t_fine;
  var1 = (((double)adc_T)/16384.0 - ((double)BMP280_calib_s.calib_T1)/1024.0) * ((double)BMP280_calib_s.calib_T2);
  var2 = ((((double)adc_T)/131072.0 - ((double)BMP280_calib_s.calib_T1)/8192.0) * (((double)adc_T)/131072.0 - ((double) BMP280_calib_s.calib_T1)/8192.0)) * ((double)BMP280_calib_s.calib_T3);
  t_fine = (var1 + var2);
  *temperature = ( t_fine /5120.0 ); 
   
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
	u16_t timeout = 0u;
	u16_t retry = 0u;

	if( BMP280_read_id() == BMP280_DEVICE_ID )
	{
		do	
		{
			SYSTICK_MGR_delay_us(4000u);

			/* Setup the config and set "forced mode" */
			BMP280_setup_config(BMP280_SST_CONFIG);

			SYSTICK_MGR_delay_us(4000u);

			BMP280_read_calib_values();

			SYSTICK_MGR_delay_us(4000u);

			/* Do the conversion */
			BMP280_convert( &BMP280_temp_s, &BMP280_pres_s );
		}
		/* This should retry the temp measurement until we get a valid result */
		while( ( FAIL == BMP280_validate_temperature( &BMP280_temp_s ) && retry < 5u ) );
	}
	else
	{
		STDC_assert(1);
		BMP280_error_s = BMP280_ID_ERR;
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
BMP280_error_type_et BMP280_get_error_status( void )
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
void BMP280_set_mode( BMP280_operating_modes_et mode )
{
  u8_t register_data = 0u;
  HAL_I2C1_read_registers( BMP280_I2C_ADDR, BMP280_CTRL_MEAS, &register_data, 1u );

  register_data &= ~( 0x03 );
  register_data |= mode;

  HAL_I2C1_write_registers( BMP280_I2C_ADDR, BMP280_CTRL_MEAS, &register_data, 1u );
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
float BMP280_get_temperature( void )
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
pass_fail_et BMP280_validate_temperature( float * val )
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
