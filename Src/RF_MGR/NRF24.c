/*! \file
*               Author: mstewart
*   \brief      NRF24 module
*/
/***************************************************************************************************
**                              Includes                                                          **
***************************************************************************************************/
#include "STDC.h"
#include "HAL_BRD.h"
#include "HAL_SPI.h"
#include "NVM.h"
#include "RF_MGR.h"
#include "SYSTICK_MGR.h"
#include "NRF24.h"
#include "NRF24_Registers.h"



/* Module Identification for STDC_assert functionality */
#define STDC_MODULE_ID   STDC_MOD_RF

/* different data pipes that can be used :) */
STATIC const u8_t NRF24_data_pipe_default_s [5] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE};
STATIC const u8_t NRF24_data_pipe_custom_s_1[5] = {0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
STATIC const u8_t NRF24_data_pipe_custom_s_2[5] = {0xBB, 0xCC, 0xDD, 0xEE, 0xAA};
STATIC const u8_t NRF24_data_pipe_custom_s_3[5] = {0xBB, 0xCC, 0xDD, 0xEE, 0xBB};
STATIC const u8_t NRF24_data_pipe_custom_s_4[5] = {0xBB, 0xCC, 0xDD, 0xEE, 0xCC};
STATIC const u8_t NRF24_data_pipe_custom_s_5[5] = {0xBB, 0xCC, 0xDD, 0xEE, 0xDD};

STATIC u8_t       NRF24_register_readback_s[DEFAULT_CONFIGURATION_SIZE];


/***************************************************************************************************
**                              Data declarations and definitions                                 **
***************************************************************************************************/
/* None */



/*!
****************************************************************************************************
*
*   \brief         Initialise all the RF registers
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
pass_fail_et NRF24_set_configuration( NRF24_static_configuration_et config )
{
    pass_fail_et returnType = PASS;

    if( config >= NRF24_CFG_MAX )
    {
        // invalid configuration
        STDC_basic_assert();
        returnType = FAIL;
    }
    else
    {
        /* Config is OK */
    	u8_t len;
    	len = NRF24_config_c[ config ].length;

        u8_t i;

        // write to all registers defined in lookup table for configuration to RF chip
        for( i = 0u; i < len; i++ )
        {
            if( NRF24_write_registers( W_REGISTER, NRF24_config_c[config].buffer_p[i].NRF24_register, 
            &NRF24_config_c[config].buffer_p[i].register_data, 1 ) == FAIL )
            {
                /* Configuration failed :( */
                STDC_basic_assert();
                returnType = FAIL;
                break;
            }
        }
    }

    return ( returnType );
}


false_true_et NRF24_reg_static_check( NRF24_registers_et reg )
{
    false_true_et status;

    switch( reg )
    {
        case STATUS:
        case FIFO_STATUS:
        case DYNPD:
        case FEATURE:
        case ADDRESS_NOP:
        case OBSERVE_TX:
            status = FALSE;
        break;

        default:
            status = TRUE;
        break;
    }

    return( status );
}




pass_fail_et NRF24_setup_low_level_regs( void )
{
    pass_fail_et returnType = PASS;
    
    returnType = NRF24_set_configuration( NRF24_DEFAULT_CONFIG );

	/* open up the data pipe to communicate with the receiver */
	NRF24_open_write_data_pipe( 0, NRF24_data_pipe_default_s );

	/* Setup retries and dynamic ACKS */
	NRF24_set_dynamic_payloads( ENABLE, 0 );
	NRF24_setup_dynamic_ack( ENABLE );
	NRF24_set_AA_data_pipe( ENABLE, 0 );
	NRF24_setup_retries( RF24_2000_US, 10u );

    return( returnType );
}



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
pass_fail_et NRF24_read_registers( NRF24_instruction_et instruction, NRF24_registers_et address, u8_t read_data[], u8_t num_bytes )
{
    /* assume pass until prove otherwise */
    pass_fail_et result = PASS;
    u8_t i = 0;

    /* Instruction and address looks good so carry out the read */

    /* pull NCS / Slave Select line low */
    NRF24_spi_slave_select( LOW );

    if( ( instruction == R_RX_PAYLOAD ) || ( instruction == R_RX_PL_WID ) )
    {
        /* Send the instruction as the address */
        HAL_SPI1_write_and_read_data( instruction );
    }
    else
    {
        /* Send the address */
        HAL_SPI1_write_and_read_data( address );
    }

    /* Writes data to chip */
    for( i = 0; i < num_bytes; i++ )
    {
        /* send dummy byte to force readback */
        read_data[ i ] = (u8_t)HAL_SPI1_write_and_read_data( 0xFF );
    }

    /* pull NCS / Slave Select line high */
    NRF24_spi_slave_select( HIGH );

    return( result );
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
*******************************************************************************
*/
pass_fail_et NRF24_write_registers( NRF24_instruction_et instruction, NRF24_registers_et address, u8_t write_data[], u8_t num_bytes )
{
    // assume pass until proven otherwise
    pass_fail_et result = PASS;
    u8_t read_back_register[5] = {0u,};
    u8_t i;
    u8_t modified_address;

    /* pull NCS / Slave Select line low */
    NRF24_spi_slave_select( LOW );

    /* Check if this is a register write */
    if( instruction == W_REGISTER )
    {
        /* Now we need to modify the address with the "write" bit set */
        modified_address = address | NRF24_WRITE_BIT;

        /* Send register address */
        HAL_SPI1_write_and_read_data( modified_address );
    }
    else
    {
        /* These registers are the instructions themselves,
        so instead of using the address, we write down the instruction */
        HAL_SPI1_write_and_read_data( instruction );
    }

    /* Write all data to chip */
    for( i = 0; i < num_bytes; i++ )
    {
        HAL_SPI1_write_and_read_data( write_data[ i ] );
    }

    /* release Slave Select line */
    NRF24_spi_slave_select( HIGH );

    if( ( instruction == W_REGISTER ) && ( NRF24_reg_static_check( address ) == TRUE ) )
    {
        /* Now read back the register to ensure it was written succesfully,
        NOTE!! We can only read back the registers that we can write data to */
        NRF24_read_registers( R_REGISTER, address, read_back_register, num_bytes );

        /* compare actual values with expected values */
        if( STDC_memcompare( read_back_register, write_data, num_bytes ) == FALSE )
        {
          result = FAIL;
          STDC_basic_assert();
        }
    }

    return( result );
}



/*!
*******************************************************************************
*
*   \brief          Powers up or down the RF chip by using the SPI command ( Register 0 ( CONFIG ) )
*
*   \author         MS
*
*   \return         result - pass or fail
*
*******************************************************************************
*/
pass_fail_et NRF24_set_low_level_mode( NRF24_low_level_mode_et mode )
{
 	u8_t register_val;

 	/* Read back the current register status, modify it and then rewrite it back down */
	NRF24_read_registers( R_REGISTER, CONFIG, &register_val, 1 );

 	/* Power down mode is the only mode that sets the PWR_UP bit low, so by default set it high */
    STDC_bit_set( register_val, PWR_UP );

 	/* RX mode is the only mode thats sets the PRIM RX bit high so lets set it low by default */
    STDC_bit_clr( register_val, PRIM_RX );

 	switch( mode )
 	{
        case NRF_POWER_DOWN_MODE:
            STDC_bit_clr( register_val, PWR_UP );
            break;

        case NRF_STANDBY_1_MODE:
            break;

        case NRF_STANDBY_2_MODE:
            break;

        case NRF_RX_MODE:
            STDC_bit_set( register_val, PRIM_RX );
            break;

        case NRF_TX_MODE:
            break;

        default:
            break;
 	}

	NRF24_write_registers( W_REGISTER, CONFIG, &register_val, 1u );

    return( PASS );
}




/*!
*******************************************************************************
*
*   \brief          Sets the radio chanel by using the SPI command ( Register 0 ( CONFIG ) )
*
*   \author         MS
*
*   \return         result - pass or fail
*
*******************************************************************************
*/
pass_fail_et NRF24_set_channel( u8_t channel )
{
 	/* The channel does not need to be masked but bit 7 always needs to be 0 */
	if( channel > NRF_MAX_CHANNEL_SELECTION )
	{
	  	channel = NRF_MAX_CHANNEL_SELECTION;
	}

	NRF24_write_registers( W_REGISTER, RF_CH, &channel, 1 );

    return( PASS );
}




/*!
*******************************************************************************
*
*   \brief          Flush the RF TX buffers using the SPI command
*
*   \author         MS
*
*   \return         result - pass or fail
*
*******************************************************************************
*/
pass_fail_et NRF24_flush_tx( void )
{
    u8_t data = NOP;
	NRF24_write_registers( FLUSH_TX, ADDRESS_NOP, &data, 1 );

    return ( PASS );
}



/*!
*******************************************************************************
*
*   \brief          Flush the RF RX buffers using the SPI command
*
*   \author         MS
*
*   \return         result - pass or fail
*
*******************************************************************************
*/
pass_fail_et NRF24_flush_rx( void )
{
    u8_t data = NOP;
	NRF24_write_registers( FLUSH_RX, ADDRESS_NOP, &data, 1 );

    return ( PASS );
}




/*!
*******************************************************************************
*
*   \brief          Grab the curent Dynamic Payload Length ( DPL )
*
*   \author         MS
*
*   \return         Payload size in bytes
*
*******************************************************************************
*/
u8_t NRF24_get_DPL_size(void)
{
    u8_t packet_size = 0u;

    /* Read back the current register status, modify it and then rewrite it back down */
    NRF24_read_registers( R_RX_PL_WID, ADDRESS_NOP, &packet_size, 1 );

    return ( packet_size );
}




/*!
*******************************************************************************
*
*   \brief          Set the output TX power
*
*   \author         MS
*
*   \return
*
*******************************************************************************
*/
pass_fail_et NRF24_set_PA_TX_power( NRF24_power_level_et value)
{
    u8_t register_val;

    /* Read back the current register status, modify it and then rewrite it back down */
    NRF24_read_registers( R_REGISTER, RF_SETUP, &register_val, 1 );

    STDC_clr_bit_mask( register_val, RF_PWR_MASK );
    STDC_set_bit_mask( register_val,  ( value << 1 ) );

    NRF24_write_registers( W_REGISTER, RF_SETUP, &register_val, 1 );

    return ( PASS );
}



/*!
*******************************************************************************
*
*   \brief          Grab the current output TX power
*
*   \author         MS
*
*   \return
*
*******************************************************************************
*/
NRF24_power_level_et NRF24_get_PA_TX_power(void)
{
    u8_t pa_val;

    /* Read back the current register status, modify it and then rewrite it back down */
    NRF24_read_registers( R_REGISTER, RF_SETUP, &pa_val, 1 );

    /* Pull out the power level */
    pa_val = ( ( STDC_sample_bit_mask( pa_val, RF_PWR_MASK ) ) >> 1 );

    return ( ( NRF24_power_level_et)pa_val );
}




/*!
*******************************************************************************
*
*   \brief          Grab the current status of the RF chip
*
*   \author         MS
*
*   \return
*
*******************************************************************************
*/
u8_t NRF24_get_status(void)
{
    u8_t status = 0;

    /* Read back the current register status, modify it and then rewrite it back down */
    NRF24_read_registers( R_REGISTER, STATUS, &status, 1 );

    /* Update the shadow copy */
    NRF24_register_readback_s[STATUS] = status;

    return ( status );
}



/*!
*******************************************************************************
*
*   \brief          Grab the current status of the RF chip
*
*   \author         MS
*
*   \return
*
*******************************************************************************
*/
low_high_et NRF24_check_status_mask( NRF24_status_masks_et mask, u8_t* data_p )
{
    u8_t status;
    low_high_et returnval = 0u;

    /* firstly grab the status byte */
    status = NRF24_get_status();
    *data_p =  status;

    switch( mask ) 
    {
        case RF24_RX_DATA_READY:
            returnval = ( STDC_sample_bit_mask( status, RX_DR_MASK ) >> RX_DR_POS );
            break;

        case RF24_TX_DATA_SENT:
            returnval = ( STDC_sample_bit_mask( status, TX_DS_MASK ) >> TX_DS_POS );
            break;

        case RF24_MAX_RETR_REACHED:
            returnval = ( STDC_sample_bit_mask( status, MAX_RT_MASK ) >> MAX_RT_POS );
            break;

        case RF24_RX_PIPE_DATA_NUM:
            status = ( STDC_sample_bit_mask( status, RX_P_NO_MASK ));
            
            if( ( status & RX_P_NO_MASK ) == RX_P_NO_MASK )
            {
                returnval = HIGH;
            }
            else
            {
                returnval = LOW;
            }
            break;

        case RF24_TX_FIFO_FULL:
            returnval = ( STDC_sample_bit_mask( status, TX_FULL_MASK ) >> TX_FULL_POS );
            break;

        default:
            break;
    }

    return ( returnval );
}




/*!
*******************************************************************************
*
*   \brief          Grab the current fifo of the RF chip
*
*   \author         MS
*
*   \return
*
*******************************************************************************
*/
low_high_et NRF24_check_fifo_mask( NRF24_fifo_masks_et mask, u8_t* data_p )
{
    u8_t status;
    low_high_et returnval = 0u;

    /* firstly grab the status byte */
    status = NRF24_get_FIFO_status();
    *data_p =  status;

    switch( mask )
    {
        case RF24_RX_EMPTY:
            returnval = ( STDC_sample_bit_mask( status, RX_EMPTY_MASK ) >> RX_EMPTY_POS );
            break;

        case RF24_RX_FULL:
            returnval = ( STDC_sample_bit_mask( status, RX_FULL_MASK ) >> RX_FULL_POS );
            break;

        case RF24_TX_EMPTY:
            returnval = ( STDC_sample_bit_mask( status, TX_EMPTY_MASK ) >> TX_EMPTY_POS );
            break;

        case RF24_TX_FULL:
            returnval = ( STDC_sample_bit_mask( status, TX_FIFO_FULL_MASK ) >> FIFO_FULL_POS );
            break;

        case RF24_TX_REUSE:
            returnval = ( STDC_sample_bit_mask( status, TX_REUSE_MASK ) >> TX_REUSE_POS );
            break;

        default:
            break;
    }

    return ( returnval );

}



/*!
*******************************************************************************
*
*   \brief          Grab the current status of the RF chip
*
*   \author         MS
*
*   \return
*
*******************************************************************************
*/
u8_t NRF24_get_FIFO_status(void)
{
    u8_t status = 0u;

    /* Read back the current register status, modify it and then rewrite it back down */
    NRF24_read_registers( R_REGISTER, FIFO_STATUS, &status, 1 );

    NRF24_register_readback_s[FIFO_STATUS] = status;

    return ( status );
}



/*!
*******************************************************************************
*
*   \brief          Sets the RF air data rate
*
*   \author         MS
*
*   \return
*
*******************************************************************************
*/
pass_fail_et NRF24_set_rf_data_rate( NRF24_air_data_rate_et value )
{
    u8_t register_val;

    /* Read back the current register status, modify it and then rewrite it back down */
    NRF24_read_registers( R_REGISTER, RF_SETUP, &register_val, 1 );

    /* Mask of the data rate */
    STDC_clr_bit_mask( register_val, RF_DR_MASK ); 

    switch( value )
    {
        case RF24_250KBPS:
            STDC_bit_set( register_val, RF_DR_LOW_MASK );
            break;

        case RF24_1MBPS:
            break;

        case RF24_2MBPS:
            STDC_bit_set( register_val, RF_DR_HIGH_MASK );
            break;

        default:
            break;
    }

    NRF24_write_registers( W_REGISTER, RF_SETUP, &register_val, 1 );

    return ( PASS );
}





/*!
*******************************************************************************
*
*   \brief          Sets the REUSE TX payload bit
*
*   \author         MS
*
*   \return
*
*******************************************************************************
*/
pass_fail_et NRF24_set_reuse_tx_payload( disable_enable_et state )
{
    u8_t register_val;
    register_val = NOP;

    if( state == ENABLE_ )
    {
        NRF24_write_registers( REUSE_TX_PL, ADDRESS_NOP, &register_val, 1u );
    }
    else
    {
        /* Flush the TX buffer and this will reset the REUSE bit */
        NRF24_flush_tx();
    }

    return ( PASS );
}



/*!
*******************************************************************************
*
*   \brief          Sets the auto acknowledgement on a specific data pipe
*
*   \author         MS
*
*   \return
*
*******************************************************************************
*/
pass_fail_et NRF24_set_AA_data_pipe( disable_enable_et state, u8_t pipe_num )
{
    u8_t register_val;

    if( pipe_num > NRF_MAX_NUM_PIPES )
    {
        pipe_num = NRF_MAX_NUM_PIPES;
    }

    /* Read back the current register status, modify it and then rewrite it back down */
    NRF24_read_registers( R_REGISTER, EN_AUTO_ACK, &register_val, 1 );

    /* Mask of the bit */
    STDC_bit_clr( register_val, pipe_num );

    STDC_set_bit_mask( register_val, ( state << pipe_num ) );

    NRF24_write_registers( W_REGISTER, EN_AUTO_ACK, &register_val, 1 );

    return ( PASS );
}



/*!
*******************************************************************************
*
*   \brief          Enables or disables a specific data pipe
*
*   \author         MS
*
*   \return
*
*******************************************************************************
*/
pass_fail_et NRF24_enable_data_pipe( disable_enable_et state, u8_t pipe_num )
{
    u8_t register_val;

    if( pipe_num > NRF_MAX_NUM_PIPES )
    {
        pipe_num = NRF_MAX_NUM_PIPES;
    }

    /* Read back the current register status, modify it and then rewrite it back down */
    NRF24_read_registers( R_REGISTER, EN_RXADDR, &register_val, 1 );

    /* Mask of the bit */
    STDC_bit_clr( register_val, pipe_num );

    STDC_set_bit_mask( register_val, ( state << pipe_num ) );

    NRF24_write_registers( W_REGISTER, EN_RXADDR, &register_val, 1 );

    return ( PASS );
}


/*!
*******************************************************************************
*
*   \brief          Setup the retransmit time and count
*
*   \author         MS
*
*   \return
*
*******************************************************************************
*/
pass_fail_et NRF24_setup_retries( NRF24_retransmitt_time_et time, u8_t counts )
{
    u8_t register_val;

    if( counts > NRF_MAX_NUM_RETRIES )
    {
        counts = NRF_MAX_NUM_RETRIES;
    }

    /* Read back the current register status, modify it and then rewrite it back down */
    NRF24_read_registers( R_REGISTER, SETUP_RETR, &register_val, 1 );

    /* Mask of the bit */
    STDC_clr_bit_mask( register_val, ARD_MASK );
    STDC_clr_bit_mask( register_val, ARC_MASK );

    STDC_set_bit_mask( register_val, ( time << ARD_POS ) );
    STDC_set_bit_mask( register_val, ( counts << ARC_POS ) );

    NRF24_write_registers( W_REGISTER, SETUP_RETR, &register_val, 1 );

    return ( PASS );
}







/*!
*******************************************************************************
*
*   \brief          Opens a pipe to communicate over RF link
*
*   \author         MS
*
*   \return
*
*******************************************************************************
*/
pass_fail_et NRF24_open_write_data_pipe( u8_t pipe_num, const u8_t* data_pipe_address )
{
    u8_t payload_size = 32u;

    /* 6 pipes can be opened, let the user select from 0 - 5 and auto correct their
    selection if we need to */
    if( pipe_num > NRF_MAX_NUM_PIPES )
    {
        pipe_num = NRF_MAX_NUM_PIPES;
    }

    /* we will add an offset of "0x0A" onto the pipe number as this is the starting address of
    the pipe registers and always make the address of the pipe 5 bytes long*/
    NRF24_write_registers( W_REGISTER, ( NRF24_registers_et)( pipe_num + NRF_DATA_PIPE_OFFSET ), (u8_t*)data_pipe_address, 5 );

    NRF24_write_registers( W_REGISTER, TX_ADDR, (u8_t*)data_pipe_address, 5 );
    NRF24_write_registers( W_REGISTER, ( RX_PW_P0 + pipe_num ) , &payload_size, 1 );

    return ( PASS );
}



/*!
*******************************************************************************
*
*   \brief          Opens a pipe to communicate over RF link
*
*   \author         MS
*
*   \return
*
*******************************************************************************
*/
pass_fail_et NRF24_read_data_pipe( u8_t pipe_num, const u8_t* data_p )
{
    /* 6 pipes can be opened, let the user select from 0 - 5 and auto correct their
    selection if we need to */
    if( pipe_num > NRF_MAX_NUM_PIPES  )
    {
        pipe_num = NRF_MAX_NUM_PIPES ;
    }

    /* we will add an offset of "0x0A" onto the pipe number as this is the starting address of
    the pipe registers and always make the address of the pipe 5 bytes long*/
    NRF24_read_registers( R_REGISTER, ( NRF24_registers_et)( pipe_num + 0x0A ), (u8_t*)data_p, 5 );

    return ( PASS );
}




/*!
*******************************************************************************
*
*   \brief          Opens a pipe to communicate over RF link
*
*   \author         MS
*
*   \return
*
*******************************************************************************
*/
pass_fail_et NRF24_status_register_clr_bit( u8_t bit_mask )
{
    u8_t register_val;

    /* Read back the current register status, modify it and then rewrite it back down */
    NRF24_read_registers( R_REGISTER, STATUS, &register_val, 1 );

    STDC_clr_bit_mask( register_val, bit_mask );

    NRF24_write_registers( W_REGISTER, STATUS, &register_val, 1 );

    return ( PASS );
}



/*!
*******************************************************************************
*
*   \brief          Send the payload over the RF link
*
*   \author         MS
*
*   \return
*
*******************************************************************************
*/
pass_fail_et NRF24_send_payload( u8_t* buffer, u8_t len )
{
    u8_t stuff_buffer_size;
    u8_t dpl_enabled_check;
    disable_enable_et dpl_enabled = DISABLE_;

    /* Clear the max retry bit before sending any further data */
    NRF24_status_register_clr_bit( MAX_RT_MASK );

    /* Is the payload dynamic or static?, Find out by reading the FEATURE register to check
    if DPL is activated */
    NRF24_read_registers( R_REGISTER, FEATURE, &dpl_enabled_check, 1 ) ;

    if( STDC_sample_bit_mask( dpl_enabled_check, EN_DPL_MASK ) == EN_DPL_MASK )
    {
        dpl_enabled = ENABLE_;
    }
    else
    {
        /* Find out the size of the payload setting for the specific pipe */
        NRF24_read_registers( R_REGISTER, RX_PW_P0, &stuff_buffer_size, 1 );

        /* Mask off the size of the payload setting for the specific pipe */
        stuff_buffer_size = ( stuff_buffer_size & 0x3F );

        /* subtract the amount of "actual" data that you have from the max payload setting */
        stuff_buffer_size -= len;
    }

    /* pull NCS / Slave Select line low */
    NRF24_spi_slave_select( LOW );

    /* Send register address */
    HAL_SPI1_write_and_read_data( W_TX_PAYLOAD );

    /* Write all the valid data to the TX_buffer */
    while( len -- )
    {
        HAL_SPI1_write_and_read_data( *buffer++ );
    }

    if( dpl_enabled != ENABLE_ )
    {
        /* Dynamic packet length is not set so we need to "pad" the rest of the
        transmission to make it up to 32 bytes */
        while( stuff_buffer_size -- )
        {
            HAL_SPI1_write_and_read_data( 0x00 );
        }
    }

    /* release NCS / Slave Select line high */
    NRF24_spi_slave_select( HIGH );

    SYSTICK_MGR_delay_us(10);

    /* toggle the CE pin to complete the RF transfer */
    NRF24_ce_select(HIGH);

    /* hack a little delay in here */
    SYSTICK_MGR_delay_us(40);

    NRF24_ce_select(LOW);

    return ( PASS );
}






/*!
*******************************************************************************
*
*   \brief          grab the payload that has come over the RF link
*
*   \author         MS
*
*   \return
*
*******************************************************************************
*/
pass_fail_et NRF24_get_payload( u8_t* buffer )
{
    u8_t buffer_size;
    u8_t dpl_enabled_check;

    /* Is the payload dynamic or static?, Find out by reading the FEATURE register to check
    if DPL is activated */
    NRF24_read_registers( R_REGISTER, FEATURE, &dpl_enabled_check, 1 ) ;

    if( STDC_sample_bit_mask( dpl_enabled_check, EN_DPL_MASK ) == EN_DPL_MASK )
    {
        /* Find out the size of the payload setting for the specific pipe */
        NRF24_read_registers( R_RX_PL_WID, ADDRESS_NOP, &buffer_size, 1 );
    }
    else
    {
       /* Find out the size of the payload setting for the specific pipe */
      NRF24_read_registers( R_REGISTER, RX_PW_P0, &buffer_size, 1 );

      /* Mask off the size of the payload setting for the specific pipe */
      buffer_size = ( buffer_size & 0x3F );
    }

    /* pull NCS / Slave Select line low */
    NRF24_spi_slave_select( LOW );

    /* Send register address */
    HAL_SPI1_write_and_read_data( R_RX_PAYLOAD );

    /* Write all the valid data to the TX_buffer */

    while( buffer_size -- )
    {
        *buffer++ = HAL_SPI1_write_and_read_data( 0xAA );
    }

     /* release NCS / Slave Select line high */
    NRF24_spi_slave_select( HIGH );

    return ( PASS );
}



/*!
*******************************************************************************
*
*   \brief          Enables dynamic payloads
*
*   \author         MS
*
*   \return
*
*******************************************************************************
*/
pass_fail_et NRF24_set_dynamic_payloads( disable_enable_et state, u8_t pipe_num )
{
    u8_t register_val;
    pass_fail_et return_val = PASS;

    if( pipe_num > NRF_MAX_NUM_PIPES )
    {
        pipe_num = NRF_MAX_NUM_PIPES;
    }

    /* Firstly set the Dynamic payload bit in the DYNPD register */
    NRF24_read_registers( R_REGISTER, DYNPD, &register_val, 1 );

    /* This clears the specific DPLD bit */
    STDC_bit_clr( register_val, pipe_num );

    STDC_set_bit_mask( register_val, ( state << pipe_num ) );

    NRF24_write_registers( W_REGISTER, DYNPD, (u8_t*)&register_val, 1 );

    /* That's the individual pipe dynamic payload bit set, next we need to enable the
    EN_DPL bit in the feature register */

    /* Read the feature register */
    NRF24_read_registers( R_REGISTER, FEATURE, &register_val, 1 );

    /* Always enable the dynamic payload bit in the feature register */
    register_val = ( 0 | ( 1 << EN_DPL_POS ) );
    NRF24_write_registers( W_REGISTER, FEATURE, (u8_t*)&register_val, 1 );

    /* Read the feature register */
    NRF24_read_registers( R_REGISTER, FEATURE, &register_val, 1 );

    return ( return_val );
}



/*!
*******************************************************************************
*
*   \brief          Enables dynamic payloads
*
*   \author         MS
*
*   \return
*
*******************************************************************************
*/
pass_fail_et NRF24_toggle_features_register( void )
{
    u8_t register_val = 0x73;

    NRF24_write_registers( ACTIVATE, FEATURE, (u8_t*)&register_val, 1 );

    return ( PASS );
}



 /*!
*******************************************************************************
*
*   \brief          Reads back specifig registers for debug  purposes
*
*   \author         MS
*
*   \return
*
*******************************************************************************
*/
pass_fail_et NRF24_read_all_registers( u8_t* data_p )
{
	pass_fail_et return_val = PASS;

    u8_t i;

    for( i = 0u; i < DEFAULT_CONFIGURATION_SIZE; i++ )
    {
        // Read all registers
        if( NRF24_read_registers( R_REGISTER, NRF24_config_c[NRF24_DEFAULT_CONFIG].buffer_p[i].NRF24_register, &data_p[i], 1 ) == FAIL )
        {
            /* Configuration failed :( */
            STDC_basic_assert();

            return_val = FAIL;
        }
    }

	return ( return_val );
}





/*!
*******************************************************************************
*
*   \brief          sets up the low level payload to be transmitted
*
*   \author         MS
*
*   \return
*
*******************************************************************************
*/
void NRF24_setup_CRC_scheme( disable_enable_et state, NRF24_crclength_et crc_len )
{
    u8_t register_val;

    if( crc_len > RF24_CRC_16 )
    {
        crc_len = RF24_CRC_16;
    }

    /* Find out the size of the payload setting for the specific pipe */
    NRF24_read_registers( R_REGISTER, CONFIG, &register_val, 1 );

    /* Clear the CRC enable/disable bit */
    STDC_clr_bit_mask( register_val, EN_CRC_MASK );
    STDC_clr_bit_mask( register_val, CRCO_MASK );

    STDC_set_bit_mask( register_val, ( state << EN_CRC_POS ) );
    STDC_set_bit_mask( register_val, ( crc_len << CRCO_POS ) );

    NRF24_write_registers( W_REGISTER, CONFIG, (u8_t*)&register_val, 1 );
}



/*!
*******************************************************************************
*
*   \brief          sets up the constant RF wave
*
*   \author         MS
*
*   \return
*
*******************************************************************************
*/
void NRF24_setup_constant_wave( disable_enable_et state )
{
    u8_t register_val;

    /* Find out the size of the payload setting for the specific pipe */
    NRF24_read_registers( R_REGISTER, RF_SETUP, &register_val, 1 );

    /* Clear the CRC enable/disable bit */
    STDC_clr_bit_mask( register_val, CONT_WAVE_MASK );
    STDC_set_bit_mask( register_val, ( state << CONT_WAVE_POS ) );

    NRF24_write_registers( W_REGISTER, RF_SETUP, (u8_t*)&register_val, 1 );
}



/*!
*******************************************************************************
*
*   \brief          sets up the pll
*
*   \author         MS
*
*   \return
*
*******************************************************************************
*/
void NRF24_setup_pll( disable_enable_et state )
{
    u8_t register_val;

    /* Find out the size of the payload setting for the specific pipe */
    NRF24_read_registers( R_REGISTER, RF_SETUP, &register_val, 1 );

    /* Clear the enable/disable bit */
    STDC_clr_bit_mask( register_val, PLL_LOCK_MASK );
    STDC_set_bit_mask( register_val, ( state << PLL_LOCK_POS ) );

    NRF24_write_registers( W_REGISTER, RF_SETUP, (u8_t*)&register_val, 1 );
}


/*!
*******************************************************************************
*
*   \brief          sets up the data pipe address widths
*
*   \author         MS
*
*   \return
*
*******************************************************************************
*/
void NRF24_setup_address_widths( NRF24_address_width_et value )
{
    u8_t register_val;

    NRF24_read_registers( R_REGISTER, SETUP_AW, &register_val, 1 );

    STDC_clr_bit_mask( register_val, AW_MASK );
    STDC_set_bit_mask( register_val, ( value << AW_POS ) );

    NRF24_write_registers( W_REGISTER, SETUP_AW, (u8_t*)&register_val, 1 );
}



/*!
*******************************************************************************
*
*   \brief          Gets the number of retries that took place before an
*                   ACK was received for a TX
*
*   \author         MS
*
*   \return
*
*******************************************************************************
*/
u8_t NRF24_get_retry_count( void )
{
    u8_t register_val;

    NRF24_read_registers( R_REGISTER, OBSERVE_TX, &register_val, 1 );

    register_val = STDC_sample_bit_mask( register_val, ARC_CNTMASK );

    return ( register_val );
}




/*!
*******************************************************************************
*
*   \brief          Enables dynamic acks
*
*   \author         MS
*
*   \return
*
*******************************************************************************
*/
pass_fail_et NRF24_setup_dynamic_ack( disable_enable_et state )
{
    u8_t register_val;
    pass_fail_et return_val  = PASS;

    /* Read the feature register */
    NRF24_read_registers( R_REGISTER, FEATURE, &register_val, 1 );

    /* Always enable the dynamic payload bit in the feature register */
    STDC_clr_bit_mask( register_val, EN_DYN_ACK_MASK );
    STDC_set_bit_mask( register_val, ( state << EN_DYN_ACK_POS) );
    NRF24_write_registers( W_REGISTER, FEATURE, (u8_t*)&register_val, 1 );

    return ( return_val );
}




/*!
****************************************************************************************************
*
*   \brief         Handles ACKS 
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
pass_fail_et NRF24_handle_acks( void )
{
    u8_t status_reg;
	pass_fail_et status = FAIL;

    if( NRF24_check_status_mask( RF24_TX_DATA_SENT, &status_reg ) == HIGH )
    {
        /* Clear the Data sent bit or else we cant send any more data */
        NRF24_status_register_clr_bit( TX_DS_MASK );

        status = PASS;
    }

    return( status );
}






/*!
****************************************************************************************************
*
*   \brief         Handles failed tx's
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
pass_fail_et NRF24_handle_tx_failures( void )
{
    u8_t status_reg;
	pass_fail_et status = FAIL;

    if( NRF24_check_status_mask( RF24_MAX_RETR_REACHED, &status_reg ) == HIGH )
    {
        /* Clear the max retry bit before sending any further data */
        NRF24_status_register_clr_bit( MAX_RT_MASK );
        status = PASS;
    }
    return( status );
}



/*!
****************************************************************************************************
*
*   \brief         Checks if we have recieved a RF packet
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
false_true_et NRF24_check_for_packet_received( void )
{
	false_true_et status = FALSE;
	u8_t fifo;
    u8_t status_reg;

    status_reg = NRF24_get_status();

	if( ( NRF24_check_status_mask( RF24_RX_DATA_READY, &status_reg ) == HIGH ) 
    || ( NRF24_check_fifo_mask( RF24_RX_EMPTY ,&fifo ) == LOW ) )
	{
		/* We have received a packet */
		status = TRUE;
	}

    return ( status );
}




/*!
****************************************************************************************************
*
*   \brief         Completely flush out all the TX and RX buffers on the NRF chip
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
void NRF24_complete_flush( void )
{
    NRF24_flush_rx();
    NRF24_flush_tx();

    NRF24_status_register_clr_bit( TX_DS_MASK );
    NRF24_status_register_clr_bit( TX_DS_MASK );
    NRF24_status_register_clr_bit( TX_DS_MASK );
    
    NRF24_status_register_clr_bit( MAX_RT_MASK );
}





/*!
****************************************************************************************************
*
*   \brief         Self Test the NRF chip
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
pass_fail_et NRF24_self_check( void )
{
    false_true_et status = FAIL;
    u8_t register_val;

    /* Read back the current register and check the value */
    NRF24_read_registers( R_REGISTER, RF_CH, &register_val, 1 );

    if( register_val == NRF_DEF_CHANNEL_SELECTION )
    {
        status = PASS;
    }

    return( status );
}


/*!
****************************************************************************************************
*
*   \brief         Reads all the internal regs of the Rf chip
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
void NRF24_update_shadow_regs( void )
{
    /* After all the setup read back the regs */
	NRF24_read_all_registers( NRF24_register_readback_s );
}



void NRF24_spi_slave_select( low_high_et state )
{
    HAL_BRD_NRF24_spi_slave_select( state );
}


void NRF24_ce_select( low_high_et state )
{
    HAL_BRD_NRF24_set_ce_pin_state( state );
}




/***************************************************************************************************
**                              Private Functions                                                 **
***************************************************************************************************/


/****************************** END OF FILE *******************************************************/
