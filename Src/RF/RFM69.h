/*! \file
*               $Revision: 16923 $
*
*               $Author: mstewart $
*
*               $Date: 2014-01-16 15:40:40 +0000 (Thu, 16 Jan 2014) $
*
*               $HeadURL: https://selacvs01.schrader.local:8443/svn/ECU_Software/LF_TOOL_GEN2/trunk/Src/HAL/RF/RF.h $
*
*   \brief      Public header file for RF module.
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
#ifndef RFM69_H
#define RFM69_H

/***************************************************************************************************
**                              Includes                                                          **
***************************************************************************************************/
#include "C_defs.h"
#include "COMPILER_defs.h"


/***************************************************************************************************
**                              Defines                                                           **
***************************************************************************************************/
#define RFM69_VERSION           ( 0x24 )
#define RFM69_WRITE_ACCESS      ( 0x80 )
#define RFM69_MAX_TX_POWER_LEVEL 31u
#define ENCRYPT_KEY_SIZE         16u
#define FREQ_STEP                61.03515625f
#define RFM69_MAX_DATA_LEN       66u
#define RFM69_MAX_PAYLOAD_LEN    64u  //! We will stick to this as this is the maximum number of bytes that can use the AES128 encryption scheme ( 64byte FIFO ) + 1 len byte */

#define NODE_OWN_ADDRESS	     0x01

#define BIT_MASK_1_BIT 1
#define BIT_MASK_2_BIT 3
#define BIT_MASK_3_BIT 7
#define BIT_MASK_4_BIT 15

/* BIT Pneumonics ( shift patterns ) */
#define SLEEP                0x00
#define STDBY                0x04
#define FS                   0x08
#define TX                   0x0C
#define RX                   0x10
#define LISTEN_ABORT         0x20
#define LISTENON             0x40
#define SEQUENCER_OFF        0x80
#define PACKET_FORMAT        0x80
#define CLK_OUT_OFF          0x07
#define TEMP_MEASURE_START   0x08
#define TEMP_MEASURE_RUNNING 0x04
#define MODULATION_TYPE      0x18
#define DATA_PROCESS_MODE    0x60
#define LISTEN_IDLE_RES      0xC0
#define LISTEN_RX_RES        0x30
#define PA0_ON               0x80
#define PA1_ON               0x40
#define PA2_ON               0x20
#define RSSI_START           0x01
#define AES_ON               0x01
#define DIO0                 0xC0
#define DIO1                 0x30
#define DIO2                 0x0C
#define DIO3                 0x03
#define DIO4                 0xC0
#define DIO5                 0x30


#define COURSE_TEMP_COEF    -90 // puts the temperature reading in the ballpark, user can fine tune the returned value


/***************************************************************************************************
**                              Constants                                                         **
***************************************************************************************************/
/* None */


/***************************************************************************************************
**                              Data Types and Enums                                              **
***************************************************************************************************/
/* None */


typedef enum
{
    REGFIFO          = 0x00,
    REGOPMODE        = 0x01,
    REGDATAMODUL     = 0x02,
    REGBITRATEMSB    = 0x03,
    REGBITRATELSB    = 0x04,
    REGFDEVMSB       = 0x05,
    REGFDEVLSB       = 0x06,
    REGFRFMSB        = 0x07,
    REGFRFMID        = 0x08,
    REGFRFLSB        = 0x09,
    REGOSC1          = 0x0A,
    REGAFCCTRL       = 0x0B,
    RESERVED0C       = 0x0C,
    REGLISTEN1       = 0x0D,
    REGLISTEN2       = 0x0E,
    REGLISTEN3       = 0x0F,
    REGVERSION       = 0x10,
    REGPALEVEL       = 0x11,
    REGPARAMP        = 0x12,
    REGOCP           = 0x13,
    RESERVED14       = 0x14,
    RESERVED15       = 0x15,
    RESERVED16       = 0x16,
    RESERVED17       = 0x17,
    REGLNA           = 0x18,
    REGRXBW          = 0x19,
    REGAFCBW         = 0x1A,
    REGOOKPEAK       = 0x1B,
    REGOOKAVG        = 0x1C,
    REGOOKFIX        = 0x1D,
    REGAFCFEI        = 0x1E,
    REGAFCMSB        = 0x1F,
    REGAFCLSB        = 0x20,
    REGFEIMSB        = 0x21,
    REGFEILSB        = 0x22,
    REGRSSICONFIG    = 0x23,
    REGRSSIVALUE     = 0x24,
    REGDIOMAPPING1   = 0x25,
    REGDIOMAPPING2   = 0x26,
    REGIRQFLAGS1     = 0x27,
    REGIRQFLAGS2     = 0x28,
    REGRSSITHRESH    = 0x29,
    REGRXTIMEOUT1    = 0x2A,
    REGRXTIMEOUT2    = 0x2B,
    REGPREAMBLEMSB   = 0x2C,
    REGPREAMBLELSB   = 0x2D,
    REGSYNCCONFIG    = 0x2E,
    REGSYNCVALU1     = 0x2F,
    REGSYNCVALU2     = 0x30,
    REGSYNCVALU3     = 0x31,
    REGSYNCVALU4     = 0x32,
    REGSYNCVALU5     = 0x33,
    REGSYNCVALU6     = 0x34,
    REGSYNCVALU7     = 0x35,
    REGSYNCVALU8     = 0x36,
    REGPACKETCONFIG1 = 0x37,
    REGPAYLOADLENGTH = 0x38,
    REGNODEADRS      = 0x39,
    REGBROADCASTADRS = 0x3A,
    REGAUTOMODES     = 0x3B,
    REGFIFOTHRESH    = 0x3C,
    REGPACKETCONFIG2 = 0x3D,
    REGAESKEY1       = 0x3E,
    REGAESKEY2       = 0x3F,
    REGAESKEY3       = 0x40,
    REGAESKEY4       = 0x41,
    REGAESKEY5       = 0x42,
    REGAESKEY6       = 0x43,
    REGAESKEY7       = 0x44,
    REGAESKEY8       = 0x45,
    REGAESKEY9       = 0x46,
    REGAESKEY10      = 0x47,
    REGAESKEY11      = 0x48,
    REGAESKEY12      = 0x49,
    REGAESKEY13      = 0x4A,
    REGAESKEY14      = 0x4B,
    REGAESKEY15      = 0x4C,
    REGAESKEY16      = 0x4D,
    REGTEMP1         = 0x4E,
    REGTEMP2         = 0x4F,
    REGTESTLNA       = 0x58,
    REGTESTPA1       = 0x5A,
    REGTESTPA2       = 0x5C,
    REGTESTDAGC      = 0x6F,
    REGTESTAFC       = 0x71,
    REG_MAX
} RFM69_registers_et;


typedef enum
{
    RFM69_MGR_POWERING_UP  = 0,
    RFM69_MGR_INITIALISING,
    RFM69_MGR_RUNNING,
    RFM69_MGR_SETUP_TX_MODE,
    RFM69_MGR_TX_MODE,
    RFM69_MGR_SETUP_RX_MODE,
    RFM69_MGR_RX_MODE,
    RFM69_MGR_SLEEP_MODE,
    RFM69_MGR_IDLE,
    RFM69_MGR_RF_RESET,
} RFM69_state_et;



typedef enum
{
    RFM69_DEFAULT_CONFIG,      //! Amplitude Shift Key 915MHz
    RFM69_433Mhz_OOK,          //! OOk 433Mhz
    RFM69_CFG_MAX
} RFM69_static_configuration_et;


typedef enum
{
    RFM69_SLEEP_MODE = 0u,
    RFM69_STANDBY_MODE,
    RFM69_FS_MODE,
    RFM69_TRANSMIT_MODE,
    RFM69_RECEIVE_MODE,
    RFM69_LISTEN_MODE
} RFM69_operating_modes_et;


typedef enum
{
    RFM69_FIXED_PACKET_MODE = 0u,
    RFM69_VARIABLE_PACKET_MODE,
} RFM69_packet_modes_et;


typedef enum
{
    RFM69_FSK = 0u,
    RFM69_OOK,
    RFM69_RESERVED
} RFM69_modulation_type_et;

typedef enum
{
    RFM69_PACKET_MODE = 0u,
    RFM69_CONT_WITH_BIT_SYNC = 2u,
    RFM69_CONT_WITHOUT_BIT_SYNC = 3u,
    RFM69_MODE_MAX
} RFM69_data_mode_et;


//! RFM69 instructions
typedef enum
{
    WRITE_TO_CHIP_BURST_MODE    = 0x01,     //! Writes data to a number of consecutive registers
    WRITE_TO_CHIP               = 0x02,     //! Writes 1 byte of data to a specific register
    READ_FROM_CHIP              = 0x03,     //! Reads 1 byte of data from a specific register
    READ_FIFO_FROM_CHIP         = 0x04,     //! Reads data from the FIFO buffer
    READ_FROM_CHIP_BURST_MODE   = 0x05,     //! Reads data from a number of consecutive registers
    WRITE_TO_CHIP_BURST_MODE_CONF   = 0x06,     //! writes data from a config structure
    RFM69_INSTRUCTION_MAX                   //! RFM69_INSTRUCTION_MAX
} RFM69_instruction_et;

typedef enum
{
    RFM_69_64us = 1u,
    RFM_69_4100us,
    RFM_69_26200us
} RFM69_listen_time_et;

typedef enum
{
    RFM69_LISTEN_IDLE = 0u,
    RFM69_LISTEN_RX
} RFM69_listen_state_et;

typedef enum
{
    RFM69_1200bps = 0u,
    RFM69_2400bps,
    RFM69_4800bps,
    RFM69_9600bps,
    RFM69_19200bps,
    RFM69_38400bps,
    RFM69_76800bps,
    RFM69_153600bps
} RFM69_predefined_bitrates_et;


typedef enum
{
    RFM69_DIO_MODE_0 = 0u,
    RFM69_DIO_MODE_1,
    RFM69_DIO_MODE_2,
    RFM69_DIO_MODE_3,
} RFM69_DIO_map_mode_et;

typedef struct
{
   RFM69_registers_et RFM69_register;
   u8_t register_data;
} RFM69_register_data_st;


typedef struct
{
    RFM69_register_data_st* buffer_p; //! pointer to configuration
    u16_t length;                      //! length of configuration
} RFM69_static_configuration_st;


typedef struct
{
	u8_t tx_rd_data[32];
} RFM69_data_packet_st;


/***************************************************************************************************
**                              Exported Globals                                                  **
***************************************************************************************************/
/* None */




/***************************************************************************************************
**                              Function Prototypes                                               **
***************************************************************************************************/
void          RFM69_wakeup_and_send( void ); //( init )
pass_fail_et  RFM69_read_registers( RFM69_instruction_et instruction, RFM69_registers_et address, u8_t read_data[], u8_t num_bytes );
pass_fail_et  RFM69_write_registers( RFM69_instruction_et instruction, RFM69_registers_et address, u8_t write_data[], u8_t num_bytes );
pass_fail_et  RFM69_set_configuration( RFM69_static_configuration_et config );
void 		  RFM69_get_configuration( RFM69_static_configuration_et config, RFM69_register_data_st* data_p );
void          RFM69_set_reset_pin_state( low_high_et state );
void 		  RFM69_set_enable_pin_state( low_high_et state );

/* Writes */
false_true_et RFM69_set_clock_out( disable_enable_et state );
u8_t 		  RFM69_set_PA_level( u8_t level );
false_true_et RFM69_set_operating_mode( RFM69_operating_modes_et operating_mode );
false_true_et RFM69_set_packet_mode( RFM69_packet_modes_et operating_mode );
false_true_et RFM69_set_rx_payload_length( u8_t num_bytes );
false_true_et RFM69_set_modulation_type( RFM69_modulation_type_et type );
false_true_et RFM69_set_data_mode( RFM69_data_mode_et mode );
false_true_et RFM69_set_bit_rate( RFM69_predefined_bitrates_et bit_rate );
false_true_et RFM69_set_listen_time_resolution( RFM69_listen_state_et state, RFM69_listen_time_et listen_time_resolution );
false_true_et RFM69_set_listen_time( RFM69_listen_state_et state, u16_t time );
false_true_et RFM69_set_own_node_address( u8_t address );
false_true_et RFM69_set_own_network_id( u8_t network_id );
false_true_et RFM69_trigger_RSSi_measurement( void );
false_true_et RFM69_set_encryption_key( u8_t* key, false_true_et state );
false_true_et RFM69_set_DIO_mapping( u8_t pin, RFM69_DIO_map_mode_et mode );
false_true_et RFM69_write_to_FIFO( u8_t* buffer, u8_t len );
false_true_et RFM69_Send_frame( u8_t* buffer, u8_t len, u8_t rx_node_address );
void RFM69_update_packet_sent( false_true_et state );


/* Reads */
u8_t RFM69_read_version_num( void );
pass_fail_et RFM69_read_reserved_registers( void );
u8_t RFM69_read_temp( void );
u8_t RFM69_read_current_LNA_gain( void );
u32_t RFM69_read_rf_carrier_freq( void );
false_true_et RFM69_read_FIFO_register( u8_t* data_p );


#endif /* RFM69_H multiple inclusion guard */

/****************************** END OF FILE *******************************************************/
