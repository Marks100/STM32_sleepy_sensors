#ifndef NRF24_REGISTERS_H
#define NRF24_REGISTERS_H

/***************************************************************************************************
**                              Includes                                                          **
***************************************************************************************************/
#include "NRF24.h"



/***************************************************************************************************
**                              Defines                                                           **
***************************************************************************************************/


/***************************************************************************************************
**                              Exported Globals                                                  **
***************************************************************************************************/

#define DEFAULT_CONFIGURATION_SIZE      ( 27u )

/* Default register set 433Mhz ( 19200bps ) */
const NRF24_register_data_st NRF24_default_register_set[] =
{
    { CONFIG,       0x0E},
	{ EN_AUTO_ACK,  0x00},
	{ EN_RXADDR,    0x01},
    { SETUP_AW,     0x03},
	{ SETUP_RETR,   0x00},
	{ RF_CH,        0x6C},
	{ RF_SETUP,     0x26},
	{ STATUS,       0x00},
	{ OBSERVE_TX,   0x00},
	{ CD,           0x00},
	{ RX_ADDR_P0,   0x00},
	{ RX_ADDR_P1,   0x00},
	{ RX_ADDR_P2,   0x00},
	{ RX_ADDR_P3,   0x00},
	{ RX_ADDR_P4,   0x00},
	{ RX_ADDR_P5,   0x00},
	{ TX_ADDR ,     0x00},
	{ RX_PW_P0,     0x20},
	{ RX_PW_P1,     0x00},
	{ RX_PW_P2,     0x00},
	{ RX_PW_P3,     0x00},
	{ RX_PW_P4,     0x00},
	{ RX_PW_P5,     0x00},
	{ FIFO_STATUS,  0x00},
	{ DYNPD,        0x00},
	{ FEATURE,      0x00},
    { ADDRESS_NOP,  0x00},
};





const NRF24_static_configuration_st NRF24_config_c[ 1 ] =
{
    { (NRF24_register_data_st * )NRF24_default_register_set, DEFAULT_CONFIGURATION_SIZE },
};



#endif /* NRF24_REGISTERS_H multiple inclusion guard */

/****************************** END OF FILE *******************************************************/
