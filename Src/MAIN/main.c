/* STM32 specific includes */
#ifndef UNIT_TEST
	#include "misc.h"
    #include "stm32f10x.h"
    #include "stm32f10x_rcc.h"
    #include "stm32f10x_pwr.h"
	#include "stm32f10x_i2c.h"
#endif

#include "C_defs.h"
#include "COMPILER_defs.h"
#include "COMPILER_config.h"
#include "HAL_BRD.h"
#include "HAL_ADC.h"
#include "HAL_I2C.h"
#include "HAL_UART.h"
#include "main.h"



u16_t delay_timer = 0u;
u8_t task_timer_s = 0u;
u16_t battery_voltage_mv_s = 0u;
u32_t ctr = 0u;

/*!
****************************************************************************************************
*
*   \brief          Initialises System clocks
*
*   \author         MS
*
*   \return         None
*
***************************************************************************************************/
void MAIN_setupclocks( void )
{
	RCC_DeInit();
	SystemInit();

	RCC_HCLKConfig(RCC_SYSCLK_Div1);
	RCC_PCLK1Config(RCC_HCLK_Div1);
	RCC_PCLK2Config(RCC_HCLK_Div1);

	/* Enable the clocks to all the Ports */
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE );
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE );
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC, ENABLE );
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD, ENABLE );

	/* Enable the clocks to the CRC, TIM1 and SYSCFG module */
	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_CRC, ENABLE);
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_TIM1, ENABLE);

	/* Enable the TIM2 clock */
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM2, ENABLE);

	/* Enable the TIM3 clock */
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM3, ENABLE);

	/* Enable the ADC clock */
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_ADC1, ENABLE);

	/* Enable the clock to the USART1 */
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1, ENABLE);

	/* Enable the clock to the Alternative function module */
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO, ENABLE);

	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_SRAM, ENABLE);

	/* Enable the clock to the I2C module */
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_I2C1, ENABLE);

	/* Enable the clock for SPI1 module */
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_SPI1, ENABLE);

	RCC_MCOConfig( RCC_MCO_SYSCLK );
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
			"BNE 1b \n\t" : : [loops] "r" (9*us) : "memory"\
		      );
}



int main(void)
{
	ctr = 0;
	battery_voltage_mv_s = 0u;

	RCC_DeInit();
	SystemInit();

	RCC_HCLKConfig(RCC_SYSCLK_Div1);
	RCC_PCLK1Config(RCC_HCLK_Div1);
	RCC_PCLK2Config(RCC_HCLK_Div1);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

	HAL_BRD_init();
	HAL_I2C_init();

	/* Initialise the RTC */
	RTC_ext_init();

#if( DEBUG==1)
	/* In debug mode lets init the debug uart as this consumes lots of power */
	SERIAL_init();

	/* Power up the RF chip all the time in debug mode */
	HAL_BRD_Set_rf_enable_pin( ENABLE );
#else
	// ENABLE Wake Up Pin
	PWR_WakeUpPinCmd(ENABLE);
#endif

	while (1)
	{
		/* Power up the RF chip all the time in debug mode */
		HAL_BRD_Set_rf_enable_pin( ENABLE );
		HAL_BRD_Set_Pin_state(GPIOC, GPIO_Pin_13, HIGH);

#if(DEBUG==0)
		u32_t i = 0u;
		u32_t j = 0u;

		/* LED blink */
		for(i=0; i<2; i++)
		{
			/* Toggle LED which connected to PC13*/
			GPIOC->ODR ^= GPIO_Pin_13;
			/* delay */
			for(j=0; j<0x10000; j++);
		}
#endif

		/* Power up the RF chip all the time in debug mode */
		HAL_BRD_Set_rf_enable_pin( DISABLE );

		/* Disable LED */
		GPIO_SetBits(GPIOC, GPIO_Pin_13);

		if( ctr >= U32_T_MAX)
		{
			ctr = 0u;
		}
		else
		{
			ctr ++;
		}

#if( DEBUG==1)
	/* Handle the serial messages */
	SERIAL_msg_handler();
#else

	/* Disable the I2C peripheral to save power */
	HAL_I2C_de_init();
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,  DISABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, DISABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, DISABLE);

	/* Enters STANDBY mode */
	PWR_EnterSTANDBYMode();
#endif
	}
}




/* ISR for systick handler */
void SysTick_Handler( void )
{
	delay_timer--;

	task_timer_s += 1u;

	if( task_timer_s >= 20u )
	{
		/* reset the task timer */
		task_timer_s = 0u;
	}
}


u32_t get_counter( void )
{
	return ( ctr );
}



