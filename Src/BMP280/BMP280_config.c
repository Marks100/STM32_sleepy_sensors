/*! \file
*               Author: mstewart
*   \brief      BMP280 config module
*/
/***************************************************************************************************
**                              Includes                                                          **
***************************************************************************************************/
#include "BMP280_config.h"
#include "BMP280.h"


const BMP280_reg_config_st BMP280_SST_config_c[] = 
{
    { BMP280_CTRL_MEAS,  0x45 }, 
	{ BMP280_CONFIG,     0x00 },
};

const BMP280_reg_config_st BMP280_default_config_c[] = 
{
    { BMP280_CTRL_MEAS,  0x45 }, 
	{ BMP280_CONFIG,     0x00 },
};

const BMP280_config_table_st BMP280_config_table_c[] = 
{
    { BMP280_SST_config_c,     2u },
    { BMP280_default_config_c, 2u }
};


/***************************************************************************************************
**                              Private Functions                                                 **
***************************************************************************************************/


/****************************** END OF FILE *******************************************************/
