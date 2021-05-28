/***************************************************************************************************
**                              Includes                                                          **
***************************************************************************************************/
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_gpio.h"
#include "misc.h"

#include "C_defs.h"
#include "STDC.h"
#include "CLI_MGR.h"
#include "HAL_config.h"
#include "HAL_UART.h"


STATIC u8_t HAL_UART_rx_buf_char_s;


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
void HAL_USART2_init( void )
{
	/* Enable GPIOA clock, should be enabled anyway but just in case */
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);

	/* Enable USART2 clock */
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_USART2, ENABLE );

	/* Configure the GPIOs */
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Configure USART1 Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = USART2_TX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(USART2_TX_PORT, &GPIO_InitStructure);

	/* Configure USART2 Rx as input floating */
	GPIO_InitStructure.GPIO_Pin = USART2_RX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(USART2_RX_PORT, &GPIO_InitStructure);

	/* Configure the USART2 */
	USART_InitTypeDef USART_InitStructure;

	/* USART1 configuration ------------------------------------------------------*/
	/* USART1 configured as follow:
		- BaudRate = 115200 baud
		- Word Length = 8 Bits
		- One Stop Bit
		- No parity
		- Hardware flow control disabled (RTS and CTS signals)
		- Receive and transmit enabled
		- USART Clock disabled
		- USART CPOL: Clock is active low
		- USART CPHA: Data is captured on the middle
		- USART LastBit: The clock pulse of the last data bit is not output to
			the SCLK pin
	 */
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART2, &USART_InitStructure);

	/* Enable USART1 */
	USART_Cmd(USART2, ENABLE);

	/* Enable the USART2 Receive interrupt: this interrupt is generated when the
		USART2 receive data register is not empty */
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

	HAL_UART_rx_buf_char_s = 0u;
}




/*!
****************************************************************************************************
*
*   \brief         Close the serial port
*
*   \author        MS
*
*   \return        none
*
*   \note          Fixed baudrate for now at 9600 8N1
*
***************************************************************************************************/
void HAL_USART2_close( void )
{
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
void HAL_USART2_send_data(const char *pucBuffer, u16_t data_size )
{
	u16_t byte_index = 0u;

    while( data_size )
    {
        USART_SendData(USART2, pucBuffer[byte_index++] );
        while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
        {
        }
        data_size--;
    }
}





///***************************************************************************************************
//**                              ISR Handlers                                                      **
//***************************************************************************************************/
void USART2_IRQHandler(void)
{
	HAL_UART_rx_buf_char_s = USART_ReceiveData(USART2);

	CLI_MGR_handle_received_char( HAL_UART_rx_buf_char_s );
}

///****************************** END OF FILE *******************************************************/
