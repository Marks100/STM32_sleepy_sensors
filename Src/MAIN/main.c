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

RCC_ClocksTypeDef RCC_Clocks;
u16_t delay_timer = 0u;
u8_t  NRF24_register_readback_s[NRF24_DEFAULT_CONFIGURATION_SIZE];
u8_t  NRF24_rf_frame_s[10];
u8_t  old_val = 0u;

/* These values have been calculated with an excel sheet using
 the steinheart heart equation and a 11795 ohm resistor (R1) */
const s8_t NTS_LOOKUP_TABLE[886] =
{
    120,118,117,116,115,114,113,112,111,110,110,109,108,107,106,105,105,104,103,103,102,101,100,100,99,99,98,97,97,
    96,96,95,95,94,93,93,92,92,91,91,90,90,89,89,89,88,88,87,87,86,86,86,85,85,84,84,84,83,83,82,82,82,81,81,81,80,
    80,80,79,79,79,78,78,78,77,77,77,76,76,76,75,75,75,75,74,74,74,73,73,73,73,72,72,72,71,71,71,71,70,70,70,70,69,
    69,69,69,68,68,68,68,67,67,67,67,67,66,66,66,66,65,65,65,65,65,64,64,64,64,64,63,63,63,63,62,62,62,62,62,61,61,
    61,61,61,61,60,60,60,60,60,59,59,59,59,59,58,58,58,58,58,58,57,57,57,57,57,57,56,56,56,56,56,56,55,55,55,55,55,
    55,54,54,54,54,54,54,53,53,53,53,53,53,53,52,52,52,52,52,52,52,51,51,51,51,51,51,50,50,50,50,50,50,50,49,49,49,
    49,49,49,49,49,48,48,48,48,48,48,48,47,47,47,47,47,47,47,47,46,46,46,46,46,46,46,45,45,45,45,45,45,45,45,44,44,
    44,44,44,44,44,44,43,43,43,43,43,43,43,43,43,42,42,42,42,42,42,42,42,41,41,41,41,41,41,41,41,41,40,40,40,40,40,
    40,40,40,40,39,39,39,39,39,39,39,39,39,38,38,38,38,38,38,38,38,38,37,37,37,37,37,37,37,37,37,37,36,36,36,36,36,
    36,36,36,36,35,35,35,35,35,35,35,35,35,35,34,34,34,34,34,34,34,34,34,34,33,33,33,33,33,33,33,33,33,33,32,32,32,
    32,32,32,32,32,32,32,31,31,31,31,31,31,31,31,31,31,31,30,30,30,30,30,30,30,30,30,30,29,29,29,29,29,29,29,29,29,
    29,29,28,28,28,28,28,28,28,28,28,28,28,27,27,27,27,27,27,27,27,27,27,27,26,26,26,26,26,26,26,26,26,26,26,25,25,
    25,25,25,25,25,25,25,25,25,24,24,24,24,24,24,24,24,24,24,24,24,23,23,23,23,23,23,23,23,23,23,23,22,22,22,22,22,
    22,22,22,22,22,22,22,21,21,21,21,21,21,21,21,21,21,21,20,20,20,20,20,20,20,20,20,20,20,20,19,19,19,19,19,19,19,
    19,19,19,19,19,18,18,18,18,18,18,18,18,18,18,18,17,17,17,17,17,17,17,17,17,17,17,17,16,16,16,16,16,16,16,16,16,
    16,16,16,15,15,15,15,15,15,15,15,15,15,15,15,14,14,14,14,14,14,14,14,14,14,14,13,13,13,13,13,13,13,13,13,13,13,
    13,12,12,12,12,12,12,12,12,12,12,12,12,11,11,11,11,11,11,11,11,11,11,11,10,10,10,10,10,10,10,10,10,10,10,10,9,9,
    9,9,9,9,9,9,9,9,9,8,8,8,8,8,8,8,8,8,8,8,8,7,7,7,7,7,7,7,7,7,7,7,6,6,6,6,6,6,6,6,6,6,6,5,5,5,5,5,5,5,5,5,5,5,4,4,
    4,4,4,4,4,4,4,4,4,3,3,3,3,3,3,3,3,3,3,2,2,2,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-2,-2,-2,-2,-2,-2,-2,-2,-2,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-4,-4,-4,-4,-4,-4,-4,-4,-4,-5,-5,
    -5,-5,-5,-5,-5,-5,-5,-5,-6,-6,-6,-6,-6,-6,-6,-6,-6,-7,-7,-7,-7,-7,-7,-7,-7,-8,-8,-8,-8,-8,-8,-8,-8,-8,-9,-9,-9,
    -9,-9,-9,-9,-9,-10,-10,-10,-10,-10,-10,-10,-10,-11,-11,-11,-11,-11,-11,-11,-11,-12,-12,-12,-12,-12,-12,-12,-13,
    -13,-13,-13,-13,-13,-13,-13,-14,-14,-14,-14,-14,-14,-14,-15,-15,-15,-15,-15,-15,-15,-16,-16,-16,-16,-16,-16,-17,
    -17,-17,-17,-17,-17,-17,-18,-18,-18,-18,-18,-18,-19,-19,-19,-19,-19,-19,-20,-20,-20,-20,-20,
};

int main(void)
{
	RCC_DeInit();
	SystemInit();

	RCC_GetClocksFreq (&RCC_Clocks);

	RCC_HCLKConfig(RCC_SYSCLK_Div1);
	RCC_PCLK1Config(RCC_HCLK_Div1);
	RCC_PCLK2Config(RCC_HCLK_Div1);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

	/* Init the HW */
	HAL_BRD_init();
	HAL_I2C_init();
	HAL_SPI_init();
	HAL_ADC_init();
	NVM_init();

	/* Initialise the RTC */
	RTC_ext_init();

	/* Initialise the NRF24 variables */
	NRF24_init();

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

			/* Disable the ADC before the RF transmission to save as much power as possible */
			HAL_ADC_de_init();

			/* Disable the I2C peripheral and clock to save power  */
			HAL_I2C_de_init();

			/* Send the data */
			NRF_simple_send( NRF24_rf_frame_s, sizeof( NRF24_rf_frame_s ), 1u );

			/* Disable the SPI peripheral and clock to save power after the RF transmission */
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

				NRF_simple_send( NRF24_rf_frame_s, sizeof( NRF24_rf_frame_s ), 1u );

				HAL_BRD_toggle_led();

				/* Set the trigger back to false */
				HAL_BRD_set_rtc_trigger_status( FALSE );
			}
		}
	}
}




/* ISR for systick handler */
void SysTick_Handler( void )
{
	delay_timer--;
}




void MAIN_SYSTICK_init( void )
{
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq (&RCC_Clocks);

	SysTick_CLKSourceConfig( SysTick_CLKSource_HCLK_Div8 );

	/* Trigger an interrupt every 1ms */
	SysTick_Config(72000);
}




u8_t generate_random_number( void )
{
	u32_t seed;
	u8_t  random_number;
	u8_t  time_array_s[RTC_TIME_ARRAY_SIZE];

	/* We need to create a "random" value here, and in order to do that we "seed" the rand function with
	 an instantaneous time value comprised of secs, mins, hours and even days */
	RTC_grab_current_running_time( time_array_s );
	seed = time_array_s[0];						   /* The seconds part */
	seed |= ( ( time_array_s[1] & 0x7F ) << 8u );  /* The minutes part */
	seed |= ( ( time_array_s[2] & 0x3F ) << 16u ); /* The hours part */
	seed |= ( ( time_array_s[3] & 0x3F ) << 24u ); /* The days part */

	/* This means that "seed" is now a 32bit number but the max value that srand can accept is 32768,
	   so lets mod the "seed" with 32768 to give us a remainder that can be fed in to create a seed  */
	seed = ( seed % 32768u );

    /* Now Set the seed */
	srand( seed );

    /* Grab the now "random :)" number and limit the values to between 0 and 255 ( 1 byte ) */
    random_number = rand()%255u;

	return ( random_number );
}



void populate_rf_frame( void )
{
	u8_t  buff[30];
	u8_t  data_len = 10u;
	s8_t  temperature = 30u;
	u8_t  humidity = 100u;
	u16_t battery_voltage;

	/* Calculate the battery voltage */
	//battery_voltage = HAL_ADC_sample_batt_voltage();
	//battery_voltage = ( battery_voltage / BATTERY_DIVISION );

	/* Calculate the temperature in celcius, and add the offset */
	//temperature = calculate_temperature();
	//temperature += TEMPERATURE_OFFSET;

	/* Calculate the Humidity */
	//humidity = calculate_humidity();

	NRF24_rf_frame_s[0] =  generate_random_number();
	NRF24_rf_frame_s[1] =  SENSOR_ID;
	NRF24_rf_frame_s[2] =  SENSOR_TYPE;
	NRF24_rf_frame_s[3] =  data_len;
	NRF24_rf_frame_s[4] =  temperature;
	NRF24_rf_frame_s[5] =  humidity;
	NRF24_rf_frame_s[6] =  battery_voltage;
	NRF24_rf_frame_s[7] =  'E';
	NRF24_rf_frame_s[8] =  'M';
	NRF24_rf_frame_s[9] =  'P';
	NRF24_rf_frame_s[10] = '\0';

	if( old_val == NRF24_rf_frame_s[0] )
	{
		old_val = NRF24_rf_frame_s[0] + 1;
	}
	else
	{
		old_val = NRF24_rf_frame_s[0];
	}
}




s8_t calculate_temperature( void )
{
	u8_t i = 0u;
	u32_t result = 0u;
	s8_t temperature;

	for ( i = 0; i < NUM_ADC_TEMP_SAMPLES; i++)
	{
		/* Read the actual adc value */
		result += HAL_ADC_measure_temp();
	}

	result = ( result / NUM_ADC_TEMP_SAMPLES );

	/* shift it right by 2, this essentially now becomes a 10bit ADC reading */
	result = result >> 2u;

	/*! I have already used excel to create a lookup table to convert from voltage to temperature
	using the steinheart equation as this processor ( STM32 ) cannot compute the LOG of a variable ???
	however the temperature range is very extreme and we do not need that kind of range
	so we we will limit the temperature from -20 to 120 which gives us 895 values */

	if( ( result < MIN_TEMP_ARRAY_VAL ) || ( result > MAX_TEMP_ARRAY_VAL ) )
	{
		/* These values are out of range!!! */
		temperature = TEMP_NOT_AVAILABLE;
	}
	else
	{
		/* The ADC value seems reasonable so lets grab a value from the lookup table */

		/* First we need to take an offset away from the ADC reading as we are not using the full
		swing of values */
		result -= MIN_TEMP_ARRAY_VAL;

		/* now index into the table */
		temperature = NTS_LOOKUP_TABLE[result];
	}

	return ( temperature );
}


u8_t calculate_humidity( void )
{
	u8_t humidity = 55;

	return ( humidity );
}



void delay_ms(u16_t ms)
{
	delay_timer = ms;
	while(delay_timer != 0);
}



void delay_us(u16_t us)
{
	/* If we are just delaying time then we want to do this as efficiently as possible,
	 * Lets reduce the clock speed down as low as we can to do this ( /256 ), this means we are 256
	 * times slower than what we originally were running at */

	RCC_HCLKConfig(RCC_SYSCLK_Div8);
	RCC_GetClocksFreq(&RCC_Clocks);

	asm volatile (	"MOV R0,%[loops]\n\t"\
			"1: \n\t"\
			"SUB R0, #1\n\t"\
			"CMP R0, #0\n\t"\
			"BNE 1b \n\t" : : [loops] "r" (1*us) : "memory"\
		      );

	RCC_HCLKConfig(RCC_SYSCLK_Div1);
	RCC_GetClocksFreq (&RCC_Clocks);
}





