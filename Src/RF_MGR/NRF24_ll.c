/*! \file
*               Author: mstewart
*   \brief      NRF24 ll driver
*/
/***************************************************************************************************
**                              Includes                                                          **
***************************************************************************************************/
#include "C_defs.h"
#include "STDC.h"
#include "HAL_BRD.h"
#include "HAL_SPI.h"
#include "NRF24.h"
#include "NRF24_ll.h"



/* Module Identification for STDC_assert functionality */
#define STDC_MODULE_ID   STDC_MOD_NRF_LL


/***************************************************************************************************
**                              Data declarations and definitions                                 **
***************************************************************************************************/
u8_t NRF24_ll_write_buff_s[NRF_MAX_PAYLOAD_SIZE+1u];
u8_t NRF24_ll_read_buff_s[NRF_MAX_PAYLOAD_SIZE+1];


/*!
****************************************************************************************************
*
*   \brief         Reads several bytes from the RF chip
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
void NRF24_ll_read_registers( NRF24_instruction_et instruction, NRF24_registers_et address, u8_t read_data[], u8_t num_bytes )
{
    u8_t i = 0;
    /* Instruction and address looks good so carry out the read */

    /* pull NCS / Slave Select line low */
    NRF24_ll_spi_slave_select( LOW );

    if( ( instruction == R_RX_PAYLOAD ) || ( instruction == R_RX_PL_WID ) )
    {
        NRF24_ll_write_buff_s[0u] = instruction;
    }
    else
    {
       NRF24_ll_write_buff_s[0u] = address;
    }

    for( i = 0; i < num_bytes; i++ )
    {
        /* send dummy byte to force readback */
        NRF24_ll_write_buff_s[i+1] = 0xAA;
    }

    /* Read data from chip */
    HAL_SPI1_write_and_read_data( NRF24_ll_write_buff_s, NRF24_ll_read_buff_s, (num_bytes+1u) );

    /* Now copy across the relevant output data */
    STDC_memcpy( read_data, &NRF24_ll_read_buff_s[1u], num_bytes );

    /* pull NCS / Slave Select line high */
    NRF24_ll_spi_slave_select( HIGH );
}




/*!
*******************************************************************************
*
*   \brief          Writes several bytes of consecutive data starting at a particular register
*
*   \author         MS
*
*   \param          config - configuration in question
*   \param          instruction - Instruction to be executed
*   \param          address - address to be written to
*   \param          write_data - data to be written
*   \param          num_bytes - Number of bytes to be written
*
*
*   \return         result - pass or fail
*
*******************************************************************************/
void NRF24_ll_write_registers( NRF24_instruction_et instruction, NRF24_registers_et address, u8_t write_data[], u8_t num_bytes )
{
    /* pull NCS / Slave Select line low */
    NRF24_ll_spi_slave_select( LOW );

    /* Check if this is a register write */
    if( instruction == W_REGISTER )
    {
        /* Now we need to modify the address with the "write" bit set */
        NRF24_ll_write_buff_s[0u] = address | NRF24_WRITE_BIT; 
    }
    else
    {
        /* These registers are the instructions themselves,
        so instead of using the address, we write down the instruction */
        NRF24_ll_write_buff_s[0u] = instruction; 
    }

    STDC_memcpy( &NRF24_ll_write_buff_s[1], write_data, num_bytes );

    HAL_SPI1_write_and_read_data( NRF24_ll_write_buff_s, NRF24_ll_read_buff_s, (num_bytes+1u) );

    /* release Slave Select line */
    NRF24_ll_spi_slave_select( HIGH );
}



/*!
*******************************************************************************
*
*   \brief          Controls the slave select line
*
*   \author         MS
*
*   \return         none
*
*******************************************************************************/
void NRF24_ll_spi_slave_select( low_high_et state )
{
    HAL_BRD_NRF24_spi_slave_select( state );
}


/*!
*******************************************************************************
*
*   \brief          Controls the Chip Enable line
*
*   \author         MS
*
*   \return         none
*
*******************************************************************************/
void NRF24_ll_ce_select( low_high_et state )
{
    HAL_BRD_NRF24_set_ce_pin_state( state );
}



/***************************************************************************************************
**                              Private Functions                                                 **
***************************************************************************************************/


/****************************** END OF FILE *******************************************************/
