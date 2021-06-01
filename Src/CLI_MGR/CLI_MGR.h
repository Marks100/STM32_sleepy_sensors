#ifndef CLI_MGR_H
#define CLI_MGR_H


#define CLI_MGR_MAX_INPUT_CHARS           40u
#define CLI_MGR_CMD_LINE_ARGS_MAX         6
#define CLI_MGR_MAX_COMMAND_HISTORY       6
#define CLI_MGR_BACKSPACE                 0x08
#define CLI_MGR_CR                        0x0D
#define CLI_MGR_SPACE                     0x20
#define CLI_MGR_DEL                       0x7F

#define UP_ARROW                      0x41
#define DOWN_ARROW                    0x42
#define LEFT_ARROW					  0x43
#define RIGHT_ARROW					  0x44
#define ERASE_CURRENT_LINE            "\r                                                                        \r\b\b\b"
#define IGNORE_RANGE_CHECK            ((u32_t)~0)

#define RED			"\x1b[31m"
#define GREEN		"\x1b[32m"
#define BLUE		"\x1b[34m"
#define YELLOW		"\x1b[33m"
#define CYAN		"\x1b[36m"
#define MAGENTA		"\x1b[35m"
#define WHITE		"\x1b[37m"

#define SUPPORTED_FOR_ALL_MODES                      0xFFFF

#define HELP_HELP                "help:              Prints the help menu\r\n"
#define HELP_VER                 "ver:               Returns the SW and HW versions of the sensor\r\n"
#define HELP_SET_MODE            "setmode:           Sets the operating mode of the device\r\n"
#define HELP_RESET               "reset:             Resets CPU\r\n"
#define HELP_NVM                 "nvm:               Returns the current NVM info\r\n"
#define HELP_CLOCKS				 "clocks:            Reads the values of the internal clocks\r\n"
#define HELP_TEMP				 "temp:              Reads the internal temperature value of the device\r\n"
#define HELP_BATT				 "batt:              Reads the battery/supply voltoge of the device\r\n"
#define HELP_TEST                "test:              Used for random tests\r\n"
#define HELP_SAVEMEVM            "savenvm:           Requests a flush of the NVM data into flash memory\r\n"
#define HELP_SET_ID              "setid:             Sets the ID in HEX of the sensor \r\n"
#define HELP_RTC_CLR             "rtcclr:            Clears the stored time on the RTC \r\n"
#define HELP_WAKEUP_TIME         "setwkp:            Sets the wakeup time of the sensor in secs \r\n"

#define NULL_PARAM_LIST { { 0, (u32_t)0x0000, (u32_t)0x0000 } }
#define SET_ID_CMD_PARAM_LIST { { 8, (u32_t)0x1, (u32_t)0xFFFFFFFE } }
#define SET_WAKEUP_TIME_PARAM_LIST { { 8, (u32_t)1, (u32_t)0xFFFFFFFE } }

typedef enum
{
    CLI_MGR_ERROR_NONE = 0,
    CLI_MGR_ERROR_EMPTY,
    CLI_MGR_ERROR_FAILED,
    CLI_MGR_ERROR_INVALID_ARGS_NUM,
    CLI_MGR_ERROR_INVALID_ARGS,
    CLI_MGR_ERROR_NOT_SUPPORTED,
    CLI_MGR_ERROR_PROHIBITED,
    CLI_MGR_ERROR_NOT_FOUND,
    CLI_MGR_ERROR_PARAM_OUT_OF_RANGE,
    CLI_MGR_ERROR_INVALID_HEX_PARAM,
    CLI_MGR_ERROR_INVALID_DEC_PARAM,
    CLI_MGR_ERROR_INVALID_HEX_PARAM_LEN,
    CLI_MGR_ERROR_INVALID_DEC_PARAM_LEN,
    CLI_MGR_ALREADY_SET,
    CLI_MGR_ERROR_INVALID_STR_PARAM,
    CLI_MGR_ERROR_MAX
} CLI_MGR_error_et;

typedef struct
{
    u8_t cmd[CLI_MGR_MAX_INPUT_CHARS + 1u];
}CLI_MGR_cmd_st;


typedef struct
{
    u8_t index_to_next_cmd;
    CLI_MGR_cmd_st cmd_list[CLI_MGR_MAX_COMMAND_HISTORY];
}CLI_MGR_cmd_history_st;

typedef struct
{
    u8_t  num_chars;
    u32_t llimit;
    u32_t ulimit;
} CLI_MGR_Param_st;

typedef struct
{
    const char *command_name;
    CLI_MGR_error_et (*command_handler)(u8_t aArgCount, char *aArgVector[] );
    const char *helpinfo;
    u16_t availbility;  			// bit map setting of all modes in which this cmd would be supported
    disable_enable_et public;       // if this is TRUE it means this command is exposed to customers otherwise it is for developers
    u8_t num_params;
    CLI_MGR_Param_st param_list[CLI_MGR_CMD_LINE_ARGS_MAX];
} CLI_MGR_Command_st;



void               CLI_MGR_init( void );
void               CLI_MGR_display_startup_message( void );
void               CLI_MGR_message_handler( void );
void               CLI_MGR_send_newline( void );
void               CLI_MGR_print_welcome_message( void );
void               CLI_MGR_handle_received_char( u8_t received_char );
void               CLI_MGR_send_data( char* data, u16_t data_size );
void 		       CLI_MGR_handle_serial_command ( void );
false_true_et      CLI_MGR_is_hex_char(char c);
false_true_et      CLI_MGR_is_dec_char(char c);
false_true_et      CLI_MGR_carriage_return_check( void );
void               CLI_MGR_clear_rx_buffer( void );
void               CLI_MGR_print_prompt( false_true_et newline );
false_true_et      CLI_MGR_is_space_or_newLine( char c );
unsigned long long CLI_MGR_str_to_hex( const char* str );
unsigned long long CLI_MGR_str_to_dec( const char* str );
CLI_MGR_error_et   CLI_MGR_parse_cmd( char* message_string, u8_t* calc_argumen_count, char *argument_vector[], u8_t max_num_args );  
CLI_MGR_error_et   CLI_MGR_process_cmd( u8_t  aArgCount, char *aArgVector[] );
CLI_MGR_error_et   CLI_MGR_validate_arguments( u8_t  aArgCount, char *aArgVector[], u8_t command_index );
void 		       CLI_MGR_store_history( void );
  
/* CLI handler functions */
CLI_MGR_error_et help_handler( u8_t aArgCount, char *aArgVector[] );
CLI_MGR_error_et ver_handler( u8_t aArgCount, char *aArgVector[] );
CLI_MGR_error_et setmode_handler( u8_t aArgCount, char *aArgVector[] );
CLI_MGR_error_et reset_handler( u8_t aArgCount, char *aArgVector[] );
CLI_MGR_error_et readnvm_handler( u8_t aArgCount, char *aArgVector[] );
CLI_MGR_error_et nvm_handler( u8_t aArgCount, char *aArgVector[] );
CLI_MGR_error_et clocks_handler( u8_t aArgCount, char *aArgVector[] );
CLI_MGR_error_et temp_handler( u8_t aArgCount, char *aArgVector[] );
CLI_MGR_error_et batt_handler( u8_t aArgCount, char *aArgVector[] );
CLI_MGR_error_et test_handler( u8_t aArgCount, char *aArgVector[] );
CLI_MGR_error_et savenvm_handler( u8_t aArgCount, char *aArgVector[] );
CLI_MGR_error_et set_id_handler( u8_t aArgCount, char *aArgVector[] );
CLI_MGR_error_et rtc_clear_handler( u8_t aArgCount, char *aArgVector[] );
CLI_MGR_error_et set_wakeup_time_handler( u8_t aArgCount, char *aArgVector[] );

#endif
