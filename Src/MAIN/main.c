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
#include "main.h"



u16_t delay_timer = 0u;
u8_t task_timer_s = 0u;

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
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

	/* Enable the clocks to the CRC, TIM1 and SYSCFG module */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

	/* Enable the TIM2 clock */
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM2, ENABLE);

	/* Enable the TIM3 clock */
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM3, ENABLE);

	/* Enable the ADC clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	/* Enable the clock to the USART1 */
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1, ENABLE);

	/* Enable the clock to the Alternative function module */
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO, ENABLE);

	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_SRAM, ENABLE);

	/* Enable the clock to the I2C module */
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_I2C1, ENABLE);

	/* Enable the clock for SPI1 module */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

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
	int i, j;
	/* Initialize Leds mounted on STM32 board */
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_DeInit();
	SystemInit();

	RCC_HCLKConfig(RCC_SYSCLK_Div1);
	RCC_PCLK1Config(RCC_HCLK_Div1);
	RCC_PCLK2Config(RCC_HCLK_Div1);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,   ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,  ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	/* Configure the GPIO_LED pin */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* Configure I2C_EE pins: SCL and SDA */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);


	I2C_InitTypeDef  I2C_InitStructure;

	/* I2C configuration */
//	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
//	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
//	I2C_InitStructure.I2C_OwnAddress1 = 0x38;
//	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
//	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
//	I2C_InitStructure.I2C_ClockSpeed = 100000;
//
//	/* I2C Peripheral Enable */
//	I2C_Cmd(I2C1, ENABLE);
//	/* Apply I2C configuration after enabling it */
//	I2C_Init(I2C1, &I2C_InitStructure);
//
//
//	  I2C_GenerateSTART(I2C1,ENABLE);
//	  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
//
//	  I2C_Send7bitAddress(I2C1, 0x51, I2C_Direction_Transmitter);
//	  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
//
//	  I2C_SendData(I2C1,0x00);
//	  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
//
//	  I2C_SendData(I2C1,0x55);
//	  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
//
//	  I2C_GenerateSTOP(I2C1,ENABLE);
//
//	  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));



	// ENABLE Wake Up Pin
	PWR_WakeUpPinCmd(ENABLE);

	while (1)
	{
	  /* LED blink */
	  for(j=0; j<6; j++)
	  {
		  /* Toggle LED which connected to PC13*/
		  GPIOC->ODR ^= GPIO_Pin_13;
		  /* delay */
		  for(i=0; i<0x100000; i++);
	  }

	  /* Disable LED */
	  GPIO_SetBits(GPIOC, GPIO_Pin_13);

	  //I2C_Cmd(I2C1, DISABLE);
	  //I2C_DeInit(I2C1);

	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,  DISABLE);
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, DISABLE);
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, DISABLE);

	  /* Enters STANDBY mode */
	  PWR_EnterSTANDBYMode();
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




