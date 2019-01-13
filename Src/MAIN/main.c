/* STM32 specific includes */
#ifndef UNIT_TEST
	#include "misc.h"
    #include "stm32f10x.h"
    #include "stm32f10x_rcc.h"
    #include "stm32f10x_pwr.h"
	#include "stm32f10x_i2c.h"
#endif

#include "C_defs.h"
#include "PROJ_config.h"
#include "COMPILER_defs.h"
#include "COMPILER_config.h"
#include "HAL_BRD.h"
#include "HAL_ADC.h"
#include "HAL_SPI.h"
#include "NVM.h"
#include "RTC.h"
#include "HAL_I2C.h"
#include "HAL_UART.h"
#include "NRF24.h"
#include "main.h"


u16_t delay_timer = 0u;
u32_t ctr = 0u;
u8_t  NRF24_register_readback_s[NRF24_DEFAULT_CONFIGURATION_SIZE];
u8_t  data_to_send_s[10];
u8_t  time_array_s[RTC_TIME_ARRAY_SIZE];
u8_t  old_val = 0u;


int main(void)
{
	ctr = 0;

	RCC_DeInit();
	SystemInit();

	RCC_HCLKConfig(RCC_SYSCLK_Div1);
	RCC_PCLK1Config(RCC_HCLK_Div1);
	RCC_PCLK2Config(RCC_HCLK_Div1);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

	/* Init the HW */
	HAL_BRD_init();
	HAL_I2C_init();
	HAL_SPI_init();
	NVM_init();

	/* Initialise the RTC */
	RTC_ext_init();

	/* Initialise the NRF24 variables */
	NRF24_init();

	HAL_BRD_set_NRF_power_pin_state( OFF );

	if( debug_mode == ENABLE )
	{
		/* In debug mode lets init the debug usart as this consumes lots of power */
		//SERIAL_init();
	}

	while (1)
	{
		if( debug_mode != ENABLE )
		{
			populate_rf_frame();

			/* Send the data */
			NRF_simple_send( data_to_send_s, sizeof( data_to_send_s ), 1u );

			/* Disable the I2C peripheral and clock to save power */
			HAL_I2C_de_init();

			/* Disable the SPI peripheral and clock to save power */
			HAL_SPI_de_init();

			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, DISABLE);
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, DISABLE);

			/* Enable the wakeup pin */
			PWR_WakeUpPinCmd(ENABLE);

			/* Enters STANDBY mode */
			PWR_EnterSTANDBYMode();
		}
		else
		{
			/* Handle the serial messages */
			//SERIAL_msg_handler();

			if( HAL_BRD_get_rtc_trigger_status() == TRUE )
			{
				populate_rf_frame();

				NRF_simple_send( data_to_send_s, sizeof( data_to_send_s ), 1u );

				HAL_BRD_toggle_led();

				/* Set the trigger back to false */
				HAL_BRD_set_rtc_trigger_status( FALSE );
			}
		}

		if( ctr >= U32_T_MAX)
		{
			ctr = 0u;
		}
		else
		{
			ctr ++;
		}
	}
}




/* ISR for systick handler */
void SysTick_Handler( void )
{
	delay_timer--;
}


u32_t get_counter( void )
{
	return ( ctr );
}


void MAIN_SYSTICK_init( void )
{
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq (&RCC_Clocks);

	SysTick_CLKSourceConfig( SysTick_CLKSource_HCLK );

	/* Trigger an interrupt every 1ms */
	SysTick_Config(72000);
}




u8_t generate_random_number( void )
{
	u8_t randomiser;
	u8_t random_number;

	RTC_grab_current_running_time( time_array_s );
	randomiser =  time_array_s[0];

    /* Set the seed */
	srand( randomiser );

    /* Grab the now "random :)" number */
    random_number = rand()%100;

	return ( random_number );
}


void populate_rf_frame( void )
{
	data_to_send_s[0] = generate_random_number();
	data_to_send_s[1] = SENSOR_ID;
	data_to_send_s[2] = SENSOR_TYPE;

	if( old_val == data_to_send_s[0] )
	{
		old_val = data_to_send_s[0] + 1;
	}
	else
	{
		old_val = data_to_send_s[0];
	}
}


void delay_ms(u16_t ms)
{
	delay_timer = ms;
	while(delay_timer != 0);
}



void delay_us(u16_t us)
{
	asm volatile (	"MOV R0,%[loops]\n\t"\
			"1: \n\t"\
			"SUB R0, #1\n\t"\
			"CMP R0, #0\n\t"\
			"BNE 1b \n\t" : : [loops] "r" (8*us) : "memory"\
		      );
}





