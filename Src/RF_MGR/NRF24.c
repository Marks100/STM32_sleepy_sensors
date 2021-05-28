/*! \file
*               Author: mstewart
*   \brief      NRF24 module
*/
/***************************************************************************************************
**                              Includes                                                          **
***************************************************************************************************/
#include "C_defs.h"
#include "SELMATH.h"
#include "STDC.h"
#include "NVM.h"
#include "RF_MGR.h"
#include "SYSTICK_MGR.h"
#include "NRF24.h"
#include "NRF24_ll.h"
#include "NRF24_config.h"


/* Module Identification for STDC_assert functionality */
#define STDC_MODULE_ID   STDC_MOD_NRF


/***************************************************************************************************
**                              Data declarations and definitions                                 **
***************************************************************************************************/
STATIC NRF24_reg_config_st NRF24_shadow_regs_s[] = 
{ 
    { CONFIG,       1u, { 0x00 } },
	{ EN_AUTO_ACK,  1u, { 0x00 } },
	{ EN_RXADDR,    1u, { 0x00 } },
    { SETUP_AW,     1u, { 0x00 } },
	{ SETUP_RETR,   1u, { 0x00 } },
	{ RF_CH,        1u, { 0x00 } },
	{ RF_SETUP,     1u, { 0x00 } },
	{ STATUS,       1u, { 0x00 } },
	{ OBSERVE_TX,   1u, { 0x00 } },
	{ CD,           1u, { 0x00 } },
	{ RX_ADDR_P0,   5u, { 0x00, 0x00, 0x00, 0x00, 0x00 } },
	{ RX_ADDR_P1,   5u, { 0x00, 0x00, 0x00, 0x00, 0x00 } },
	{ RX_ADDR_P2,   1u, { 0x00 } },
	{ RX_ADDR_P3,   1u, { 0x00 } },
	{ RX_ADDR_P4,   1u, { 0x00 } },
	{ RX_ADDR_P5,   1u, { 0x00 } },
	{ TX_ADDR ,     5u, { 0x00, 0x00, 0x00, 0x00, 0x00 } },
	{ RX_PW_P0,     1u, { 0x00 } },
	{ RX_PW_P1,     1u, { 0x00 } },
	{ RX_PW_P2,     1u, { 0x00 } },
	{ RX_PW_P3,     1u, { 0x00 } },
	{ RX_PW_P4,     1u, { 0x00 } },
	{ RX_PW_P5,     1u, { 0x00 } },
	{ FIFO_STATUS,  1u, { 0x00 } },
	{ ACK_PLD,      1u, { 0x00 } },
	{ TX_PLD,       1u, { 0x00 } },
	{ RX_PLD,       1u, { 0x00 } },
	{ DYNPD,        1u, { 0x00 } },
	{ FEATURE,      1u, { 0x00 } }
};

/*!
****************************************************************************************************
*
*   \brief         Initialise the NRF chip with a chosen config
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
pass_fail_et NRF24_setup_transceiver( void )
{
    pass_fail_et status;
    
    status = NRF24_set_configuration( NRF24_SST_CONFIG );

    return ( status );
}


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
pass_fail_et NRF24_set_configuration( NRF24_configuration_et config )
{
    pass_fail_et returnType = PASS;

#if( RF_MGR_READBACK_REG == 1 )
    u8_t readback[5];
#endif

    if( config >= NRF24_CFG_MAX )
    {
        // invalid configuration
        STDC_basic_assert();
        returnType = PASS;
    }
    else
    {
        /* Config is OK */
    	u8_t len;
        u8_t i;
        NRF24_reg_config_st* data_p;
        
        len = NRF24_config_table_c[config].config_len;
        data_p = NRF24_config_table_c[config].dataset;

        /* Write to all registers defined in lookup table for configuration to RF chip */
        for( i=0u; i<len; i++ )
        {
            NRF24_ll_write_registers( W_REGISTER, (data_p)->reg, (data_p)->data, (data_p)->len );

#if( RF_MGR_READBACK_REG == 1 )
            /* Check each value gets written correctly where possible */
            if( NRF24_reg_static_check((data_p)->reg) == TRUE )
            {
                NRF24_ll_read_registers( R_REGISTER, (data_p)->reg, readback, (data_p)->len );
                
                if( STDC_memcompare( (data_p)->data, readback, (data_p)->len ) == FALSE )
                {
                    returnType = FAIL;
                }
            }
#endif
            data_p += 1;
        }
    }

    return ( returnType );
}



/*!
****************************************************************************************************
*
*   \brief         Check if a register is static or subject t change by the device itself
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
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
	NRF24_ll_read_registers( R_REGISTER, CONFIG, &register_val, 1 );

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
        case NRF_STANDBY_2_MODE:
        case NRF_TX_MODE:
        break;

        case NRF_RX_MODE:
            STDC_bit_set( register_val, PRIM_RX );
        break;


        default:
            break;
 	}

	NRF24_ll_write_registers( W_REGISTER, CONFIG, &register_val, 1u );

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

	NRF24_ll_write_registers( W_REGISTER, RF_CH, &channel, 1 );

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
	NRF24_ll_write_registers( FLUSH_TX, ADDRESS_NOP, &data, 1 );

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
	NRF24_ll_write_registers( FLUSH_RX, ADDRESS_NOP, &data, 1 );

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
    NRF24_ll_read_registers( R_RX_PL_WID, ADDRESS_NOP, &packet_size, 1 );

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
    NRF24_ll_read_registers( R_REGISTER, RF_SETUP, &register_val, 1 );

    STDC_clr_bit_mask( register_val, RF_PWR_MASK );
    STDC_set_bit_mask( register_val,  ( value << 1 ) );

    NRF24_ll_write_registers( W_REGISTER, RF_SETUP, &register_val, 1 );

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
    NRF24_ll_read_registers( R_REGISTER, RF_SETUP, &pa_val, 1 );

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
    NRF24_ll_read_registers( R_REGISTER, STATUS, &status, 1 );

    /* Update the shadow copy */
    NRF24_shadow_regs_s[STATUS].data[0] = status;

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
low_high_et NRF24_check_status_mask( NRF24_status_masks_et mask )
{
    u8_t status;
    low_high_et returnval = 0u;

    /* firstly grab the status byte */
    status = NRF24_get_status();

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
low_high_et NRF24_check_fifo_mask( NRF24_fifo_masks_et mask )
{
    u8_t status;
    low_high_et returnval = 0u;

    /* firstly grab the status byte */
    status = NRF24_get_FIFO_status();

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
    NRF24_ll_read_registers( R_REGISTER, FIFO_STATUS, &status, 1 );

    NRF24_shadow_regs_s[FIFO_STATUS].data[0u] = status;

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
    NRF24_ll_read_registers( R_REGISTER, RF_SETUP, &register_val, 1 );

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

    NRF24_ll_write_registers( W_REGISTER, RF_SETUP, &register_val, 1 );

    return ( PASS );
}




/*!
*******************************************************************************
*
*   \brief          Sets the TX address
*
*   \author         MS
*
*   \return
*
*******************************************************************************
*/
pass_fail_et NRF24_set_tx_address( u8_t data_pipe_address[NRF_DATA_PIPE_SIZE] )
{
    NRF24_ll_write_registers( W_REGISTER, RF_SETUP, data_pipe_address, 5u );

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
        NRF24_ll_write_registers( REUSE_TX_PL, ADDRESS_NOP, &register_val, 1u );
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
    NRF24_ll_read_registers( R_REGISTER, EN_AUTO_ACK, &register_val, 1 );

    /* Mask of the bit */
    STDC_bit_clr( register_val, pipe_num );

    STDC_set_bit_mask( register_val, ( state << pipe_num ) );

    NRF24_ll_write_registers( W_REGISTER, EN_AUTO_ACK, &register_val, 1 );

    return ( PASS );
}



/*!
*******************************************************************************
*
*   \brief          Sets the size of the receive data pipe
*
*   \author         MS
*
*   \return
*
*******************************************************************************
*/
pass_fail_et NRF24_set_pipe_payload_size( u8_t size, u8_t pipe_num )
{
    u8_t pipe_size;

    if( pipe_num > NRF_MAX_NUM_PIPES )
    {
        pipe_num = NRF_MAX_NUM_PIPES;
    }

    if( size > NRF_MAX_PAYLOAD_SIZE )
    {
        pipe_size = NRF_MAX_PAYLOAD_SIZE;
    }
    else
    {
        pipe_size = SELMATH_max_u8_t( 1u, size );
    }
    
    NRF24_ll_write_registers( W_REGISTER, ( RX_PW_P0 + pipe_num ), &pipe_size, 1u );

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
    NRF24_ll_read_registers( R_REGISTER, EN_RXADDR, &register_val, 1 );

    /* Mask of the bit */
    STDC_bit_clr( register_val, pipe_num );

    STDC_set_bit_mask( register_val, ( state << pipe_num ) );

    NRF24_ll_write_registers( W_REGISTER, EN_RXADDR, &register_val, 1 );

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
    NRF24_ll_read_registers( R_REGISTER, SETUP_RETR, &register_val, 1 );

    /* Mask of the bit */
    STDC_clr_bit_mask( register_val, ARD_MASK );
    STDC_clr_bit_mask( register_val, ARC_MASK );

    STDC_set_bit_mask( register_val, ( time << ARD_POS ) );
    STDC_set_bit_mask( register_val, ( counts << ARC_POS ) );

    NRF24_ll_write_registers( W_REGISTER, SETUP_RETR, &register_val, 1 );

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
    NRF24_ll_write_registers( W_REGISTER, ( NRF24_registers_et)( pipe_num + NRF_DATA_PIPE_OFFSET ), (u8_t*)data_pipe_address, 5 );

    NRF24_ll_write_registers( W_REGISTER, TX_ADDR, (u8_t*)data_pipe_address, 5 );
    NRF24_ll_write_registers( W_REGISTER, ( RX_PW_P0 + pipe_num ) , &payload_size, 1 );

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
    NRF24_ll_read_registers( R_REGISTER, ( NRF24_registers_et)( pipe_num + 0x0A ), (u8_t*)data_p, 5 );

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
    NRF24_ll_read_registers( R_REGISTER, STATUS, &register_val, 1 );

    STDC_clr_bit_mask( register_val, bit_mask );

    NRF24_ll_write_registers( W_REGISTER, STATUS, &register_val, 1 );

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

    /* Clear the max retry bit before sending any further data */
    NRF24_status_register_clr_bit( MAX_RT_MASK );

    /* Is the payload dynamic or static?, Find out by reading the FEATURE register to check
    if DPL is activated */
    NRF24_ll_read_registers( R_REGISTER, FEATURE, &dpl_enabled_check, 1 ) ;

    if( STDC_sample_bit_mask( dpl_enabled_check, EN_DPL_MASK ) != EN_DPL_MASK )
    {
        /* Find out the size of the payload setting for the specific pipe */
        NRF24_ll_read_registers( R_REGISTER, RX_PW_P0, &stuff_buffer_size, 1 );

        /* Mask off the size of the payload setting for the specific pipe */
        stuff_buffer_size = ( stuff_buffer_size & 0x3F );

        /* subtract the amount of "actual" data that you have from the max payload setting */
        stuff_buffer_size -= len;
    }

    /* Send register address */
    NRF24_ll_write_registers( W_TX_PAYLOAD, ADDRESS_NOP, buffer, (len+stuff_buffer_size) );

    /* toggle the CE pin to complete the RF transfer */
    NRF24_ll_ce_select(HIGH);

    /* hack a little delay in here */
    SYSTICK_MGR_delay_us(40u);

    NRF24_ll_ce_select(LOW);

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
    NRF24_ll_read_registers( R_REGISTER, FEATURE, &dpl_enabled_check, 1 ) ;

    if( STDC_sample_bit_mask( dpl_enabled_check, EN_DPL_MASK ) == EN_DPL_MASK )
    {
        /* Find out the size of the payload setting for the specific pipe */
        NRF24_ll_read_registers( R_RX_PL_WID, ADDRESS_NOP, &buffer_size, 1 );
    }
    else
    {
       /* Find out the size of the payload setting for the specific pipe */
      NRF24_ll_read_registers( R_REGISTER, RX_PW_P0, &buffer_size, 1 );

      /* Mask off the size of the payload setting for the specific pipe */
      buffer_size = ( buffer_size & 0x3F );
    }

    NRF24_ll_read_registers( R_RX_PAYLOAD, ADDRESS_NOP, buffer, buffer_size );

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
    NRF24_ll_read_registers( R_REGISTER, DYNPD, &register_val, 1 );

    /* This clears the specific DPLD bit */
    STDC_bit_clr( register_val, pipe_num );

    STDC_set_bit_mask( register_val, ( state << pipe_num ) );

    NRF24_ll_write_registers( W_REGISTER, DYNPD, (u8_t*)&register_val, 1 );

    /* That's the individual pipe dynamic payload bit set, next we need to enable the
    EN_DPL bit in the feature register */

    /* Read the feature register */
    NRF24_ll_read_registers( R_REGISTER, FEATURE, &register_val, 1 );

    /* Always enable the dynamic payload bit in the feature register */
    register_val = ( 0 | ( 1 << EN_DPL_POS ) );
    NRF24_ll_write_registers( W_REGISTER, FEATURE, (u8_t*)&register_val, 1 );

    /* Read the feature register */
    NRF24_ll_read_registers( R_REGISTER, FEATURE, &register_val, 1 );

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

    NRF24_ll_write_registers( ACTIVATE, FEATURE, (u8_t*)&register_val, 1 );

    return ( PASS );
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
    NRF24_ll_read_registers( R_REGISTER, CONFIG, &register_val, 1 );

    /* Clear the CRC enable/disable bit */
    STDC_clr_bit_mask( register_val, EN_CRC_MASK );
    STDC_clr_bit_mask( register_val, CRCO_MASK );

    STDC_set_bit_mask( register_val, ( state << EN_CRC_POS ) );
    STDC_set_bit_mask( register_val, ( crc_len << CRCO_POS ) );

    NRF24_ll_write_registers( W_REGISTER, CONFIG, (u8_t*)&register_val, 1 );
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
    NRF24_ll_read_registers( R_REGISTER, RF_SETUP, &register_val, 1 );

    /* Clear the CRC enable/disable bit */
    STDC_clr_bit_mask( register_val, CONT_WAVE_MASK );
    STDC_set_bit_mask( register_val, ( state << CONT_WAVE_POS ) );

    NRF24_ll_write_registers( W_REGISTER, RF_SETUP, (u8_t*)&register_val, 1 );
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
    NRF24_ll_read_registers( R_REGISTER, RF_SETUP, &register_val, 1 );

    /* Clear the enable/disable bit */
    STDC_clr_bit_mask( register_val, PLL_LOCK_MASK );
    STDC_set_bit_mask( register_val, ( state << PLL_LOCK_POS ) );

    NRF24_ll_write_registers( W_REGISTER, RF_SETUP, (u8_t*)&register_val, 1 );
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

    NRF24_ll_read_registers( R_REGISTER, SETUP_AW, &register_val, 1 );

    STDC_clr_bit_mask( register_val, AW_MASK );
    STDC_set_bit_mask( register_val, ( value << AW_POS ) );

    NRF24_ll_write_registers( W_REGISTER, SETUP_AW, (u8_t*)&register_val, 1 );
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

    NRF24_ll_read_registers( R_REGISTER, OBSERVE_TX, &register_val, 1 );

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
    NRF24_ll_read_registers( R_REGISTER, FEATURE, &register_val, 1 );

    /* Always enable the dynamic payload bit in the feature register */
    STDC_clr_bit_mask( register_val, EN_DYN_ACK_MASK );
    STDC_set_bit_mask( register_val, ( state << EN_DYN_ACK_POS) );
    NRF24_ll_write_registers( W_REGISTER, FEATURE, (u8_t*)&register_val, 1 );

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
	pass_fail_et status = FAIL;

    if( NRF24_check_status_mask( RF24_TX_DATA_SENT ) == HIGH )
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
	pass_fail_et status = FAIL;

    if( NRF24_check_status_mask( RF24_MAX_RETR_REACHED ) == HIGH )
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

    if( NRF24_check_fifo_mask( RF24_RX_EMPTY ) == LOW )
	{
		/* We have a packet sitting in the RX FIFO */
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
    NRF24_ll_read_registers( R_REGISTER, RF_CH, &register_val, 1 );

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
    NRF24_reg_config_st* data_p;
    u8_t len;
    u8_t i;
        
    len = NRF24_READ_CONFIG_DATA_SIZE;
    data_p = NRF24_shadow_regs_s;

    /* Write to all registers defined in lookup table for configuration to RF chip */
    for( i = 0u; i < len; i++ )
    {
        NRF24_ll_read_registers( R_REGISTER, (data_p+i)->reg, (data_p+i)->data, (data_p+i)->len );
    }
}



/***************************************************************************************************
**                              Private Functions                                                 **
***************************************************************************************************/


/****************************** END OF FILE *******************************************************/
