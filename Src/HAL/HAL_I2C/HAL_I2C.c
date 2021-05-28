/*! \file
*               Author: mstewart
*   \brief      HAL_SPI module
*/
/***************************************************************************************************
**                              Includes                                                          **
***************************************************************************************************/
/***************************************************************************************************
**                              Includes                                                          **
***************************************************************************************************/
#include "stm32f10x_rcc.h"
#include "stm32f10x_i2c.h"
#include "stm32f10x_gpio.h"
#include "misc.h"

#include "C_defs.h"
#include "STDC.h"
#include "COMPILER_defs.h"
#include "HAL_config.h"
#include "DBG_MGR.h"
#include "HAL_I2C.h"



/***************************************************************************************************
**                              Data declarations and definitions                                 **
***************************************************************************************************/
/* None */
u32_t HAL_I2C_fail_cnt_s;


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
*   \note
*
***************************************************************************************************/
void HAL_I2C1_init( void )
{
	/* Enable I2C and GPIOA clock, should be enabled anyway but just in case */
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_I2C1, ENABLE );
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE );

	I2C_SoftwareResetCmd(I2C1, ENABLE);
	I2C_SoftwareResetCmd(I2C1, DISABLE);

	/* Configure the GPIOs */
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Configure I2C_EE pins: SCL and SDA */
	GPIO_InitStructure.GPIO_Pin =  I2C1_SDA_PIN | I2C1_SCL_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(I2C1_PORT, &GPIO_InitStructure);

	I2C_InitTypeDef  I2C_InitStructure;

	/* I2C configuration */
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = 0x38;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = 100000;

	/* I2C Peripheral Enable */
	I2C_Cmd(I2C1, ENABLE);
	/* Apply I2C configuration after enabling it */
	I2C_Init(I2C1, &I2C_InitStructure);

	I2C_AcknowledgeConfig(I2C1,ENABLE);

	HAL_I2C_fail_cnt_s = 0u;
}



void HAL_I2C1_de_init( void )
{
	/* De-init the I2C module */
	I2C_DeInit(I2C1);

	/* I2C Peripheral Disable */
	I2C_Cmd(I2C1, DISABLE);

	/* Disable I2C clock*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, DISABLE);
}



void HAL_I2C1_write_single_register( u8_t dev_add, u8_t reg_add, u8_t* data )
{
	HAL_I2C1_write_registers( dev_add, reg_add, data, 1u );
}


void HAL_I2C1_write_registers( u8_t dev_add, u8_t reg_add, u8_t* data, u8_t num_bytes )
{
	I2C_GenerateSTART(I2C1, ENABLE);
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

	I2C_Send7bitAddress(I2C1, ( dev_add << 1u ), I2C_Direction_Transmitter);
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	I2C_SendData(I2C1, reg_add);
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	while( num_bytes-- )
	{
		I2C_SendData(I2C1, *data);
		while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	}

	I2C_GenerateSTOP(I2C1, ENABLE);
}


void HAL_I2C1_read_single_registers( u8_t dev_add, u8_t reg_add, u8_t* data )
{
	HAL_I2C1_read_registers( dev_add, reg_add, data, 1u );
}




void HAL_I2C1_read_registers( u8_t dev_add, u8_t reg_add, u8_t* data, u8_t num_bytes )
{
	I2C_AcknowledgeConfig(I2C1,ENABLE);
  	I2C_GenerateSTART(I2C1,ENABLE);

  	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
  	I2C_Send7bitAddress(I2C1, ( dev_add << 1u ), I2C_Direction_Transmitter);

  	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

  	I2C_SendData(I2C1,reg_add);
  	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

  	I2C_GenerateSTART(I2C1,ENABLE);
  	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

  	I2C_Send7bitAddress(I2C1, ( dev_add << 1u ), I2C_Direction_Receiver);
  	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

	while( num_bytes-- )
	{
  		while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
  		*data = I2C_ReceiveData(I2C1);
  		data ++;
	}

  	I2C_GenerateSTOP(I2C1,ENABLE);
  	I2C_AcknowledgeConfig(I2C1,DISABLE);

  	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
  	I2C_ReceiveData(I2C1);
}


///***************************************************************************************************
//**                              ISR Handlers                                                      **
//***************************************************************************************************/


///****************************** END OF FILE *******************************************************/
