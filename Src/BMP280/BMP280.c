/*! \file
*
*               $Author: mstewart $
*
*               $Date: 2014-01-16 15:40:40 +0000 (Thu, 16 Jan 2014) $
*
*   \brief      BMP280
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
#include "NVM.h"
#include "COMPILER_defs.h"
#include "HAL_I2C.h"
#include "BMP280.h"



extern NVM_info_st NVM_info_s;

BMP280_calib_st BMP280_calib_s;

/***************************************************************************************************
**                              Data declarations and definitions                                 **
***************************************************************************************************/
/* None */


void BMP280_init( void )
{
	u8_t register_data = BMP280_MEAS_BIT_MASK;

	STDC_memcpy( &BMP280_calib_s, 0x00, sizeof( BMP280_calib_s ) );

	BMP280_read_id();


	HAL_I2C_write_single_register( BMP280_I2C_ADDR, BMP280_CTRL_MEAS, &register_data );


	register_data = BMP280_CONFIG_BIT_MASK;

	HAL_I2C_write_single_register( BMP280_I2C_ADDR, BMP280_CONFIG, &register_data );
}


u8_t BMP280_read_id( void )
{
	u8_t id;

	HAL_I2C_read_register( BMP280_I2C_ADDR, BMP280_ID, &id );

	return ( id );
}


void BMP280_reset( void )
{
	u8_t register_data = BMP280_RESET_CMD;

	HAL_I2C_write_single_register( BMP280_I2C_ADDR, BMP280_RESET, &register_data );
}


u8_t BMP280_read_status( void )
{
	u8_t status;

	HAL_I2C_read_register( BMP280_I2C_ADDR, BMP280_STATUS, &status );

	return( status );
}



///****************************** END OF FILE *******************************************************/
