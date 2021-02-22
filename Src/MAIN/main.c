/* STM32 specific includes */
#include "misc.h"
#include "stm32f10x_pwr.h"

#include "C_defs.h"
#include "PROJ_config.h"
#include "HAL_BRD.h"
#include "HAL_ADC.h"
#include "HAL_SPI.h"
#include "HAL_I2C.h"
#include "HAL_UART.h"
#include "RTC.h"
#include "NRF24.h"
#include "BMP280.h"
#include "NVM.h"
#include "MODE_MGR.h"
#include "CLI_MGR.h"
#include "SYSTICK_MGR.h"
#include "RNG_MGR.h"
#include "RF_MGR.h"
#include "DBG_LOG_MGR.h"
#include "SEN_MGR.h"
#include "VER.h"
#include "MAIN.h"


int main(void)
{
	RCC_ClocksTypeDef RCC_Clocks;
	
	RCC_DeInit();
	SystemInit();

	RCC_GetClocksFreq (&RCC_Clocks);
	RCC_HCLKConfig(RCC_SYSCLK_Div1);
	RCC_PCLK1Config(RCC_HCLK_Div1);
	RCC_PCLK2Config(RCC_HCLK_Div1);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

	/* Important that these modules get initialised early */
	SYSTICK_MGR_init();
	MODE_MGR_init();

	/* Init the HW */ 
	HAL_BRD_init();
	HAL_I2C1_init();
	HAL_SPI1_init();
	HAL_ADC_init();
	NVM_init();

	/* Init the rest of the Modules */
	RTC_init();
	SEN_MGR_init();	
	RNG_MGR_init();
	RF_MGR_init();
	BMP280_init();
	
	/* Write the default config down to the RTC */
	RTC_setup_default_config();

	if( MODE_MGR_get_operating_mode() == MODE_MGR_DEBUG_MODE )
	{
		/* In debug mode lets init the debug uart */
		HAL_USART1_init();
		CLI_MGR_init();
		CLI_MGR_display_startup_message();
		HAL_BRD_set_onboard_led( ON );
	}

	while (1)
	{
		if( MODE_MGR_get_operating_mode() == MODE_MGR_NORMAL_MODE )
		{
			/* Update the temperature */
			SEN_MGR_update_temperature();

			/* Populate all the data into the RF frame */
			RF_MGR_populate_rf_frame();

			/* Send the data */
			RF_MGR_send_rf_frame();

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
				/* Update the temperature */
				SEN_MGR_update_temperature();

				/* Update the runtime */
				SEN_MGR_update_run_time();
				
				/* Populate all the data into the RF frame */
				RF_MGR_populate_rf_frame();

				/* Send the data */
				RF_MGR_send_rf_frame();

				/* Clear the RTC alarm */
				RTC_clear_alarm();
			}
		}
	}
}

