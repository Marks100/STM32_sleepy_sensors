/*! \file
*               Author: mstewart
*   \brief      SYSTICK_MGR module
*/
/***************************************************************************************************
**                              Includes                                                          **
***************************************************************************************************/
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stm32f10x.h"

#include "STDC.h"
#include "C_defs.h"
#include "NVM.h"
#include "HAL_BRD.h"
#include "HAL_UART.h"
#include "CLI_MGR.h"
#include "SEN_MGR.h"
#include "RTC.h"
#include "VER.h"

extern NVM_info_st        NVM_info_s;

/* Debug display strings */
const char CLI_MGR_welcome_message_s[] =
		"\r\n\n\rHello...\r\nCLI Console is now open, Type 'help' for list of commands";

const char CLI_MGR_closing_serial_port_s[] =
		"\r\nClosing serial port due to inactivity to save power, please press the \"Enter\" key to re-open the debug prompt";

const char CLI_MGR_newline[] =
		"\r\n";

const char* CLI_MGR_err_string[ CLI_MGR_ERROR_MAX ] =
{
	"\r",
	"\r",
	"\r\nOperation failed\r\n",
	"\r\nInvalid number of arguments\r\n",
	"\r\nInvalid arguments\r\n",
	"\r\nCommand not supported\r\n",
	"\r\nCommand prohibited in this mode\r\n",
	"\r\nCommand not found\r\n",
	"\r\nOut of range\r\n",
	"\r\nInvalid Hex param\r\n",
	"\r\nInvalid Dec param\r\n",
	"\r\nInvalid hex param length\r\n",
	"\r\nInvalid Dec param length\r\n",
	"\r\nValue is already set\r\n",
	"\r\nInvalid string input\r\n"
};

STATIC u8_t               CLI_MGR_byte_index_s;
STATIC u8_t               CLI_MGR_rx_command_len;
STATIC false_true_et      CLI_MGR_cr_received_s;
STATIC u8_t               CLI_MGR_msg_read_s[CLI_MGR_MAX_INPUT_CHARS + 1];
STATIC CLI_MGR_cmd_history_st CLI_MGR_cmd_history_s;


STATIC const CLI_MGR_Command_st CLI_MGR_commands[] =
{
	{"help",	  &help_handler,	        HELP_HELP, 	      SUPPORTED_FOR_ALL_MODES, ENABLE, 0, NULL_PARAM_LIST },
	{"ver",		  &ver_handler,		        HELP_VER , 	      SUPPORTED_FOR_ALL_MODES, ENABLE, 0, NULL_PARAM_LIST },
	{"setmode",	  &setmode_handler,	        HELP_SET_MODE,    SUPPORTED_FOR_ALL_MODES, ENABLE, 0, NULL_PARAM_LIST },
	{"reset",	  &reset_handler,	        HELP_RESET,       SUPPORTED_FOR_ALL_MODES, ENABLE, 0, NULL_PARAM_LIST },
	{"nvm",		  &nvm_handler,		        HELP_NVM,	      SUPPORTED_FOR_ALL_MODES, ENABLE, 0, NULL_PARAM_LIST },
	{"clocks",	  &clocks_handler,	        HELP_CLOCKS,      SUPPORTED_FOR_ALL_MODES, ENABLE, 0, NULL_PARAM_LIST },
	{"temp",	  &temp_handler,	        HELP_TEMP,	      SUPPORTED_FOR_ALL_MODES, ENABLE, 0, NULL_PARAM_LIST },
	{"batt",	  &batt_handler,            HELP_BATT,	      SUPPORTED_FOR_ALL_MODES, ENABLE, 0, NULL_PARAM_LIST },
	{"test",      &test_handler,	        HELP_TEST,	      SUPPORTED_FOR_ALL_MODES, ENABLE, 0, NULL_PARAM_LIST },
	{"savenvm",   &savenvm_handler,	        HELP_SAVEMEVM,    SUPPORTED_FOR_ALL_MODES, ENABLE, 0, NULL_PARAM_LIST },
	{"setid",     &set_id_handler,          HELP_SET_ID,      SUPPORTED_FOR_ALL_MODES, ENABLE, 1, SET_ID_CMD_PARAM_LIST },
	{"rtcclr",    &rtc_clear_handler,       HELP_RTC_CLR,     SUPPORTED_FOR_ALL_MODES, ENABLE, 0, NULL_PARAM_LIST },
	{"setwkp",    &set_wakeup_time_handler, HELP_WAKEUP_TIME, SUPPORTED_FOR_ALL_MODES, ENABLE, 0, SET_WAKEUP_TIME_PARAM_LIST },
	{ NULL,		  NULL,					    NULL,             SUPPORTED_FOR_ALL_MODES, ENABLE, 0, NULL_PARAM_LIST }
};


void CLI_MGR_init( void )
{
	u8_t i = 0u;

	CLI_MGR_byte_index_s = 0u;
	CLI_MGR_rx_command_len = 0u;
	CLI_MGR_cr_received_s = 0u;

	STDC_memset( CLI_MGR_msg_read_s, 0x20, sizeof( CLI_MGR_msg_read_s ) );

	/* Reload the command history :) */
	for( i = 0u; i < CLI_MGR_MAX_COMMAND_HISTORY; i++ )
	{
		STDC_memcpy( &CLI_MGR_cmd_history_s.cmd_list[i], &NVM_info_s.NVM_generic_data_blk_s.cmd_list[i], sizeof( CLI_MGR_cmd_history_s.cmd_list[i] ) );
	}
}


void CLI_MGR_display_startup_message( void )
{
	/* Print a welcome message */
	CLI_MGR_print_welcome_message();

	CLI_MGR_print_prompt( TRUE );
}


void CLI_MGR_message_handler( void )
{
	if( CLI_MGR_carriage_return_check() == TRUE )
	{
		/* Reset the carriage return flag */
		CLI_MGR_cr_received_s = FALSE;

		/* handle the actual received command */
		CLI_MGR_handle_serial_command();

		/* Clear out the buffers */
		CLI_MGR_clear_rx_buffer();

		CLI_MGR_print_prompt( TRUE );
	}
}


void CLI_MGR_handle_serial_command ( void )
{
	CLI_MGR_error_et error = CLI_MGR_ERROR_NONE;
	char *argVector[ CLI_MGR_CMD_LINE_ARGS_MAX ];
	u8_t argument_count = 0u;

	char  output_string[100];
	STDC_memset( output_string, 0, sizeof( output_string ) );

	error = CLI_MGR_parse_cmd( (char*)CLI_MGR_msg_read_s, &argument_count, argVector, CLI_MGR_CMD_LINE_ARGS_MAX );

	if( error == CLI_MGR_ERROR_NONE )
	{
		error = CLI_MGR_process_cmd( argument_count, argVector );
	}

	strcpy( output_string, CLI_MGR_err_string[error] );

	CLI_MGR_send_data( output_string, strlen(output_string) );
}

/*!
****************************************************************************************************
*
*   \brief         Check each charachter received
*
*   \author        MS
*
*   \return        void
*
***************************************************************************************************/

void CLI_MGR_handle_received_char( u8_t received_char )
{
	u8_t useable_commands = 0u;
	false_true_et command_present = FALSE;
	u8_t i = 0;

	if( ( ( received_char >= CLI_MGR_SPACE ) && ( received_char <= CLI_MGR_DEL ) ) || ( received_char <= CLI_MGR_CR ) )
	{
		/* count the number of non 0 commands */
		for( i = 0u; i < CLI_MGR_MAX_COMMAND_HISTORY; i++ )
		{
			if( CLI_MGR_cmd_history_s.cmd_list[i].cmd[0] != 0x00 )
			{
				useable_commands += 1u;
			}
		}

		if ( received_char == '\r')
		{
			CLI_MGR_rx_command_len = CLI_MGR_byte_index_s;
			CLI_MGR_byte_index_s = 0u;

			/* We have received a Carriage Return, so lets set this flag to let the system know a
			command has been received */
			CLI_MGR_cr_received_s = TRUE;
			CLI_MGR_send_data( "\n", sizeof(received_char) );

			if( CLI_MGR_rx_command_len != 0u )
			{
				/* Dont add the new command if its already on the current list */
				for( i = 0u; i < useable_commands; i++ )
				{
					if( STDC_memcompare( CLI_MGR_cmd_history_s.cmd_list[i].cmd, CLI_MGR_msg_read_s, CLI_MGR_rx_command_len ) )
					{
						command_present = TRUE;
						break;
					}
				}

				if( command_present == FALSE )
				{
					if( useable_commands != CLI_MGR_MAX_COMMAND_HISTORY)
					{
						 /* Setup the index for the first command entered by the user, as the history is not full */
						 CLI_MGR_cmd_history_s.index_to_next_cmd = ( useable_commands  % CLI_MGR_MAX_COMMAND_HISTORY );
					}

					STDC_memset( &CLI_MGR_cmd_history_s.cmd_list[CLI_MGR_cmd_history_s.index_to_next_cmd].cmd, 0x00, sizeof( CLI_MGR_cmd_history_s.cmd_list[CLI_MGR_cmd_history_s.index_to_next_cmd].cmd ) );

					STDC_memcpy( &CLI_MGR_cmd_history_s.cmd_list[CLI_MGR_cmd_history_s.index_to_next_cmd].cmd, CLI_MGR_msg_read_s, CLI_MGR_rx_command_len );

					CLI_MGR_cmd_history_s.index_to_next_cmd = ( CLI_MGR_cmd_history_s.index_to_next_cmd + 1u )%CLI_MGR_MAX_COMMAND_HISTORY;
				}
			}

			CLI_MGR_store_history();
		}
		else if( received_char == CLI_MGR_DEL )
		{
			CLI_MGR_byte_index_s = 0u;
			STDC_memset( CLI_MGR_msg_read_s, 0x00, sizeof( CLI_MGR_msg_read_s ) );
			CLI_MGR_msg_read_s[ CLI_MGR_byte_index_s ] = 0u;
			CLI_MGR_send_data( (char*)ERASE_CURRENT_LINE, strlen(ERASE_CURRENT_LINE));
			CLI_MGR_print_prompt( FALSE );
		}
		else if ( received_char == CLI_MGR_BACKSPACE )
		{
			if( CLI_MGR_byte_index_s > 0u )
			{
				CLI_MGR_byte_index_s--;
				CLI_MGR_msg_read_s[ CLI_MGR_byte_index_s ] = 0x00;
				CLI_MGR_send_data( "\b ", strlen("\b ") );
				CLI_MGR_send_data( (char*)&received_char, 1u );
			}

			return;
		}
		else if( ( received_char == UP_ARROW || received_char == DOWN_ARROW ) && ( CLI_MGR_msg_read_s[ CLI_MGR_byte_index_s - 1u ] == '[' ) )
		{
			//received 'A' and may be we had received a '[' previously which means a up arrow has been hit.
			//load the previous command and print it and wait for the carriage return
			CLI_MGR_msg_read_s[ CLI_MGR_byte_index_s++ ] = received_char;

			/* If there are no useable commands then dont allow the user to scroll up or down */
			if( useable_commands != 0u )
			{
				if( received_char == UP_ARROW )
				{
					if( CLI_MGR_cmd_history_s.index_to_next_cmd == 0u )
					{
						CLI_MGR_cmd_history_s.index_to_next_cmd = ( useable_commands - 1u );
					}
					else
					{
						CLI_MGR_cmd_history_s.index_to_next_cmd -= 1u;
					}
				}
				else
				{
					CLI_MGR_cmd_history_s.index_to_next_cmd = ( ( CLI_MGR_cmd_history_s.index_to_next_cmd + 1u ) % useable_commands );
				}

				CLI_MGR_store_history();

				STDC_memset( CLI_MGR_msg_read_s, 0x00, sizeof( CLI_MGR_msg_read_s ) );
				strcpy( (char*)CLI_MGR_msg_read_s, (char*)CLI_MGR_cmd_history_s.cmd_list[ CLI_MGR_cmd_history_s.index_to_next_cmd ].cmd);

				CLI_MGR_byte_index_s = strlen( (char*)CLI_MGR_cmd_history_s.cmd_list[ CLI_MGR_cmd_history_s.index_to_next_cmd ].cmd );

				CLI_MGR_send_data( (char*)ERASE_CURRENT_LINE, strlen(ERASE_CURRENT_LINE) );

				CLI_MGR_print_prompt( FALSE );

				CLI_MGR_send_data( (char*)CLI_MGR_msg_read_s, strlen( (char*)CLI_MGR_msg_read_s ) );
			}
			return;
		}
		else if( ( received_char == LEFT_ARROW || received_char == RIGHT_ARROW ) && ( CLI_MGR_msg_read_s[ CLI_MGR_byte_index_s - 1u ] == '[' ) )
		{
			/* handle the left and right arrow keys aswell, but for now just ignore them */
			return;
		}
		else if( received_char == '[' )
		{
			CLI_MGR_msg_read_s[ CLI_MGR_byte_index_s++ ] = received_char;
			return;
		}
		else
		{
			if( CLI_MGR_byte_index_s < CLI_MGR_MAX_INPUT_CHARS )
			{
				/* Record every byte that gets received */
				CLI_MGR_msg_read_s[ CLI_MGR_byte_index_s++ ] = received_char;
				CLI_MGR_send_data( (char*)&received_char, sizeof(received_char) );
			}
		}
	}
}

void CLI_MGR_clear_rx_buffer( void )
{
	 STDC_memset( CLI_MGR_msg_read_s, 0x00, sizeof( CLI_MGR_msg_read_s ) );
	 CLI_MGR_byte_index_s = 0u;
	 CLI_MGR_rx_command_len = 0u;
}


false_true_et CLI_MGR_carriage_return_check( void )
{
	false_true_et return_type = FALSE;

	if( CLI_MGR_cr_received_s == TRUE )
	{
		return_type = TRUE;
	}
	return ( return_type );
}



false_true_et CLI_MGR_is_space_or_newLine( char c )
{
	false_true_et return_type = FALSE;

	if( (c == ' ') || (c == '\t') || (c == '\r') || (c == '\n') )
	{
		return_type = TRUE;
	}
	return ( return_type );
}


false_true_et CLI_MGR_is_hex_char( char c )
{
	false_true_et return_type = FALSE;

	if( (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') )
	{
		return_type = TRUE;
	}

	return ( return_type );
}

false_true_et CLI_MGR_is_dec_char( char c )
{
	false_true_et return_type = FALSE;

	if( c >= '0' && c <= '9' )
	{
		return_type = TRUE;
	}
	return ( return_type );
}


unsigned long long CLI_MGR_str_to_hex( const char* str )
{
	return (unsigned long long) strtoull(str, 0, 16);
}

unsigned long long CLI_MGR_str_to_dec( const char* str )
{
	return (unsigned long long) atoi(str);
}

void CLI_MGR_send_newline( void )
{
	CLI_MGR_send_data( (char*)CLI_MGR_newline, sizeof( CLI_MGR_newline ) );
}

void CLI_MGR_print_welcome_message( void )
{
	CLI_MGR_send_data( (char*)CLI_MGR_welcome_message_s, sizeof( CLI_MGR_welcome_message_s ) );
}

void CLI_MGR_print_prompt( false_true_et newline )
{
	char prompt_string[100u];

	STDC_memset( prompt_string, 0x20, sizeof( prompt_string ) );

	if( newline == TRUE )
	{
		CLI_MGR_send_newline();
		CLI_MGR_send_newline();
	}
	sprintf( prompt_string, "CLI PROMPT>" );

	CLI_MGR_send_data( prompt_string, strlen( prompt_string ) );
}


void CLI_MGR_send_data( char* data, u16_t data_size )
{
	HAL_USART2_send_data( data, data_size );
}


CLI_MGR_error_et CLI_MGR_parse_cmd( char* message_string, u8_t* argumen_count, char *argument_vector[], u8_t max_num_args )
{
	CLI_MGR_error_et error = CLI_MGR_ERROR_NONE;
	char* cmd;
	u8_t calc_argumen_count = 0u;
	u8_t i = 0u;

	cmd = message_string;

	// Search for the first charachter */
	for( i = 0u; i < strlen( message_string ) - 1; i++ )
	{
		if( CLI_MGR_is_space_or_newLine( *(cmd+i) ) != TRUE )
		{
			break;
		}
	}

	/* We have pointed the command to the start of the string ignoring any leading spaces or tabs
	 * and because we have found that this charachter was not a space we can immediately move to the next charachter*/

	argument_vector[calc_argumen_count++] = cmd;
	cmd += ( i + 1u );

	for (; *cmd; cmd++)
	{
		if ( CLI_MGR_is_space_or_newLine(*cmd) == TRUE )
		{
			*cmd = '\0';
		}
		else if ( *(cmd - 1u) == '\0')
		{
			if( ( calc_argumen_count ) >= max_num_args )
			{
				error = CLI_MGR_ERROR_INVALID_ARGS_NUM;
				break;
			}
			argument_vector[calc_argumen_count++] = cmd;
		}
	}

	*argumen_count = calc_argumen_count;

	return error;
}


CLI_MGR_error_et CLI_MGR_validate_arguments( u8_t aArgCount, char *aArgVector[], u8_t command_index )
{
	CLI_MGR_error_et error = CLI_MGR_ERROR_NONE;
	u8_t i = 0u;
	u8_t len = 0u;
	u32_t min_arg_value;
	u32_t max_arg_value;

	(void)min_arg_value;
	(void)max_arg_value;

	if( ( aArgCount > CLI_MGR_CMD_LINE_ARGS_MAX ) || ( aArgCount < CLI_MGR_commands[command_index].num_params ) )
	{
		error = CLI_MGR_ERROR_INVALID_ARGS_NUM;
	}

	if( error == CLI_MGR_ERROR_NONE)
	{
		/* Valid number of args, now check the length of each args */
		for( i = 0u; i < aArgCount; i++ )
		{
			len = strlen( aArgVector[i+1] );
			if( len > CLI_MGR_commands[command_index].param_list[i].num_chars )
			{
				error = CLI_MGR_ERROR_INVALID_ARGS;
				break;
			}
		}

	}
	return ( error );
}



CLI_MGR_error_et CLI_MGR_process_cmd( u8_t aArgCount, char *aArgVector[] )
{
	CLI_MGR_error_et error = CLI_MGR_ERROR_NONE;
	const CLI_MGR_Command_st *cmd = NULL;
	u8_t command_index = 0u;

	for ( cmd = CLI_MGR_commands; cmd->command_name != NULL; cmd++ )
	{
		if ( (strcmp( aArgVector[0], cmd->command_name ) == 0u ) )
		{
			//the command can be executed. Now get into validating the parameters and their values provided by the user
			error = CLI_MGR_validate_arguments( aArgCount - 1, aArgVector, command_index );
//			}
//			else
//			{
//				error = CLI_MGR_ERROR_PROHIBITED;
//			}
			// there is a matching entry in the table. just come out of the loop!
			break;
		}
		command_index += 1u;
	}

	if( error == CLI_MGR_ERROR_NONE )
	{
		if(cmd->command_name == NULL)
		{
			// looks like we have looped through the entire table and we did not get see the command.
			error = CLI_MGR_ERROR_NOT_FOUND;
		}
	}

	if( error == CLI_MGR_ERROR_NONE  )
	{
		error = cmd->command_handler((aArgCount-1), aArgVector );
	}

	return error;
}



void CLI_MGR_store_history( void )
{
	u8_t i = 0u;

	for( i = 0u; i < CLI_MGR_MAX_COMMAND_HISTORY; i++ )
	{
		STDC_memcpy( &NVM_info_s.NVM_generic_data_blk_s.cmd_list[i], &CLI_MGR_cmd_history_s.cmd_list[i], sizeof( CLI_MGR_cmd_st ) );
	}
}


CLI_MGR_error_et help_handler( u8_t aArgCount, char *aArgVector[] )
{
	(void)aArgCount;
	(void)aArgVector;
	char  output_string[250] = { 0 };
	const CLI_MGR_Command_st *cmd = NULL;
	CLI_MGR_error_et error = CLI_MGR_ERROR_NONE;

	CLI_MGR_send_newline();

	for ( cmd = CLI_MGR_commands; cmd->command_name != NULL; cmd++)
	{
		STDC_memset( output_string, 0x20, sizeof( output_string ) );

		if( strcmp( cmd->command_name,"mode") == 0 )
		{
			sprintf(output_string, cmd->helpinfo);
			CLI_MGR_send_data( output_string, strlen(output_string));

			STDC_memset( output_string, 0, sizeof( output_string ) );
			CLI_MGR_send_newline();
		}
		else
		{
			sprintf(output_string, "\r\t%s", cmd->helpinfo);
			CLI_MGR_send_data( output_string, strlen(output_string));
		}
	}
	CLI_MGR_send_newline();

	return ( error );
}

CLI_MGR_error_et setmode_handler( u8_t aArgCount, char *aArgVector[] )
{
	(void)aArgCount;
	(void)aArgVector;
	CLI_MGR_error_et error = CLI_MGR_ERROR_NONE;

	return( error );

}

CLI_MGR_error_et reset_handler( u8_t aArgCount, char *aArgVector[] )
{
	(void)aArgCount;
	(void)aArgVector;
	CLI_MGR_error_et error = CLI_MGR_ERROR_NONE;
	char output_string[200];

	STDC_memset(output_string, 0x00, sizeof(output_string));

	sprintf( output_string, "\r\nSaving NVM contents....\r\nResetting Core..." );
	CLI_MGR_send_data( output_string, strlen(output_string));
	CLI_MGR_send_newline();

	NVM_request_flush();

	HAL_BRD_reset();

	return( error );
}


CLI_MGR_error_et ver_handler( u8_t aArgCount, char *aArgVector[] )
{
	(void)aArgCount;
	(void)aArgVector;
	CLI_MGR_error_et error = CLI_MGR_ERROR_NONE;
	char output_string[200];
	char version_num[SW_VERSION_NUM_SIZE];

	STDC_memset( output_string, 0x20, sizeof( output_string ) );

	VER_get_sw_version_num( (u8_t*)version_num );

	CLI_MGR_send_newline();
	sprintf( output_string, "SW version is %d.%d.%d", version_num[0], version_num[1], version_num[2] );
	CLI_MGR_send_data( output_string, strlen(output_string));

	//VER_get_HW_version_num( (u8_t*)version_num );

	CLI_MGR_send_newline();
	sprintf( output_string, "HW version is %d", version_num[0] );
	CLI_MGR_send_data( output_string, strlen(output_string));

	return( error );
}


CLI_MGR_error_et nvm_handler( u8_t aArgCount, char *aArgVector[] )
{
	(void)aArgCount;
	(void)aArgVector;
	CLI_MGR_error_et error = CLI_MGR_ERROR_NONE;
	char output_string[200];
	u8_t i = 0u;

	STDC_memset( output_string, 0x00, sizeof( output_string ) );

	CLI_MGR_send_newline();
	CLI_MGR_send_newline();
	sprintf( output_string, "/****** NVM data ******/\r\nchksum:\t\t0x%02X\r\nVers:\t\t%d\r\nwrites:\t\t%d",
			NVM_info_s.checksum,
			(int)NVM_info_s.version,
			(int)NVM_info_s.write_count );
	CLI_MGR_send_data( output_string, strlen(output_string));
	CLI_MGR_send_newline();
	CLI_MGR_send_newline();

	sprintf( output_string, "ID:\t\t0x%08x\r\ntype:\t\t%d\r\nwakeup_per:\t%d sec(s)\r\n",
			NVM_info_s.NVM_generic_data_blk_s.sensor_id,
			NVM_info_s.NVM_generic_data_blk_s.sensor_type,
			NVM_info_s.NVM_generic_data_blk_s.wakeup_period_sec );
	CLI_MGR_send_data( output_string, strlen(output_string));
	CLI_MGR_send_newline();
	CLI_MGR_send_newline();

	STDC_memset( output_string, 0x00, sizeof( output_string ) );
	sprintf( output_string, "/*** Stored CLI commands ***/" );
	CLI_MGR_send_data( output_string, strlen(output_string));
	CLI_MGR_send_newline();

	for( i = 0u; i < CLI_MGR_MAX_COMMAND_HISTORY; i++ )
	{
		STDC_memset( output_string, 0x00, sizeof( output_string ) );
		sprintf( output_string, "%2d: %s\r\n", i + 1, NVM_info_s.NVM_generic_data_blk_s.cmd_list[i].cmd );
		CLI_MGR_send_data( output_string, strlen(output_string));
	}

	CLI_MGR_send_newline();

	return( error );
}


CLI_MGR_error_et clocks_handler( u8_t aArgCount, char *aArgVector[] )
{
	(void)aArgCount;
	(void)aArgVector;
	CLI_MGR_error_et error = CLI_MGR_ERROR_NONE;
	char output_string[200];

	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq (&RCC_Clocks);

	sprintf( output_string, "SYSCLK:\t%dHz\r\nHCLK:\t%dHz\r\nPCLK1:\t%dHz\r\nPCLK2:\t%dHz\r\nADCCLK:\t%dHz\r\n",
			(int)RCC_Clocks.SYSCLK_Frequency, (int)RCC_Clocks.HCLK_Frequency, (int)RCC_Clocks.PCLK1_Frequency,
			(int)RCC_Clocks.PCLK2_Frequency, (int)RCC_Clocks.ADCCLK_Frequency );

	CLI_MGR_send_newline();
	CLI_MGR_send_data( output_string, strlen(output_string));

	return( error );
}


CLI_MGR_error_et temp_handler( u8_t aArgCount, char *aArgVector[] )
{
	(void)aArgCount;
	(void)aArgVector;
	CLI_MGR_error_et error = CLI_MGR_ERROR_NONE;
	char output_string[200u];
	s32_t dec = (s16_t)SEN_MGR_get_temperature();
	u32_t frac = ( abs(SEN_MGR_get_temperature() * 10u) - abs(dec*10u) );

	sprintf( output_string, "%d.%1d'c", dec, frac );

	CLI_MGR_send_newline();
	CLI_MGR_send_data( output_string, strlen(output_string));

	return( error );
}

CLI_MGR_error_et batt_handler( u8_t aArgCount, char *aArgVector[] )
{
	(void)aArgCount;
	(void)aArgVector;
	CLI_MGR_error_et error = CLI_MGR_ERROR_NONE;
	char output_string[200];

	sprintf( output_string, "%dmv", SEN_MGR_get_batery_voltage() );

	CLI_MGR_send_newline();
	CLI_MGR_send_data( output_string, strlen(output_string));

	return( error );
}


	

CLI_MGR_error_et savenvm_handler( u8_t aArgCount, char *aArgVector[] )
{
	(void)aArgCount;
	(void)aArgVector;
	CLI_MGR_error_et error = CLI_MGR_ERROR_NONE;
	char output_string[200];

	/* Force a write to nvm */
	//NVM_set_override_state();
	NVM_request_flush();

	CLI_MGR_send_newline();

	sprintf( output_string, "NVM flush has been completed" );
	CLI_MGR_send_data( output_string, strlen(output_string));
	CLI_MGR_send_newline();

	return( error );
}




CLI_MGR_error_et set_id_handler( u8_t aArgCount, char *aArgVector[] )
{
	(void)aArgCount;
	(void)aArgVector;
	CLI_MGR_error_et error = CLI_MGR_ERROR_NONE;

	SEN_MGR_set_sensor_id( CLI_MGR_str_to_hex( aArgVector[1] ) );

	return( error );
}


CLI_MGR_error_et rtc_clear_handler( u8_t aArgCount, char *aArgVector[] )
{
	(void)aArgCount;
	(void)aArgVector;
	CLI_MGR_error_et error = CLI_MGR_ERROR_NONE;

	RTC_reset_date_and_time();

	return( error );
}

CLI_MGR_error_et set_wakeup_time_handler( u8_t aArgCount, char *aArgVector[] )
{
	(void)aArgCount;
	(void)aArgVector;
	CLI_MGR_error_et error = CLI_MGR_ERROR_NONE;

	RTC_set_wakeup_time( CLI_MGR_str_to_dec( aArgVector[1] ) );

	return( error );
}


CLI_MGR_error_et test_handler( u8_t aArgCount, char *aArgVector[] )
{
	(void)aArgCount;
	(void)aArgVector;
	CLI_MGR_error_et error = CLI_MGR_ERROR_NONE;

	while(1);

	return( error );
}


