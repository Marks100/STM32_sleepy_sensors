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
#include "HAL_I2C.h"
#include "HAL_UART.h"
#include "NRF24.h"
//#include "NRF24_registers.h"
#include "main.h"


u16_t delay_timer = 0u;
u32_t ctr = 0u;
const u8_t NRF24_data_pipe_default_s [5] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE};
u8_t  NRF24_register_readback_s[NRF24_DEFAULT_CONFIGURATION_SIZE];

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

	/* Power down the RF chip by setting the low level mode of operation to power down */
	NRF24_set_low_level_mode( NRF_POWER_DOWN_MODE );
	NRF24_spi_slave_select( HIGH );
	NRF24_ce_select( LOW );

	if( debug_mode == ENABLE )
	{
		/* In debug mode lets init the debug usart as this consumes lots of power */
		//SERIAL_init();
	}

	while (1)
	{
		if( debug_mode != ENABLE )
		{
			/* We have woken up from a shutdown, lets fire off the RF frame */
			NRF24_ce_select( LOW );
            NRF24_spi_slave_select( HIGH );

            NRF24_set_low_level_mode( NRF_STANDBY_1_MODE );

            /* Setup initial register values */
            NRF24_set_configuration( NRF24_DEFAULT_CONFIG );

            /* Set channel and power levels */
            NRF24_set_PA_TX_power( RF_MAX_TX_PWR );
            NRF24_set_rf_data_rate( RF24_250KBPS );

            /* open up the data pipe to communicate with the receiver */
            NRF24_open_write_data_pipe( 0, NRF24_data_pipe_default_s );

            /* Setup retries and dynamic ACKS */
            NRF24_set_dynamic_payloads( ENABLE, 0 );
            NRF24_setup_dynamic_ack( ENABLE );
            NRF24_set_AA_data_pipe( ENABLE, 0 );
            NRF24_setup_retries( RF24_2000_US, 10u );

            /* After all the setup read back the regs */
            NRF24_read_all_registers( NRF24_register_readback_s );

			/* Flush out the tx and rx buffers to get ready for tx */
            NRF24_complete_flush();

			/* carry out the necessary steps to transition to TX_MODE */
            NRF24_set_low_level_mode( NRF_TX_MODE );

            NRF24_setup_payload( "Start Test", 10u );

            /* Send the configured payload */
			NRF24_send_payload();

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
				/* Toggle LED which connected to PC13*/
				HAL_BRD_toggle_led();

				/* We have woken up from a shutdown, lets fire off the RF frame */
				NRF24_ce_select( LOW );
				NRF24_spi_slave_select( HIGH );

				delay_us(1000);

				NRF24_set_low_level_mode( NRF_STANDBY_1_MODE );

				/* Setup initial register values */
				NRF24_set_configuration( NRF24_DEFAULT_CONFIG );

				/* Set channel and power levels */
				NRF24_set_PA_TX_power( RF_MAX_TX_PWR );
				NRF24_set_rf_data_rate( RF24_250KBPS );

				/* open up the data pipe to communicate with the receiver */
				NRF24_open_write_data_pipe( 0, NRF24_data_pipe_default_s );

				/* Setup retries and dynamic ACKS */
				NRF24_set_dynamic_payloads( ENABLE, 0 );
				NRF24_setup_dynamic_ack( ENABLE );
				NRF24_set_AA_data_pipe( ENABLE, 0 );
				NRF24_setup_retries( RF24_2000_US, 10u );

				/* After all the setup read back the regs */
				NRF24_read_all_registers( NRF24_register_readback_s );

				/* Flush out the tx and rx buffers to get ready for tx */
				NRF24_complete_flush();

				/* carry out the necessary steps to transition to TX_MODE */
				NRF24_set_low_level_mode( NRF_TX_MODE );

				NRF24_setup_payload( "Start Test", 10u );

				/* Send the configured payload */
				NRF24_send_payload();

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
			"BNE 1b \n\t" : : [loops] "r" (10*us) : "memory"\
		      );
}





