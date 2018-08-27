#include "stm32f10x_gpio.h"
#include "stm32f10x_exti.h"
#include "misc.h"

#include "HAL_BRD.h"


u8_t counter;
EXTITrigger_TypeDef trigger;

low_high_et previous_pin_a;
low_high_et previous_pin_b;
low_high_et pin_a;
low_high_et pin_b;




/*!
****************************************************************************************************
*
*   \brief         Initialise the Pins
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


	GPIO_InitTypeDef  GPIO_InitStructure;

	/* Setup the Power switch input pin */
	GPIO_InitStructure.GPIO_Pin = ( GPIO_Pin_12 );
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Setup the Vreg Enable pin */
	GPIO_InitStructure.GPIO_Pin = ( GPIO_Pin_13 );
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
//
//	/* Setup the LED NEO_PIXEL pins */
//	GPIO_InitStructure.GPIO_Pin = ( GPIO_Pin_13 );
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
//	GPIO_Init(GPIOC, &GPIO_InitStructure);
//
//	/* Setup the Rotary encoder pins ( PC14 and PC15 ) */
//	GPIO_InitStructure.GPIO_Pin = ( GPIO_Pin_15 | GPIO_Pin_14 );
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
//    GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* Setup the Rotary encoder button ( PA2 ) */
	GPIO_InitStructure.GPIO_Pin = ( GPIO_Pin_2 ) ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);


    /* Setup the GPIO pins for ADC measurements ( PA0, PA1 ) */
	GPIO_InitStructure.GPIO_Pin   = ( GPIO_Pin_0 | GPIO_Pin_1 );
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);


	/* Setup the PWM pins for TIM1 for the motor control output ( PA8, and PA9) */
	GPIO_InitStructure.GPIO_Pin = ( GPIO_Pin_8 | GPIO_Pin_9 );
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Setup a heartbeat Pin for the SysTick timer */
	GPIO_InitStructure.GPIO_Pin = ( GPIO_Pin_11 );
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Setup a input switch pin ( laser )*/
	GPIO_InitStructure.GPIO_Pin = ( GPIO_Pin_10 );
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Setup an output pin for the laser */
	GPIO_InitStructure.GPIO_Pin = ( GPIO_Pin_9 );
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);


    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource15 );

    EXTI_InitTypeDef EXTI_InitStruct;

    EXTI_InitStruct.EXTI_Line = EXTI_Line15 ;
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt ;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_Init(&EXTI_InitStruct);

    NVIC_InitTypeDef NVIC_InitStruct;

	/* Add IRQ vector to NVIC */
	NVIC_InitStruct.NVIC_IRQChannel = EXTI15_10_IRQn;
	/* Set priority */
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
	/* Set sub priority */
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
	/* Enable interrupt */
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	/* Add to NVIC */
	NVIC_Init(&NVIC_InitStruct);

	previous_pin_a = HIGH;
	previous_pin_b = HIGH;
	pin_a = HIGH;
	pin_b = HIGH;
	trigger = EXTI_Trigger_Falling;
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




/**************************************************************************************************
EXTERNAL API's
***************************************************************************************************/

/*!
****************************************************************************************************
*
*   \brief         Sets the Vreg EN pin
*
*   \author        MS
*
*   \return        None
*
***************************************************************************************************/
void HAL_BRD_set_vreg_enable_pin( low_high_et state )
{
    HAL_BRD_Set_Pin_state( GPIOB, GPIO_Pin_13, state );
}


/*!
****************************************************************************************************
*
*   \brief         Sets the pwm pin state for the x axis
*
*   \author        MS
*
*   \return        None
*
***************************************************************************************************/
void HAL_BRD_set_pwmPin_x_axis_state( low_high_et state )
{
    HAL_BRD_Set_Pin_state(GPIOA, GPIO_Pin_8, state );
}


/*!
****************************************************************************************************
*
*   \brief         Sets the pwm pin state for the y axis
*
*   \author        MS
*
*   \return        None
*
***************************************************************************************************/
void HAL_BRD_set_pwmPin_y_axis_state( low_high_et state )
{
    HAL_BRD_Set_Pin_state(GPIOA, GPIO_Pin_9, state );
}




/*!
****************************************************************************************************
*
*   \brief         Sets the state of the laser
*
*   \author        MS
*
*   \return        None
*
***************************************************************************************************/
void HAL_BRD_set_laser_Pin_state( off_on_et state )
{
    if( state == OFF )
    {
        HAL_BRD_Set_Pin_state(GPIOB, GPIO_Pin_9, LOW );
    }
    else
    {
        HAL_BRD_Set_Pin_state(GPIOB, GPIO_Pin_9, HIGH );
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



/*!
****************************************************************************************************
*
*   \brief         Reads the input laser switch pin
*
*   \author        MS
*
*   \return        None
*
***************************************************************************************************/
low_high_et HAL_BRD_read_switch_input_laser( void )
{
    low_high_et state;

    state = HAL_BRD_Read_Pin_state(GPIOB, GPIO_Pin_10);

    return ( state );
}



/*!
****************************************************************************************************
*
*   \brief         Reads the input of the rotary encoder switch
*
*   \author        MS
*
*   \return        None
*
***************************************************************************************************/
low_high_et HAL_BRD_read_switch_input_rotary_encoder( void )
{
    low_high_et state;

    state = HAL_BRD_Read_Pin_state(GPIOA, GPIO_Pin_2);

    return ( state );
}




/*!
****************************************************************************************************
*
*   \brief         Reads the input of the power button
*
*   \author        MS
*
*   \return        None
*
***************************************************************************************************/
low_high_et HAL_BRD_read_switch_input_power( void )
{
	low_high_et state;

	state = HAL_BRD_Read_Pin_state(GPIOB, GPIO_Pin_12);

	return( state );
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


