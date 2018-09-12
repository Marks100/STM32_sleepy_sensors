#include "stm32f10x_gpio.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x.h"
#include "misc.h"

#include "HAL_BRD.h"





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
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	/* Configure the GPIOs */
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Configure the GPIO_LED pin */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

#if( DEBUG==1)

	/* Configure the GPIO_LED pin */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* If we are in debug mode then we can configure the wakeup pin to have an ISR */

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0 );

	EXTI_InitTypeDef EXTI_InitStruct;

	EXTI_InitStruct.EXTI_Line = EXTI_Line0 ;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt ;
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
#endif
}








/*!
****************************************************************************************************
*
*   \brief         Reads Any PORT and any PIN
*
*   \author        MS
*
*   \return        low_high_et
*
***************************************************************************************************/
low_high_et HAL_BRD_Read_Pin_state( GPIO_TypeDef * port, u16_t pin )
{
	low_high_et returnType;

	if( ( port->IDR & pin ) == pin )
	{
		returnType = HIGH;
	}
	else
	{
		returnType = LOW;
	}

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
void HAL_BRD_Set_Pin_state(  GPIO_TypeDef * port, u16_t pin, low_high_et state )
{
	if( state == HIGH)
	{
		port->ODR |= pin;
	}
	else
	{
		port->ODR &= ~pin;
	}
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
void HAL_BRD_Toggle_Pin_state(  GPIO_TypeDef * port, u16_t pin )
{

    /* Firstly read the PIN state */
    if( ( port->ODR & pin ) == pin )
    {
        HAL_BRD_Set_Pin_state( port, pin, LOW );
    }
    else
    {
        HAL_BRD_Set_Pin_state( port, pin, HIGH );
    }
}




/*!
****************************************************************************************************
*
*   \brief         SETS the battery voltage enable pin
*
*   \author        MS
*
*   \return        None
*
***************************************************************************************************/
void HAL_BRD_Set_batt_monitor_state( disable_enable_et state )
{
	if( state == ENABLE )
	{
		//HAL_BRD_Set_Pin_state();
	}
	else
	{
		//HAL_BRD_Set_Pin_state();
	}
}


/**************************************************************************************************
EXTERNAL API's
***************************************************************************************************/




/*!
****************************************************************************************************
*
*   \brief         SETS the rf enable pin
*
*   \author        MS
*
*   \return        None
*
***************************************************************************************************/
void HAL_BRD_Set_rf_enable_pin( disable_enable_et state )
{
	if( state == ENABLE )
	{
		//HAL_BRD_Set_Pin_state();
	}
	else
	{
		//HAL_BRD_Set_Pin_state();
	}
}






/*!
****************************************************************************************************
*
*   \brief         Toggles the heartbeat pin
*
*   \author        MS
*
*   \return        None
*
***************************************************************************************************/
void HAL_BRD_Toggle_heartbeat_pin(  void )
{
    HAL_BRD_Toggle_Pin_state( GPIOB, GPIO_Pin_11 );
}












void EXTI0_IRQHandler(void)
{
	/* Make sure that interrupt flag is set */
	if ( EXTI_GetFlagStatus(EXTI_Line0) != RESET )
	{
		/* Now we keep track of the interrupt edge */
		/* Clear interrupt flag */
		EXTI_ClearITPendingBit(EXTI_Line0);

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
	}
}






/*!
****************************************************************************************************
*
*   \brief         Interrupt Handler
*
*   \author        MS
*
*   \return        low_high_et
*
***************************************************************************************************/
/* Handle PB12 interrupt */
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


