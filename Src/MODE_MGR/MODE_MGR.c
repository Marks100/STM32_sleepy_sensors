/*! \file
*               Author: mstewart
*   \brief      MODE_MGR module
*/
/***************************************************************************************************
**                              Includes                                                          **
***************************************************************************************************/
#include "stm32f10x_pwr.h"

#include "COMPILER_defs.h"
#include "C_defs.h"
#include "PROJ_config.h"
#include "STDC.h"
#include "HAL_BRD.h"
#include "HAL_ADC.h"
#include "HAL_SPI.h"
#include "HAL_I2C.h"
#include "CLI_MGR.h"
#include "MODE_MGR.h"
#include "RF_MGR.h"
#include "RNG_MGR.h"
#include "RTC.h"
#include "NVM.h"
#include "SEN_MGR.h"




/*******************************************************************************
**                      Local Data                                            **
*******************************************************************************/
STATIC MODE_MGR_mode_et MODE_MGR_mode_s;


/*!
*******************************************************************************
*
*   \brief          Init the module
*
*   \author         MS
*
*   \return         none
*
*****************************************************************************/
void MODE_MGR_init( void )
{
    /* Default the mode and then afterwards read the mode switch to update */
    MODE_MGR_mode_s = MODE_MGR_NORMAL_MODE;
}



/*!
*******************************************************************************
*
*   \brief
*
*   \author         MS
*
*   \return         none
*
*****************************************************************************/
void MODE_MGR_tick( void )
{
    if( MODE_MGR_get_operating_mode() == MODE_MGR_NORMAL_MODE )
	{
        MODE_MGR_run_activity();

		/* Enter Low power Mode */
		MODE_MGR_enter_lowpower_mode();
	}
	else
	{
		/* Handle the serial messages */
		CLI_MGR_message_handler();

		/* Check if theRTC alarm has been triggered */
		if( TRUE == RTC_get_alarm_status() )
		{
			HAL_BRD_set_onboard_led( ON );
			
            MODE_MGR_run_activity();
			
            HAL_BRD_set_onboard_led( OFF );
		
        	/* Clear the RTC alarm */
			RTC_clear_alarm();
		}
	}
}



/*!
*******************************************************************************
*
*   \brief          Sets the operating mode
*
*   \author         MS
*
*   \return         none
*
*****************************************************************************/
void MODE_MGR_set_operating_mode( MODE_MGR_mode_et mode )
{
	MODE_MGR_mode_s = mode;
}



/*!
*******************************************************************************
*
*   \brief          Gets the operating mode
*
*   \author         MS
*
*   \return         none
*
*****************************************************************************/
MODE_MGR_mode_et MODE_MGR_get_operating_mode( void )
{
	return( MODE_MGR_mode_s );
}



/*!
*******************************************************************************
*
*   \brief          Reads the opereating mode from the user input
*
*   \author         MS
*
*   \return         none
*
*****************************************************************************/
void MODE_MGR_read_operating_mode( void )
{
    low_high_et state;
	
    state = HAL_BRD_read_debug_mode_pin();

    if( state == HIGH )
    {
        MODE_MGR_set_operating_mode( MODE_MGR_DEBUG_MODE );
    }
    else
    {
        MODE_MGR_set_operating_mode( MODE_MGR_NORMAL_MODE );
    }

#if( AUTO_DEBUG_MODE == 1 )
		MODE_MGR_set_operating_mode( MODE_MGR_DEBUG_MODE );
#endif
}



/*!
*******************************************************************************
*
*   \brief          Prepares the system for power saving mode
*
*   \author         MS
*
*   \return         none
*
*****************************************************************************/
void MODE_MGR_prepare_for_lowpower_mode( void )
{
    /* Disable the clocks and peripherals to save power */
    HAL_ADC_de_init();
    HAL_I2C1_de_init();
    HAL_SPI1_de_init();
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, DISABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, DISABLE);
}




/*!
*******************************************************************************
*
*   \brief          Enters Low power mode on the sysytem
*
*   \author         MS
*
*   \return         none
*
*****************************************************************************/
void MODE_MGR_enter_lowpower_mode( void )
{
    MODE_MGR_prepare_for_lowpower_mode();

    /* Enable the wakeup pin */
    PWR_WakeUpPinCmd(ENABLE);

    /* Enters STANDBY mode */
    PWR_EnterSTANDBYMode();
}


/*!
*******************************************************************************
*
*   \brief          Carries out normal sensor duties
*
*   \author         MS
*
*   \return         none
*
*****************************************************************************/
void MODE_MGR_run_activity( void )
{
    /* Update the temperature */
	SEN_MGR_update_temperature(); 
    /* Update the battery voltage*/
    SEN_MGR_update_batery_voltage(); 
	/* Update the runtime */
	SEN_MGR_update_run_time(); 
	/* Populate all the data into the RF frame */
	RF_MGR_populate_rf_frame();
	/* Send the data */
	RF_MGR_send_rf_frame();
    /* Update NVM if required */        
    NVM_tick();
}