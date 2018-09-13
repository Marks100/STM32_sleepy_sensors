/*! \file
*               $Revision: 16923 $
*
*               $Author: mstewart $
*
*               $Date: 2014-01-16 15:40:40 +0000 (Thu, 16 Jan 2014) $
*
*               $HeadURL:
*
*   \brief      RF module
*/
/***************************************************************************************************
**                              Includes                                                          **
***************************************************************************************************/
#ifndef UNIT_TEST
#endif

#ifdef GCC_TEST
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#endif // GCC_TEST

#include "C_defs.h"
#include "STDC.h"
#include "COMPILER_defs.h"
#include "HAL_BRD.h"
#include "HAL_SPI.h"
#include "main.h"
#include "RFM69.h"
#include "RFM69_Registers.h"


#define STDC_MODULE_ID   STDC_MOD_RF_DECODE

//STATIC u8_t  RF_status_register;

STATIC RFM69_data_packet_st RFM69_data_packet_s;

STATIC u8_t send_data[RFM69_MAX_PAYLOAD_LEN] =
{
    1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,
    28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,
    52,53,54,55,56,57,58,59,60,61,62,63,64
};



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
*   \note
*
***************************************************************************************************/
void RFM69_wakeup_and_send( void )
{
	/* power up the RF chip */
	RFM69_set_enable_pin_state( HIGH );
    RFM69_set_reset_pin_state( LOW );

    /* Give the RF chip time to stabilise */
    delay_us(1000);

	/* Go through the elements and reset them to 0xFF */
	STDC_memset( &RFM69_data_packet_s, 0xFF, sizeof( RFM69_data_packet_st ) );

	/* Firstly check that the version number is correct */
	if( RFM69_read_version_num() != RFM69_VERSION )
	{
	   STDC_basic_assert();
	}

	if( RFM69_read_reserved_registers() == PASS )
	{
		/* Fire down a config of registers */
		RFM69_set_configuration( RFM69_433Mhz_OOK, RFM69_433Mhz_CONFIGURATION_SIZE );

		/* Put the chip into standby mode ( should be be default */
		RFM69_set_operating_mode( RFM69_STANDBY_MODE );

		/* this sets DIO1 to be "Packet Sent" indicator */
		RFM69_set_DIO_mapping( 0, RFM69_DIO_MODE_0 );
		RFM69_set_DIO_mapping( 1, RFM69_DIO_MODE_1 );
		RFM69_set_DIO_mapping( 2, RFM69_DIO_MODE_0 );
		RFM69_set_DIO_mapping( 3, RFM69_DIO_MODE_1 );
		RFM69_set_DIO_mapping( 4, RFM69_DIO_MODE_0 );
		RFM69_set_DIO_mapping( 5, RFM69_DIO_MODE_3 );
	}
	else
	{
		STDC_basic_assert();
	}

	/* Fill the buffer */
	RFM69_Send_frame( send_data, sizeof( send_data ) );

	/* Put the chip into TX mode */
	RFM69_set_operating_mode( RFM69_TRANSMIT_MODE );

	/* Put the chip into Standby mode for the next time round as this is the mode
	that allows us to fill the FIFO, this also clears the old FIFO */
	//RFM69_set_operating_mode( RFM69_STANDBY_MODE );

	//RFM69_read_FIFO_register();
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
pass_fail_et RFM69_set_configuration( RFM69_static_configuration_et config, u16_t len )
{
    pass_fail_et returnType = PASS;

    if( config >= RFM69_CFG_MAX )
    {
        // invalid configuration
        STDC_basic_assert();
    }
    else
    {
        /* Config is OK */

        u8_t i;

        for( i = 0u; i < len; i++ )
        {
            // write to all registers defined in lookup table for configuration to RFM69 chip
            if( RFM69_write_registers( WRITE_TO_CHIP, RFM69_config_c[config].buffer_p[i].RFM69_register, &RFM69_config_c[config].buffer_p[i].register_data, 1 ) == FAIL )
            {
                /* Configuration failed :( */
                STDC_basic_assert();

                returnType = FAIL;
            }
        }
    }

    return ( returnType );
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
pass_fail_et RFM69_read_registers( RFM69_instruction_et instruction, RFM69_registers_et address, u8_t read_data[], u8_t num_bytes )
{
    /* assume pass until prove otherwise */
    pass_fail_et result = PASS;
    u8_t i = 0;

    /* Need to make sure that the SPI interface is initialised */
    if( HAL_SPI_get_init_status() == FALSE )
    {
        /* it is not initialised so do that now */
    	HAL_SPI_init();
    }

    /* Instruction and address looks good so carry out the read */
    if( ( instruction != READ_FROM_CHIP ) && ( instruction != READ_FROM_CHIP_BURST_MODE ) )
    {
        result = FAIL;
    }
    else if( address > REG_MAX )
    {
        result = FAIL;
    }
    else
    {
        /* OK to continue */

        /* pull NCS / Slave Select line low */
        HAL_BRD_RFM69_spi_slave_select( LOW );

        /* Send the address */
        HAL_SPI_write_and_read_data( address );

        if( instruction == READ_FROM_CHIP )
        {
            /* Only 1 byte allowed to be Read for this instruction */
            num_bytes = 1;
        }

        /* Writes data to chip */
        for( i = 0; i < num_bytes; i++ )
        {
            /* send dummy byte to force readback */
            read_data[ i ] = (u8_t)HAL_SPI_write_and_read_data( 0xFF );
        }

        /* pull NCS / Slave Select line high */
        HAL_BRD_RFM69_spi_slave_select( HIGH );
    }

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
pass_fail_et RFM69_write_registers( RFM69_instruction_et instruction, RFM69_registers_et address, u8_t write_data[], u8_t num_bytes )
{
    /* assume pass until prove otherwise */
    pass_fail_et result = PASS;
    u8_t i;
    u8_t modified_address;

    /* validate input parameters */
    if( ( instruction != WRITE_TO_CHIP ) && ( instruction != WRITE_TO_CHIP_BURST_MODE ) )
    {
        result = FAIL;
        STDC_assert( 1 );
    }
    /* Write only registers start at 0, so only need to validate upper limit */
    else if( address >= REG_MAX )
    {
        result = FAIL;
        STDC_assert( 1 );
    }
    else
    {
        /* OK to continue */

        /* pull NCS / Slave Select line low */
        HAL_BRD_RFM69_spi_slave_select( LOW );

        /* Now we need to modify the address with the "write" bit set */
        modified_address = ( address | RFM69_WRITE_ACCESS );

        /* Send register address */
        HAL_SPI_write_and_read_data( modified_address );

        if( instruction == WRITE_TO_CHIP )
        {
            /* Only 1 byte allowed to be Read / Written for this instruction*/
            num_bytes = 1;
        }

        /* Write all data to chip */
        for( i = 0; i < num_bytes; i++ )
        {
            HAL_SPI_write_and_read_data( write_data[ i ] );
            //if( instruction == WRITE_TO_CHIP_BURST_MODE )
			//{
            	//i++;
			//}
        }

        /* release NCS / Slave Select line high */
        HAL_BRD_RFM69_spi_slave_select( HIGH );

#ifdef READ_BACK_REGISTERS
        /* Now read back the register to ensure it was written succesfully,
        NOTE!! We can only read back the registers that we can write data to */
        u8_t read_back_register[num_bytes];

        RFM69_read_registers( READ_FROM_CHIP_BURST_MODE, address, read_back_register, num_bytes );

        /* compare actual values with expected values */
        if( STDC_memcompare( read_back_register, write_data, num_bytes ) == FALSE )
        {
            result = FAIL;
            STDC_basic_assert();
        }
#endif
    }

    return( result );
}


/*!
*******************************************************************************
*
*   \brief          Reads the version number of the RFM69 chip
*
*   \author         MS
*
*   \return         result - pass or fail
*
*******************************************************************************
*/
u8_t RFM69_read_version_num( void )
{
 	u8_t register_val;

 	RFM69_read_registers( READ_FROM_CHIP, REGVERSION, &register_val, 1 );

    return( register_val );
}


/*!
*******************************************************************************
*
*   \brief          Reads the current RF frequency of the chip
*
*   \author         MS
*
*   \return         result - pass or fail
*
*******************************************************************************
*/
u32_t RFM69_read_rf_carrier_freq( void )
{
 	u32_t carrier_freq = 0u;
 	u8_t register_val;

 	RFM69_read_registers( READ_FROM_CHIP, REGFRFMSB, &register_val, 1 );
    carrier_freq |= ( register_val << 16 );

 	RFM69_read_registers( READ_FROM_CHIP, REGFRFMID, &register_val, 1 );
 	carrier_freq |= ( register_val << 8 );

 	RFM69_read_registers( READ_FROM_CHIP, REGFRFLSB, &register_val, 1 );
 	carrier_freq |= ( register_val );

 	/* Multiply by the freq step factor to get the carrier freq */
 	carrier_freq = ( carrier_freq * FREQ_STEP );

    return( register_val );
}


/*!
*******************************************************************************
*
*   \brief          Reads the temperature the RFM69 chip
*
*   \author         MS
*
*   \return         result - pass or fail
*
*******************************************************************************
*/
u8_t RFM69_read_temp( void )
{
 	u8_t register_val;

 	/* Read back the current register status */
	RFM69_read_registers( READ_FROM_CHIP, REGOPMODE, &register_val, 1 );

	if( register_val != RFM69_STANDBY_MODE )
    {
        /* NEED to be in standby mode before this can be done */
        STDC_basic_assert();
    }
    else
    {
        register_val = 0u;

        /* start the temperature measurement */
        register_val |= ( 1 << TEMP_MEASURE_START );

        /* delay at least 100us before reading the register */
        //delay_ms(1);

        /* Read the temperature measurement */
        RFM69_read_registers( READ_FROM_CHIP, REGTEMP2, &register_val, 1 );
    }

    return( register_val );
}






/*!
*******************************************************************************
*
*   \brief          Reads the RX RSSI
*
*   \author         MS
*
*   \return         result - pass or fail
*
*******************************************************************************
*/
u8_t RFM69_read_RSSI( void )
{
 	u8_t register_val;

 	/* Read back the current register status */
	RFM69_read_registers( READ_FROM_CHIP, REGOPMODE, &register_val, 1 );

	if( register_val != RFM69_STANDBY_MODE )
    {
        /* NEED to be in standby mode before this can be done */
        STDC_basic_assert();
    }
    else
    {
        register_val = 0u;

        /* start the temperature measurement */
        register_val |= ( 1 << TEMP_MEASURE_START );

        /* delay at least 100us before reading the register */
        delay_ms(1);

        /* Read the temperature measurement */
        RFM69_read_registers( READ_FROM_CHIP, REGTEMP2, &register_val, 1 );
    }

    return( register_val );
}



u8_t RFM69_read_RSSi_measurement( void )
{
    u8_t register_val = 0u;
    false_true_et status = FALSE;

    /* Trigger the RSSI measurement */
    register_val |= ( 1 << RSSI_START );

    RFM69_read_registers( READ_FROM_CHIP, REGRSSIVALUE, &register_val, 1 );
    status = TRUE;

    return ( status );
}



false_true_et RFM69_read_FIFO_register( u8_t* data_p )
{
    false_true_et status = FALSE;

    RFM69_read_registers( READ_FROM_CHIP_BURST_MODE, REGFIFO, data_p, RFM69_MAX_DATA_LEN );
    status = TRUE;

    return ( status );
}




/*!
*******************************************************************************
*
*   \brief          Reads the Reserved Registers of the RFM69 chip
*
*   \author         MS
*
*   \return         result - pass or fail
*
*******************************************************************************
*/
pass_fail_et RFM69_read_reserved_registers( void )
{
	pass_fail_et result = PASS;
 	u8_t reserved_vals[4] = { 0x40, 0xB0, 0x7B, 0x9B };
 	u8_t readback_vals[ sizeof( reserved_vals ) ];

 	RFM69_read_registers( READ_FROM_CHIP_BURST_MODE, RESERVED14, readback_vals, sizeof( readback_vals ) );

	/* compare actual values with expected values */
	if( STDC_memcompare( reserved_vals, readback_vals, sizeof( reserved_vals ) ) == FALSE )
	{
		result = FAIL;
		STDC_basic_assert();
	}

    return( result );
}


/*!
*******************************************************************************
*
*   \brief          Turns on or off the CLK_OUT feature on DI05
*
*   \author         MS
*
*   \return         result - pass or fail
*
*******************************************************************************
*/
false_true_et RFM69_set_clock_out( disable_enable_et state )
{
 	u8_t register_val;

 	/* Read back the current register status, modify it and then rewrite it back down */
	RFM69_read_registers( READ_FROM_CHIP, REGDIOMAPPING2, &register_val, 1 );

    /* This masks off and enables the CLK out */
    register_val &= ~BIT_MASK_3_BIT;

    /* This disables the CLK_OUT */
 	if( state == DISABLE )
    {
        register_val |= CLK_OUT_OFF;
    }

    RFM69_write_registers( WRITE_TO_CHIP, REGDIOMAPPING2, &register_val, 1 );

    return( PASS );
}



/*!
*******************************************************************************
*
*   \brief          Sets the Transmitter power level
*
*   \author         MS
*
*   \return         result - pass or fail
*
*******************************************************************************
*/
false_true_et RFM69_set_PA_level( RFM69_PA_level_et level )
{
 	u8_t register_val;

 	/* Read back the current register status, modify it and then rewrite it back down */
	RFM69_read_registers( READ_FROM_CHIP, REGPALEVEL, &register_val, 1 );

#ifdef RFM69W
    register_val |= 0x80;
#endif

    /* Clear the current power level setting */
    register_val = ( register_val & 0x00 );

	switch ( level )
	{
        case RFM69_MIN_POWER:
            break;

        case RFM69_MED0_POWER:
            register_val |= 0x0F;
            break;

        case RFM69_MED1_POWER:
            register_val |= 0x10;
            break;

        case RFM69_MAX_POWER:
            register_val |= 0x1f;
            break;

        default:
            break;
	}

    RFM69_write_registers( WRITE_TO_CHIP, REGPALEVEL, &register_val, 1 );

    return( PASS );
}




/*!
*******************************************************************************
*
*   \brief          Sets the operational mode of the RFM69
*
*   \author         MS
*
*   \return         result - pass or fail
*
*******************************************************************************
*/
false_true_et RFM69_set_operating_mode( RFM69_operating_modes_et operating_mode )
{
 	u8_t register_val;
 	false_true_et listen_mode_enabled;

 	/* Read back the current register status, modify it and then rewrite it back down */
	RFM69_read_registers( READ_FROM_CHIP, REGOPMODE, &register_val, 1 );

	listen_mode_enabled = ( false_true_et )( ( register_val & ( 1 << LISTENON ) ) >> LISTENON );

	/* Clear the current power level setting */
	register_val &~( SLEEP | STDBY | FS | TX | RX );

	switch ( operating_mode )
	{
        case RFM69_SLEEP_MODE:
            register_val &= ~( BIT_MASK_1_BIT << LISTENON );

            /* Turn off CLK out to save power */
            RFM69_set_clock_out( OFF );
            break;

        case RFM69_STANDBY_MODE:
            register_val |= ( BIT_MASK_1_BIT << STDBY );
            break;

        case RFM69_FS_MODE:
            register_val |= ( BIT_MASK_1_BIT << FS );
            register_val &= ~( BIT_MASK_1_BIT << LISTENON );
            break;

        case RFM69_TRANSMIT_MODE:
            register_val |= ( BIT_MASK_1_BIT << TX );
            register_val &= ~( BIT_MASK_1_BIT << LISTENON );
            break;

        case RFM69_RECEIVE_MODE:
            register_val |= ( BIT_MASK_1_BIT << RX );
            register_val &= ~( BIT_MASK_1_BIT << LISTENON );
            break;

        case RFM69_LISTEN_MODE:
            register_val |= ( BIT_MASK_1_BIT << STDBY );
            register_val |= ( BIT_MASK_1_BIT << LISTENON );
            break;

        default:
            break;
	}

	if( ( listen_mode_enabled == TRUE ) && ( operating_mode != RFM69_LISTEN_MODE ) )
    {
        register_val |= ( BIT_MASK_1_BIT << LISTEN_ABORT );
        RFM69_write_registers( WRITE_TO_CHIP, REGOPMODE, &register_val, 1 );
        register_val &= ~( BIT_MASK_1_BIT << LISTEN_ABORT );
    }

    RFM69_write_registers( WRITE_TO_CHIP, REGOPMODE, &register_val, 1 );

    return( PASS );
}


/*!
*******************************************************************************
*
*   \brief          Sets the operational mode of the RFM69
*
*   \author         MS
*
*   \return         result - pass or fail
*
*******************************************************************************
*/
false_true_et RFM69_set_packet_mode( RFM69_packet_modes_et operating_mode )
{
 	u8_t register_val;

 	/* Read back the current register status, modify it and then rewrite it back down */
	RFM69_read_registers( READ_FROM_CHIP, REGPACKETCONFIG1, &register_val, 1 );

	/* Clear the current setting */
	register_val &~( BIT_MASK_1_BIT << PACKET_FORMAT );
	register_val |= ( operating_mode << PACKET_FORMAT );

    RFM69_write_registers( WRITE_TO_CHIP, REGPACKETCONFIG1, &register_val, 1 );

    return( PASS );
}



/*!
*******************************************************************************
*
*   \brief          Sets the payload length of the RFM69 packet
*
*   \author         MS
*
*   \return         result - pass or fail
*
*******************************************************************************
*/
false_true_et RFM69_set_payload_length( u8_t num_bytes )
{
    u8_t register_val;

	/* Clear the current setting */
	register_val = num_bytes;

    RFM69_write_registers( WRITE_TO_CHIP, REGPAYLOADLENGTH, &register_val, 1 );

    return( PASS );
}



/*!
*******************************************************************************
*
*   \brief          Sets the modulation type of the RF data
*
*   \author         MS
*
*   \return         result - pass or fail
*
*******************************************************************************
*/
false_true_et RFM69_set_modulation_type( RFM69_modulation_type_et type )
{
    u8_t register_val;
    false_true_et status;

    if( type >= RFM69_RESERVED)
    {
        status = FAIL;
    }
    else
    {
        /* Read back the current register status, modify it and then rewrite it back down */
        RFM69_read_registers( READ_FROM_CHIP, REGDATAMODUL, &register_val, 1 );

        register_val &~( BIT_MASK_2_BIT << MODULATION_TYPE );
        register_val |= ( type << MODULATION_TYPE );

        RFM69_write_registers( WRITE_TO_CHIP, REGDATAMODUL, &register_val, 1 );

        status = PASS;
    }

    return( status );
}



false_true_et RFM69_set_data_mode( RFM69_data_mode_et mode )
{
    u8_t register_val;
    false_true_et status;

    if( mode >= RFM69_MODE_MAX)
    {
        status = FAIL;
    }
    else
    {
        /* Read back the current register status, modify it and then rewrite it back down */
        RFM69_read_registers( READ_FROM_CHIP, REGDATAMODUL, &register_val, 1 );

        register_val &~( BIT_MASK_2_BIT << DATA_PROCESS_MODE );
        register_val |= ( mode << DATA_PROCESS_MODE );

        RFM69_write_registers( WRITE_TO_CHIP, REGDATAMODUL, &register_val, 1 );

        status = PASS;
    }

    return( status );
}




false_true_et RFM69_set_bit_rate( RFM69_predefined_bitrates_et bit_rate )
{
    false_true_et status = TRUE;
    u8_t register_val;
    RFM69_modulation_type_et modulation_type;
    u16_t data_rate_setting;

     /* Read back the current register status ( modulation Scheme )*/
    RFM69_read_registers( READ_FROM_CHIP, REGDATAMODUL, &modulation_type, 1 );
    modulation_type = ( ( modulation_type & ( BIT_MASK_2_BIT << MODULATION_TYPE ) ) >> MODULATION_TYPE );

    /* OOK only supports up to 19.2Kbps */
    if( modulation_type == RFM69_OOK  )
    {
        if( bit_rate > RFM69_19200bps )
        {
            bit_rate = RFM69_19200bps;
        }
    }

    switch( bit_rate )
    {
        /*! These are predeined standard values for bitrates taken from datasheet */
        case RFM69_1200bps:   data_rate_setting = 0x682B; break;
        case RFM69_2400bps:   data_rate_setting = 0x3415; break;
        case RFM69_4800bps:   data_rate_setting = 0x1A0B; break;
        case RFM69_9600bps:   data_rate_setting = 0x0D05; break;
        case RFM69_19200bps:  data_rate_setting = 0x0683; break;
        case RFM69_38400bps:  data_rate_setting = 0x0341; break;
        case RFM69_76800bps:  data_rate_setting = 0x01A1; break;
        case RFM69_153600bps: data_rate_setting = 0x00D0; break;
        default: break;
    }

    //! Write the MSB
    register_val = ( ( data_rate_setting & 0xFF00 ) >> 8 );
    RFM69_write_registers( WRITE_TO_CHIP, REGBITRATEMSB, &register_val, 1 );

    //! Write the LSB
    register_val = ( data_rate_setting & 0x00FF );
    RFM69_write_registers( WRITE_TO_CHIP, REGBITRATELSB, &register_val, 1 );

    return ( status );
}




false_true_et RFM69_set_listen_time_resolution( RFM69_listen_state_et state, RFM69_listen_time_et listen_time_resolution )
{
    u8_t register_val;
    false_true_et status = FALSE;

    //! Now check that the value is correct
    if( listen_time_resolution <= RFM_69_26200us )
    {
        register_val = listen_time_resolution;

        /* Read back the current register status, modify it and then rewrite it back down */
        RFM69_read_registers( READ_FROM_CHIP, REGLISTEN1, &register_val, 1 );

        //! Check that the state is correct
        if( state == RFM69_LISTEN_RX )
        {
             register_val &~( BIT_MASK_2_BIT << LISTEN_RX_RES );
             register_val |= ( listen_time_resolution << LISTEN_RX_RES );
             RFM69_write_registers( WRITE_TO_CHIP, REGLISTEN1, &register_val, 1 );

             status = TRUE;
        }
        else if ( state == RFM69_LISTEN_IDLE )
        {
             register_val & ~( BIT_MASK_2_BIT << LISTEN_IDLE_RES );
             register_val |= ( listen_time_resolution << LISTEN_IDLE_RES );
             RFM69_write_registers( WRITE_TO_CHIP, REGLISTEN1, &register_val, 1 );

             status = TRUE;
        }
        else
        {
        }
    }

    return ( status );
}



false_true_et RFM69_set_listen_time( RFM69_listen_state_et state, u16_t time )
{
    u8_t register_val;
    false_true_et status = FALSE;

    register_val = time;

    //! Check that the state is correct
    if( state == RFM69_LISTEN_RX )
    {
         RFM69_write_registers( WRITE_TO_CHIP, REGLISTEN2, &register_val, 1 );

         status = TRUE;
    }
    else if ( state == RFM69_LISTEN_IDLE )
    {
         RFM69_write_registers( WRITE_TO_CHIP, REGLISTEN3, &register_val, 1 );

         status = TRUE;
    }
    else
    {
    }

    return ( status );
}





false_true_et RFM69_set_tx_power_level( u8_t level )
{
    /* When using the NON HIGH power version of the module the power can be set from -18db to +17db ;),
    and only PA0 needs to be used for this...
    Add support later for the HP versions....  */

    u8_t register_val;
    false_true_et status = FALSE;

    if( level > MAX_TX_POWER_LEVEL )
    {
        level = MAX_TX_POWER_LEVEL;
    }

    register_val = level;

    /* There is an offset of -18db applied here */
    register_val |= ( 1 << PA0_ON );

    RFM69_write_registers( WRITE_TO_CHIP, REGPALEVEL, &register_val, 1 );
    status = TRUE;

    return ( status );
}





false_true_et RFM69_set_encryption_key( u8_t* key, false_true_et state )
{
    u8_t register_val;
    false_true_et status = FALSE;

    /* Write down the encryption key */
    RFM69_write_registers( WRITE_TO_CHIP_BURST_MODE, REGAESKEY1, key, ENCRYPT_KEY_SIZE  );

    /* Read back the current register status, modify it and then rewrite it back down */
    RFM69_read_registers( READ_FROM_CHIP, REGPACKETCONFIG2, &register_val, 1 );

    /* Mask off the correct bits and turn encryption on or off */
    register_val &~( BIT_MASK_1_BIT << AES_ON ) ;
    register_val |= ( state << AES_ON ) ;
    RFM69_write_registers( WRITE_TO_CHIP, REGPACKETCONFIG2, &register_val, 1 );

    status = TRUE;

    return ( status );
}




false_true_et RFM69_set_DIO_mapping( u8_t pin, RFM69_DIO_map_mode_et mode )
{
    u8_t register_val;
    false_true_et status = FALSE;

    //!pins 0 - 3 are on one register and pins 4 - 5 on another
    if( pin <= 3 )
    {
        /* Read back the current register status, modify it and then rewrite it back down */
        RFM69_read_registers( READ_FROM_CHIP, REGDIOMAPPING1, &register_val, 1 );

        switch( pin )
        {
            case 0:
                register_val &= ~( BIT_MASK_2_BIT << DIO0 );
                register_val |=  ( mode << DIO0 );
                break;
            case 1:
                register_val &= ~( BIT_MASK_2_BIT << DIO1 );
                register_val |=  ( mode << DIO1 );
                break;
            case 2:
                register_val &= ~( BIT_MASK_2_BIT << DIO2 );
                register_val |=  ( mode << DIO2 );
                break;
            case 3:
                register_val &= ~( BIT_MASK_2_BIT << DIO3 );
                register_val |=  ( mode << DIO3 );
                break;

                default:
                    break;
        }

        RFM69_write_registers( WRITE_TO_CHIP, REGDIOMAPPING1, &register_val, 1 );
    }
    else
    {
        /* Read back the current register status, modify it and then rewrite it back down */
        RFM69_read_registers( READ_FROM_CHIP, REGDIOMAPPING2, &register_val, 1 );

        switch( pin )
        {
            case 4:
                register_val &= ~( BIT_MASK_2_BIT << DIO4 );
                register_val |=  ( mode << DIO4 );
                break;
            case 5:
                register_val &= ~( BIT_MASK_2_BIT << DIO5 );
                register_val |=  ( mode << DIO5 );
                break;

            default:
                break;
        }

        RFM69_write_registers( WRITE_TO_CHIP, REGDIOMAPPING2, &register_val, 1 );
    }

    return ( status );
}




false_true_et RFM69_trigger_RSSi_measurement( void )
{
    u8_t register_val = 0u;
    false_true_et status = FALSE;

    /* Trigger the RSSI measurement */
    register_val |= ( 1 << RSSI_START );

    RFM69_write_registers( WRITE_TO_CHIP, REGRSSICONFIG, &register_val, 1 );
    status = TRUE;

    return ( status );
}



false_true_et RFM69_write_to_FIFO( u8_t* buffer, u8_t len )
{
    false_true_et status = FALSE;

    RFM69_write_registers( WRITE_TO_CHIP_BURST_MODE, REGFIFO, buffer, len );

    return ( status );
}



false_true_et RFM69_Send_frame( u8_t* buffer, u8_t len )
{
    false_true_et status = FALSE;

    u8_t tx_buffer[66];

    tx_buffer[0] = len;

    STDC_memcpy( &tx_buffer[2], buffer, len );

    /* I think the first thing we need to do is to set the length register?? */
    //RFM69_set_payload_length(len);

    RFM69_write_to_FIFO( tx_buffer, 66 );

    return ( status );
}






/*!
****************************************************************************************************
*
*   \brief         RF Module reset
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
void RFM69_reset( void )
{
	RFM69_set_reset_pin_state( HIGH );

	delay_us(1000);

	RFM69_set_reset_pin_state( LOW );

	delay_us(1000);
}




void RFM69_set_reset_pin_state( low_high_et state )
{
    HAL_BRD_RFM69_set_reset_Pin_state( state );
}


void RFM69_set_enable_pin_state( low_high_et state )
{
    HAL_BRD_RFM69_set_enable_Pin_state( state );
}
