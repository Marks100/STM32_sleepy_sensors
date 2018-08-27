/*! \file
*               $Revision: 16923 $
*
*               $Author: mstewart $
*
*               $Date: 2014-01-16 15:40:40 +0000 (Thu, 16 Jan 2014) $
*
*               $HeadURL: https://selacvs01.schrader.local:8443/svn/ECU_Software/LF_TOOL_GEN2/trunk/Src/HAL/HAL_UART/HAL_UART.c $
*
*   \brief      UART interface module
*/
/* COPYRIGHT NOTICE
* ==================================================================================================
*
* The contents of this document are protected under copyright and contain commercially and/or
* technically confidential information. The content of this document must not be used other than
* for the purpose for which it was provided nor may it be disclosed or copied (by the authorised
* recipient or otherwise) without the prior written consent of an authorised officer of Schrader
* Electronics Ltd.
*
*         (C) $Date:: 2014#$ Schrader Electronics Ltd.
*
****************************************************************************************************
*/
/***************************************************************************************************
**                              Includes                                                          **
***************************************************************************************************/
#include "C_defs.h"
#include "COMPILER_defs.h"
#include "stm32f10x_usart.h"
#include "misc.h"
#include "HAL_UART.h"



/* Module Identification for STDC_assert functionality */
STATIC u8_t rx_byte_s = 0;


/***************************************************************************************************
**                              Data declarations and definitions                                 **
***************************************************************************************************/
/* None */



/***************************************************************************************************
**                              Public Functions                                                  **
***************************************************************************************************/
/*!
****************************************************************************************************
*
*   \brief         Module (re-)initialisation function
*
*   \author        MS
*
*   \return        none
*
*   \note          Fixed baudrate for now at 9600 8N1
*
***************************************************************************************************/
void HAL_UART_init( void )
{
	USART_InitTypeDef usartConfig;
	USART_ClockInitTypeDef usartclockconfig;

	USART_Cmd(USART1, ENABLE_);

	usartConfig.USART_BaudRate = 57800;//9600;
	usartConfig.USART_WordLength = USART_WordLength_8b;
	usartConfig.USART_StopBits = USART_StopBits_1;
	usartConfig.USART_Parity = USART_Parity_No;
	usartConfig.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	usartConfig.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USART1, &usartConfig);

	/* Enable RXNE interrupt */
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE_);

	/* Enable USART1 global interrupt */
	NVIC_EnableIRQ(USART1_IRQn);

//	usartclockconfig.USART_CPHA = USART_CPHA_1Edge;
//	usartclockconfig.USART_CPOL = USART_CPOL_Low;
//	usartclockconfig.USART_Clock = USART_Clock_Enable;
//	usartclockconfig.USART_LastBit = USART_LastBit_Disable;
//
//	USART_ClockInit( USART1, &usartclockconfig );
}





/*!
****************************************************************************************************
*
*   \brief         Writes a buffer of information out to UART
*
*   \author        MS
*
*   \return        none
*
***************************************************************************************************/
void HAL_UART_write_data( u8_t* buffer, u16_t length )
{
    u16_t i;

    for( i = 0u; i < length; i++ )
    {
    	/* Looks like we have to send data using the 16bit registers, so lets make our 8bit char into
    	 a 16bit */
    	u16_t padded_data = buffer[i];

    	/* Wait until Tx data register is empty */
    	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);

    	// load next byte into the Tx buffer
    	USART_SendData(USART1, padded_data );
    }
    return;
}




void HAL_UART_send_temp( s8_t temp )
{
	HAL_UART_write_data((u8_t*)"Temperature:\n", 11 );
	if( temp < 0 )
	{
		HAL_UART_write_data((u8_t*)"-", 11 );
	}
	temp = abs(temp);
	HAL_UART_write_data((u8_t*)&temp,1 );

	HAL_UART_write_data((u8_t*)"\n\n\n\n", 11 );
}



//
///***************************************************************************************************
//**                              ISR Handlers                                                      **
//***************************************************************************************************/
void USART1_IRQHandler(void)
{
    /* RXNE handler */
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
    	/* Grab the latest received byte */
		rx_byte_s = (char)USART_ReceiveData(USART1);

		USART_ClearITPendingBit( USART1, USART_IT_RXNE );

		/* Handled the received bytre */

        USART_SendData(USART1, 'T');
    }
}






///****************************** END OF FILE *******************************************************/
