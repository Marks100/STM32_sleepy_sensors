/*! \file
*               Author: mstewart
*   \brief      HAL_SPI module
*/
/***************************************************************************************************
**                              Includes                                                          **
***************************************************************************************************/
#include "stm32f10x_spi.h"

#include "HAL_config.h"
#include "C_defs.h"
#include "COMPILER_defs.h"
#include "HAL_BRD.h"
#include "DBG_LOG_MGR.h"
#include "HAL_SPI.h"



/* Module Identification for STDC_assert functionality */
#define STDC_MODULE_ID   STDC_MOD_HAL_SPI



/***************************************************************************************************
**                              Data declaratons and definitions                                 **
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
*   \note          Fixed baudrate for now at 5MHz 8N1
*
***************************************************************************************************/
void HAL_SPI1_init( void )
{
	/* Enable SPI clock */
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_SPI1, ENABLE );

	/* Enable the GPIOA clock */
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE );

	/* Configure the GPIOs */
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Setup the SPI pins  */
	GPIO_InitStructure.GPIO_Pin = SPI1_SCK_PIN | SPI1_MISO_PIN | SPI1_MOSI_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(SPI1_PORT, &GPIO_InitStructure);

	SPI_InitTypeDef   SPI_InitStructure;

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 0xAAAA;

	SPI_Init(SPI1, &SPI_InitStructure);

	/* Enable SPI1 */
	SPI_Cmd(SPI1, ENABLE);
}


/*!
****************************************************************************************************
*
*   \brief         Module (re-)initialisation function
*
*   \author        MS
*
*   \return        none
*
*   \note          Fixed baudrate for now at 5MHz 8N1
*
***************************************************************************************************/
// void HAL_SPI2_init( void )
// {
// 	/* Enable SPI clock */
// 	RCC_APB1PeriphClockCmd( RCC_APB1Periph_SPI2, ENABLE );

// 	/* Enable the GPIOB clock */
// 	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE );

// 	// Reset SPI Interface
// 	SPI_I2S_DeInit(SPI2);

// 	/* Configure the GPIOs */
// 	GPIO_InitTypeDef GPIO_InitStructure;

// 	/* Setup the SPI pins */
// 	GPIO_InitStructure.GPIO_Pin = SPI2_SCK_PIN | SPI2_MISO_PIN | SPI2_MOSI_PIN;
// 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
// 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
// 	GPIO_Init(GPIOB, &GPIO_InitStructure);

// 	SPI_InitTypeDef   SPI_InitStructure;

// 	SPI_StructInit(&SPI_InitStructure);
// 	SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
// 	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
// 	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
// 	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
// 	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
// 	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
// 	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
// 	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
// 	SPI_InitStructure.SPI_CRCPolynomial = 0xAAAA;

// 	SPI_Init(SPI2, &SPI_InitStructure);

// 	//Enable DMA1 clock
// 	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

// 	DMA_InitTypeDef     DMA_InitStructure;

// 	//Configure DMA1 - Channel5== (memory -> SPI)
// 	DMA_DeInit(DMA1_Channel5); //Set DMA registers to default values
// 	DMA_StructInit(&DMA_InitStructure);
// 	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&SPI2->DR; //Address of peripheral the DMA must map to
// 	//DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&SPITransmittedValue[0]; //Variable from which data will be transmitted
// 	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
// 	DMA_InitStructure.DMA_BufferSize = 12; //Buffer size
// 	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
// 	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
// 	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
// 	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
// 	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
// 	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
// 	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
// 	DMA_Init(DMA1_Channel5, &DMA_InitStructure); //Initialise the DMA
// 	DMA_Cmd(DMA1_Channel5, ENABLE); //Enable the DMA1 - Channel5

// 	/* Enable SPI2 */
// 	SPI_Cmd(SPI2, ENABLE);

// 	// Enable the SPI2 TX DMA requests
// 	//SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);
// 	SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, DISABLE);
// }




void HAL_SPI1_de_init( void )
{
	/* Disable SPI1 */
	SPI_Cmd(SPI1, DISABLE);

	SPI_I2S_DeInit(SPI1);

	/* Disable SPI clock */
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_SPI1, DISABLE );
}



void HAL_SPI2_de_init( void )
{
	/* Disable SPI1 */
	SPI_Cmd(SPI2, DISABLE);

	SPI_I2S_DeInit(SPI2);

	/* Disable SPI clock */
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_SPI2, DISABLE );
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
u8_t HAL_SPI1_write_and_read_data( u8_t tx_data )
{
    u8_t return_value;

	/* First lets do a dummy read to make sure that the interrupt
	flag is clear and that the buffer is empty*/

	/* Send SPI1 data */
	SPI_I2S_SendData(SPI1, tx_data);

	/* Wait for the SPI busy flag to clear */
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);

	/* Read the buffer on the SPI receive register */
	return_value = SPI_I2S_ReceiveData( SPI1 );

    return ( return_value );
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
u8_t HAL_SPI2_write_and_read_data( u8_t tx_data )
{
    u8_t return_value;

	/* First lets do a dummy read to make sure that the interrupt
	flag is clear and that the buffer is empty*/

	/* Send SPI2 data */
	SPI_I2S_SendData(SPI2, tx_data);

	/* Wait for the SPI busy flag to clear */
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY) == SET);

	/* Read the buffer on the SPI receive register */
	return_value = SPI_I2S_ReceiveData( SPI2 );

    return ( return_value );
}



/*!
****************************************************************************************************
*
*   \brief
*
*   \author        FK
*
*   \return        none
*
***************************************************************************************************/
void HAL_SPI1_disable_rx_interrupt( void )
{

}

/*!
****************************************************************************************************
*
*   \brief
*
*   \author        MS
*
*   \return        none
*
***************************************************************************************************/
void HAL_SPI1_enable_rx_interrupt( void )
{

}


/*!
****************************************************************************************************
*
*   \brief
*
*   \author        MS
*
*   \return        none
*
***************************************************************************************************/
void HAL_SPI1_enable_tx_interrupt( void )
{
}


/*!
****************************************************************************************************
*
*   \brief
*
*   \author        MS
*
*   \return        none
*
***************************************************************************************************/
void HAL_SPI1_disable_tx_interrupt( void )
{
}



/*!
****************************************************************************************************
*
*   \brief
*
*   \author        MS
*
*   \return        none
*
***************************************************************************************************/
void HAL_SPI1_clear_receive_spi_buffer( void )
{

}



/***************************************************************************************************
**                              Private Functions                                                 **
***************************************************************************************************/
/* None */


/***************************************************************************************************
**                              ISR Handlers                                                      **
***************************************************************************************************/

/****************************** END OF FILE *******************************************************/
