/*! \file
*               Author: mstewart
*   \brief      NRF24 config module
*/
/***************************************************************************************************
**                              Includes                                                          **
***************************************************************************************************/
#include "NRF24_config.h"
#include "NRF24.h"


const NRF24_reg_config_st NRF24_SST_config_c[] = 
{
    { CONFIG,       1u, { 0x5E } }, 
	{ EN_AUTO_ACK,  1u, { 0x01 } },
	{ EN_RXADDR,    1u, { 0x01 } }, 
    { SETUP_AW,     1u, { 0x03 } },
	{ SETUP_RETR,   1u, { 0x84 } },
	{ RF_CH,        1u, { 0x6C } },
	{ RF_SETUP,     1u, { 0x26 } },
	{ RX_ADDR_P0,   5u, { 0xE7, 0xE7, 0xE7, 0xE7, 0xE7 } },
	{ RX_ADDR_P1,   5u, { 0xC2, 0xC2, 0xC2, 0xC2, 0xC2 } },
	{ RX_ADDR_P2,   1u, { 0xC3 } },
	{ RX_ADDR_P3,   1u, { 0xC4 } },
	{ RX_ADDR_P4,   1u, { 0xC5 } },
	{ RX_ADDR_P5,   1u, { 0xC6 } },
	{ TX_ADDR ,     5u, { 0xE7, 0xE7, 0xE7, 0xE7, 0xE7 } },
	{ RX_PW_P0,     1u, { 0x20 } },
	{ RX_PW_P1,     1u, { 0x20 } },
	{ RX_PW_P2,     1u, { 0x20 } },
	{ RX_PW_P3,     1u, { 0x20 } },
	{ RX_PW_P4,     1u, { 0x20 } },
	{ RX_PW_P5,     1u, { 0x20 } },
	{ DYNPD,        1u, { 0x00 } },
	{ FEATURE,      1u, { 0x00 } },
};


const NRF24_config_table_st NRF24_config_table_c[] = 
{
    { NRF24_SST_config_c, 22u }
};


/***************************************************************************************************
**                              Private Functions                                                 **
***************************************************************************************************/


/****************************** END OF FILE *******************************************************/
