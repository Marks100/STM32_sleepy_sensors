/*! \file
*               Author: mstewart
*   \brief      HAL_BRD module
*/
/***************************************************************************************************
**                              Includes                                                          **
***************************************************************************************************/
#include "stm32f10x_gpio.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x.h"
#include "misc.h"

#include "MODE_MGR.h"
#include "SYSTICK_MGR.h"
#include "RTC.h"
#include "HAL_BRD.h"
#include "HAL_config.h"

/*!
****************************************************************************************************
*
*   \brief         Initialise the Pins,
*   			   lets just do gpio pins here and let other modules handle themselves
*
*   \author        MS
*
*   \return        None
*
***************************************************************************************************/
void HAL_BRD_init( void )
{
	/* Disable the JTAG as this saves us some pins :) */
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	/* Setup pins to default low */
	HAL_BRD_set_NRF_power_pin_state( OFF );

	/* Configure the GPIOs */
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Configure the DEBUG selector pin, its important that this comes first */
	GPIO_InitStructure.GPIO_Pin = DEBUG_SEL_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(DEBUG_SEL_PORT, &GPIO_InitStructure);

	/* Configure the power pin for the NRF24l01 and set it low immediately*/
	GPIO_InitStructure.GPIO_Pin = NRF_PWR_EN_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(NRF_PWR_EN_PORT, &GPIO_InitStructure);

	/* Configure the NRF irq ( Data Sent ) input pin */
	GPIO_InitStructure.GPIO_Pin = NRF_IRQ_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(NRF_IRQ_PORT, &GPIO_InitStructure);

	/* Configure the Temp sensor enable pin and Set it LOW immediately */
	GPIO_InitStructure.GPIO_Pin = BMP280_PWR_EN_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(BMP280_PWR_EN_PORT, &GPIO_InitStructure);
	HAL_BRD_set_BMP280_power_pin_state(LOW);

	/* Configure the GPIO_LED pin and set LOW immediately */
	GPIO_InitStructure.GPIO_Pin = ONBOARD_LED_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(ONBOARD_LED_PORT, &GPIO_InitStructure);
	HAL_BRD_set_onboard_led( OFF );

	/* Interogates the HW Debug pin to see if Debug mode is required */
	MODE_MGR_read_operating_mode();

	if( MODE_MGR_get_operating_mode() == MODE_MGR_DEBUG_MODE )
	{
		/* configure the debug mode led ( this lets us know we are in debug mode and will only be turned
		on in debug mode */
		GPIO_InitStructure.GPIO_Pin = DEBUG_MODE_LED_PIN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_Init(DEBUG_MODE_LED_PORT, &GPIO_InitStructure);

		/* Configure the wakeup ( or in debug mode interrupt ) pin */
		GPIO_InitStructure.GPIO_Pin = HW_WAKEUP_PIN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_Init(HW_WAKEUP_PORT, &GPIO_InitStructure);

		/* If we are in debug mode then we can configure the wakeup pin to have an ISR */
		GPIO_EXTILineConfig(HW_WAKEUP_PORT_SRC, HW_WAKEUP_PIN_SRC );

		EXTI_InitTypeDef EXTI_InitStruct;

		EXTI_InitStruct.EXTI_Line = HW_WAKEUP_EXT_LINE;
		EXTI_InitStruct.EXTI_LineCmd = ENABLE;
		EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
		EXTI_Init(&EXTI_InitStruct);

		NVIC_InitTypeDef NVIC_InitStruct;

		/* Add IRQ vector to NVIC */
		NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn;
		/* Set priority */
		NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
		/* Set sub priority */
		NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
		/* Enable interrupt */
		NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
		/* Add to NVIC */
		NVIC_Init(&NVIC_InitStruct);
	}

	/* Configure the NRF24 NCS pin */
	GPIO_InitStructure.GPIO_Pin = NRF_CS_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(NRF_CS_PORT, &GPIO_InitStructure);

	/* Configure the NRF24 CE pin */
	GPIO_InitStructure.GPIO_Pin = NRF_CE_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(NRF_CE_PORT, &GPIO_InitStructure);
}




/*!
****************************************************************************************************
*
*   \brief         Resets the board
*
*   \author        MS
*
*   \return        low_high_et
*
***************************************************************************************************/
void HAL_BRD_reset( void )
{
	NVIC_SystemReset();
}






/*!
****************************************************************************************************
*
*   \brief         Reads Any PORT and any PIN
*
*   \author        MS
*
*   \return        low_high_et state of the PIN
*
***************************************************************************************************/
low_high_et HAL_BRD_read_pin_state( GPIO_TypeDef * port, u16_t pin )
{
	low_high_et returnType;

	returnType = (low_high_et)GPIO_ReadInputDataBit( port, pin );

	return ( returnType );
}




/*!
****************************************************************************************************
*
*   \brief         SETS Any PIN on any PORT
*
*   \author        MS
*
*   \return        None
*
***************************************************************************************************/
void HAL_BRD_set_pin_state(  GPIO_TypeDef * port, u16_t pin, low_high_et state )
{
	GPIO_WriteBit( port, pin, (BitAction)state );
}





/*!
****************************************************************************************************
*
*   \brief         Toggles Any PIN on any PORT
*
*   \author        MS
*
*   \return        None
*
***************************************************************************************************/
void HAL_BRD_toggle_pin_state(  GPIO_TypeDef * port, u16_t pin )
{
	low_high_et state;

    /* Firstly read the PIN state */
	state = HAL_BRD_read_pin_state( port, pin );

    if( state == HIGH )
    {
        HAL_BRD_set_pin_state( port, pin, LOW );
    }
    else
    {
        HAL_BRD_set_pin_state( port, pin, HIGH );
    }
}


/*!
****************************************************************************************************
*
*   \brief         Sets thestate of the onboard LED
*
*   \author        MS
*
*   \return        None
*
***************************************************************************************************/
void HAL_BRD_set_onboard_led( off_on_et state )
{	
	/* Onboard LED is inverse logic */
	if( state == ON )
	{
		HAL_BRD_set_pin_state( ONBOARD_LED_PORT, ONBOARD_LED_PIN, LOW );
	}
	else
	{
		HAL_BRD_set_pin_state( ONBOARD_LED_PORT, ONBOARD_LED_PIN, HIGH );
	}
}


/*!
****************************************************************************************************
*
*   \brief         Toggles the onboard LED
*
*   \author        MS
*
*   \return        None
*
***************************************************************************************************/
void HAL_BRD_toggle_onboard_led( void )
{
	HAL_BRD_toggle_pin_state( ONBOARD_LED_PORT, ONBOARD_LED_PIN );
}





/*!
****************************************************************************************************
*
*   \brief         Sets the state oftheNRF PWR pin
*
*   \author        MS
*
*   \return        None
*
***************************************************************************************************/
void HAL_BRD_set_NRF_power_pin_state( off_on_et state )
{
	/* Inverse logic for PNP transistor */
	if( state == ON )
	{
		HAL_BRD_set_pin_state( NRF_PWR_EN_PORT, NRF_PWR_EN_PIN, LOW );
	}
	else
	{
		HAL_BRD_set_pin_state( NRF_PWR_EN_PORT, NRF_PWR_EN_PIN, HIGH );
	}
}



/*!
****************************************************************************************************
*
*   \brief         Sets the BMP280 power pin state
*
*   \author        MS
*
*   \return        None
*
***************************************************************************************************/
void HAL_BRD_set_BMP280_power_pin_state( off_on_et state )
{
	HAL_BRD_set_pin_state( BMP280_PWR_EN_PORT, BMP280_PWR_EN_PIN, state );
}


/*!
****************************************************************************************************
*
*   \brief         Sets the state of the debug mode the led
*
*   \author        MS
*
*   \return        None
*
***************************************************************************************************/
void HAL_BRD_set_debug_mode_LED( off_on_et state )
{
	HAL_BRD_set_pin_state( DEBUG_MODE_LED_PORT, DEBUG_MODE_LED_PIN, state );
}



/*!
****************************************************************************************************
*
*   \brief         Toggles the debug led
*
*   \author        MS
*
*   \return        None
*
***************************************************************************************************/
void HAL_BRD_toggle_debug_mode_led( void )
{
    HAL_BRD_toggle_pin_state( DEBUG_MODE_LED_PORT, DEBUG_MODE_LED_PIN );
}




/*!
****************************************************************************************************
*
*   \brief         Reads the state of the debug pin
*
*   \author        MS
*
*   \return        None
*
***************************************************************************************************/
low_high_et HAL_BRD_read_debug_mode_pin( void )
{
	low_high_et state;

	state = HAL_BRD_read_pin_state( DEBUG_SEL_PORT, DEBUG_SEL_PIN );

	return( state );
}





/*!
****************************************************************************************************
*
*   \brief         Sets the rf CE pin state
*
*   \author        MS
*
*   \return        None
*
***************************************************************************************************/
void HAL_BRD_NRF24_set_ce_pin_state( low_high_et state )
{
	HAL_BRD_set_pin_state( NRF_CE_PORT, NRF_CE_PIN, state );
}



/*!
****************************************************************************************************
*
*   \brief         Sets the SPI chip select pin state for the RF module
*
*   \author        MS
*
*   \return        None
*
***************************************************************************************************/
void HAL_BRD_NRF24_spi_slave_select( low_high_et state )
{
	HAL_BRD_set_pin_state( NRF_CS_PORT, NRF_CS_PIN, state );
}





/*!
****************************************************************************************************
*
*   \brief         Reads the state of the IRQ Pin for the NRF module
*
*   \author        MS
*
*   \return        None
*
***************************************************************************************************/
low_high_et HAL_BRD_NRF24_read_irq_pin( void )
{
	low_high_et state;

	state = HAL_BRD_read_pin_state( NRF_IRQ_PORT, NRF_IRQ_PIN );

	return ( state );
}




/*!
****************************************************************************************************
*
*   \brief         Interrupt Handler ( 0 )
*
*   \author        MS
*
*   \return        None
*
***************************************************************************************************/
void EXTI0_IRQHandler(void)
{
	/* Make sure that interrupt flag is set */
	if ( EXTI_GetFlagStatus(EXTI_Line0) != RESET )
	{
		/* Now we keep track of the interrupt edge */
		/* Clear interrupt flag */
		EXTI_ClearITPendingBit(EXTI_Line0);

		/* Set the alarm status of the RTC module */
		RTC_alarm_triggered();
	}
}



/*!
****************************************************************************************************
*
*   \brief         Interrupt Handler ( 1 )
*
*   \author        MS
*
*   \return        low_high_et
*
***************************************************************************************************/
void EXTI1_IRQHandler(void)
{
	/* Make sure that interrupt flag is set */
	if ( EXTI_GetFlagStatus(EXTI_Line1) != RESET )
	{
		/* Now we keep track of the interrupt edge */
		/* Clear interrupt flag */
		EXTI_ClearITPendingBit(EXTI_Line1);
	}
}





/*!
****************************************************************************************************
*
*   \brief         Interrupt Handler ( 10 - 15 )
*
*   \author        MS
*
*   \return        low_high_et
*
***************************************************************************************************/
void EXTI15_10_IRQHandler(void)
{
	/* Make sure that interrupt flag is set */
	if ( EXTI_GetFlagStatus(EXTI_Line15) != RESET )
	{
	    /* Now we keep track of the interrupt edge */
		/* Clear interrupt flag */
		EXTI_ClearITPendingBit(EXTI_Line15);
	}
}


