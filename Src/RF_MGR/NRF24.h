#ifndef NRF24_H
#define NRF24_H

/***************************************************************************************************
**                              Includes                                                          **
***************************************************************************************************/
#include "C_defs.h"
#include "NRF24_config.h"



/***************************************************************************************************
**                              Defines                                                           **
***************************************************************************************************/

/* Bit Mnemonics */
#define EN_CRC_POS      3
#define CRCO_POS        2
#define PWR_UP          1
#define PRIM_RX         0
#define ENAA_P5         5
#define ENAA_P4         4
#define ENAA_P3         3
#define ENAA_P2         2
#define ENAA_P1         1
#define ENAA_P0         0
#define ERX_P5          5
#define ERX_P4          4
#define ERX_P3          3
#define ERX_P2          2
#define ERX_P1          1
#define ERX_P0          0
#define AW_POS          0
#define ARD_POS         4
#define ARC_POS         0
#define PLL_LOCK_POS    4
#define RF_DR           3
#define RF_PWR          3
#define RX_DR_POS       6
#define TX_DS_POS       5
#define MAX_RT_POS      4
#define RX_P_NO_POS     1
#define TX_FULL_POS     0
#define PLOS_CNT_POS    4
#define ARC_CNT_POS     0
#define TX_REUSE_POS    6
#define FIFO_FULL_POS   5
#define TX_EMPTY_POS    4
#define RX_FULL_POS     1
#define RX_EMPTY_POS    0
#define DPL_P5	        5
#define DPL_P4	        4
#define DPL_P3	        3
#define DPL_P2	        2
#define DPL_P1	        1
#define DPL_P0	        0
#define EN_DPL_POS      2
#define EN_ACK_PAY      1
#define EN_DYN_ACK_POS  0
#define CONT_WAVE_POS   7

#define RF_PWR_MASK       0x06
#define RF_DR_MASK        0x28
#define ARD_MASK          0xF0
#define ARC_MASK          0x0F
#define RX_DR_MASK        0x40
#define TX_DS_MASK        0x20
#define MAX_RT_MASK       0x10
#define RX_P_NO_MASK      0x0E
#define TX_FULL_MASK      0x01
#define RF_DR_LOW_MASK    0x05
#define RF_DR_HIGH_MASK   0x03
#define TX_REUSE_MASK     0x40
#define TX_FIFO_FULL_MASK 0x20
#define TX_EMPTY_MASK     0x10
#define RX_FULL_MASK      0x02
#define RX_EMPTY_MASK     0x01
#define EN_CRC_MASK       0x08
#define CRCO_MASK         0x04
#define EN_DPL_MASK       0x0C
#define CONT_WAVE_MASK    0x80
#define PLL_LOCK_MASK     0x10
#define AW_MASK           0x03
#define ARC_CNTMASK       0x0F
#define EN_DYN_ACK_MASK   0x01

#define NRF24_WRITE_BIT             0x20
#define NRF_MAX_CHANNEL_SELECTION   126u
#define NRF_DEF_CHANNEL_SELECTION   108u
#define NRF_MAX_NUM_PIPES           5u
#define NRF_DATA_PIPE_SIZE          5u
#define NRF_MAX_NUM_RETRIES         15u
#define NRF_DATA_PIPE_OFFSET        10u
#define NRF_MAX_PAYLOAD_SIZE        32u
#define NRF_PAYLOAD_BUFFERS         3u





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
    NRF_POWER_DOWN_MODE = 0u,
    NRF_STANDBY_1_MODE,
    NRF_STANDBY_2_MODE,
    NRF_RX_MODE,
    NRF_TX_MODE
} NRF24_low_level_mode_et;



typedef enum
{
	CONFIG 			= 0,
	EN_AUTO_ACK,
	EN_RXADDR,
	SETUP_AW,
	SETUP_RETR,
	RF_CH,
	RF_SETUP,
	STATUS,
	OBSERVE_TX,
	CD,
	RX_ADDR_P0,
	RX_ADDR_P1,
	RX_ADDR_P2,
	RX_ADDR_P3,
	RX_ADDR_P4,
	RX_ADDR_P5,
	TX_ADDR ,
	RX_PW_P0,
	RX_PW_P1,
	RX_PW_P2,
	RX_PW_P3,
	RX_PW_P4,
	RX_PW_P5,
	FIFO_STATUS,
    ACK_PLD,
    TX_PLD,
    RX_PLD,
    DYNPD,
	FEATURE,
    ADDRESS_NOP,
	REGISTER_MAX
} NRF24_registers_et;

typedef enum
{
    /* Instruction Mnemonics */
    R_REGISTER          = 0x00,
    W_REGISTER          = 0x20,
    REGISTER_MASK       = 0x1F,
    ACTIVATE            = 0x50,
    R_RX_PL_WID         = 0x60,
    R_RX_PAYLOAD        = 0x61,
    W_TX_PAYLOAD        = 0xA0,
    W_ACK_PAYLOAD       = 0xA8,
    FLUSH_TX            = 0xE1,
    FLUSH_RX            = 0xE2,
    REUSE_TX_PL         = 0xE3,
    W_TX_PAYLOAD_NO_ACK = 0xB0,
    NOP                 = 0xFF
} NRF24_instruction_et;


typedef enum
{
    RF_MIN_TX_PWR = 0u,
    RF_MED_1_TX_PWR,
    RF_MED_2_TX_PWR,
    RF_MAX_TX_PWR
} NRF24_power_level_et;

typedef enum
{
  	TX_ENABLE = 0u,
	RX_ENABLE
} NRF24_tx_rx_mode_et;

typedef enum
{
    ADDRESS_ILLEGAL = 0u,
  	ADDRESS_3_BYTES,
  	ADDRESS_4_BYTES,
  	ADDRESS_5_BYTES
} NRF24_address_width_et;

typedef enum
{
    RF24_1MBPS = 0u,
    RF24_2MBPS,
    RF24_250KBPS
} NRF24_air_data_rate_et;

typedef enum
{
    RF24_250_US = 0u,
    RF24_500_US,
    RF24_750_US,
    RF24_1000_US,
    RF24_1250_US,
    RF24_1500_US,
    RF24_1750_US,
    RF24_2000_US,
    RF24_2250_US,
    RF24_2500_US,
    RF24_2750_US,
    RF24_3000_US,
    RF24_3250_US,
    RF24_3500_US,
    RF24_3750_US,
    RF24_4000_US
} NRF24_retransmitt_time_et;

typedef enum
{
  RF24_CRC_8 = 0,
  RF24_CRC_16
} NRF24_crclength_et;

typedef enum
{
    RF24_RX_DATA_READY = 0u,
    RF24_TX_DATA_SENT,
    RF24_MAX_RETR_REACHED,
    RF24_RX_PIPE_DATA_NUM,
    RF24_TX_FIFO_FULL
} NRF24_status_masks_et;

typedef enum
{
    RF24_RX_EMPTY = 0u,
    RF24_RX_FULL,
    RF24_TX_EMPTY,
    RF24_TX_FULL,
    RF24_TX_REUSE
} NRF24_fifo_masks_et;



typedef struct
{
    u8_t a;
    u8_t b;
} trial_st;

/***************************************************************************************************
**                              Exported Globals                                                  **
***************************************************************************************************/
/* None */



/***************************************************************************************************
**                              Function Prototypes                                               **
***************************************************************************************************/
pass_fail_et         NRF24_setup_transceiver( void );
false_true_et        NRF24_reg_static_check( NRF24_registers_et reg );
pass_fail_et         NRF24_set_configuration( NRF24_configuration_et config );
pass_fail_et         NRF24_set_low_level_mode( NRF24_low_level_mode_et mode );
pass_fail_et         NRF24_set_channel( u8_t channel );
pass_fail_et         NRF24_flush_rx( void ) ;
pass_fail_et         NRF24_flush_tx( void );
NRF24_power_level_et NRF24_get_PA_TX_power(void);
pass_fail_et         NRF24_set_PA_TX_power( NRF24_power_level_et value);
u8_t                 NRF24_get_status(void);
pass_fail_et         NRF24_set_rf_data_rate( NRF24_air_data_rate_et value );
void                 NRF24_setup_CRC_scheme( disable_enable_et state, NRF24_crclength_et crc_len );
void                 NRF24_setup_constant_wave( disable_enable_et state );
void                 NRF24_setup_pll( disable_enable_et state );
pass_fail_et         NRF24_open_write_data_pipe( u8_t pipe_num, const u8_t* data_pipe_address );
u8_t                 NRF24_get_FIFO_status(void);
pass_fail_et         NRF24_status_register_clr_bit( u8_t bit_mask );
pass_fail_et         NRF24_send_payload( u8_t* buffer, u8_t len );
pass_fail_et         NRF24_get_payload( u8_t* buffer );
pass_fail_et         NRF24_toggle_features_register( void );
pass_fail_et         NRF24_set_AA_data_pipe( disable_enable_et state, u8_t pipe_num );
pass_fail_et         NRF24_set_pipe_payload_size( u8_t size, u8_t pipe_num );
pass_fail_et         NRF24_set_reuse_tx_payload( disable_enable_et state );
pass_fail_et         NRF24_set_dynamic_payloads( disable_enable_et state, u8_t pipe_num );
void                 NRF24_setup_address_widths( NRF24_address_width_et value );
pass_fail_et         NRF24_enable_data_pipe( disable_enable_et state, u8_t pipe_num );
pass_fail_et         NRF24_setup_retries( NRF24_retransmitt_time_et time, u8_t counts );
low_high_et          NRF24_check_status_mask( NRF24_status_masks_et mask );
low_high_et          NRF24_check_fifo_mask( NRF24_fifo_masks_et mask );
pass_fail_et         NRF24_read_data_pipe( u8_t pipe_num, const u8_t* data_p );
u8_t 				 NRF24_get_retry_count( void );
pass_fail_et 		 NRF24_setup_dynamic_ack( disable_enable_et state );
pass_fail_et         NRF24_set_tx_address( u8_t data_pipe_address[NRF_DATA_PIPE_SIZE] );

void                 NRF24_complete_flush( void );
pass_fail_et         NRF24_handle_acks( void );
pass_fail_et         NRF24_handle_tx_failures( void );
false_true_et        NRF24_check_for_packet_received( void );
pass_fail_et         NRF24_self_check( void );
void                 NRF24_update_shadow_regs( void );

#endif /* NRF24_H multiple inclusion guard */

/****************************** END OF FILE *******************************************************/
