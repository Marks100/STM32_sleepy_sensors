/*! \file
*               Author: mstewart
*   \brief      VER module
*/
/***************************************************************************************************
**                              Includes                                                          **
***************************************************************************************************/
#include "C_defs.h"
#include "STDC.h"
#include "VER.h"
#include "autoversion.h"



/***************************************************************************************************
**                              Data declarations and definitions                                 **
***************************************************************************************************/
VER_info_st VER_info_s;
VER_build_date_st VER_build_date_s;

void VER_init( void )
{
    VER_info_s.sw.major = SLEEPY_SENSOR_VERSION_MAJOR;
    VER_info_s.sw.minor = SLEEPY_SENSOR_VERSION_PATCH;
    VER_info_s.sw.patch = SLEEPY_SENSOR_VERSION_VERIFICATION;

    VER_info_s.hw.major = HW_VERSION_MAJOR;
    VER_info_s.hw.minor = HW_VERSION_MINOR;

    VER_build_date_s.date  = SLEEPY_SENSOR_BUILD_DATE;
    VER_build_date_s.month = SLEEPY_SENSOR_BUILD_MONTH;
    VER_build_date_s.year  = SLEEPY_SENSOR_BUILD_YEAR;
}


void VER_get_sw_version_num( u8_t * data_p )
{
    data_p[0] = VER_info_s.sw.major;
    data_p[1] = VER_info_s.sw.minor;
    data_p[1] = VER_info_s.sw.patch;
}

void VER_get_hw_version_num( u8_t * data_p )
{
    data_p[0] = VER_info_s.hw.major;
    data_p[1] = VER_info_s.hw.minor;
}

void VER_get_build_date( u8_t * data_p )
{
    data_p[0] = VER_build_date_s.date;
    data_p[1] = VER_build_date_s.month;
    data_p[2] = VER_build_date_s.year;
}





///****************************** END OF FILE *******************************************************/