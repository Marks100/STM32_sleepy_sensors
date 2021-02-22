#ifndef RF_MGR_H
#define RF_MGR_H

/***************************************************************************************************
**                              Includes                                                          **
***************************************************************************************************/
#include "C_defs.h"
#include "NRF24.h"

/***************************************************************************************************
**                              Defines                                                           **
***************************************************************************************************/

#define RF_MGR_MAX_PAYLOAD_SIZE      NRF_MAX_PAYLOAD_SIZE
#define RF_MGR_MSG_RETRY_CNT         2u

#define RF_WATCHDOG_TIMEOUT			 ( 60u * 60u )	//1hr


/***************************************************************************************************
**                              Constants                                                         **
***************************************************************************************************/
/* None */


/***************************************************************************************************
**                              Data Types and Enums                                              **
***************************************************************************************************/

typedef enum
{
    RF_TX_IDLE = 0u,
    RF_TX_IN_PROGRESS,
    RF_MGR_RETRY_LIMIT_REACHED,
    RF_MGR_TX_SUCCESS,

} RF_MGR_tx_status_et;


typedef enum
{
	RF_MGR_MISSING_SENSOR = 0u
} RF_MGR_generic_dtc_et;


typedef enum
{
    RF_MGR_INITIALISING = 0u,
    RF_MGR_IDLE,
    RF_MGR_SETUP_TX,
    RF_MGR_TX,
    RF_MGR_TX_WAIT_COMPLETE,
    RF_MGR_TX_TEST_MODE,
    RF_MGR_RX,
    RF_MGR_RX_TEST_MODE,
    RF_MGR_POWER_DOWN,
    RF_MGR_SETUP_CONST_WAVE,
    RF_MGR_CONST_WAVE,
    RF_MGR_FAULT,
    RF_MGR_RESET
} RF_MGR_rf_state_et;


typedef struct
{
    u8_t rx_payload[NRF_MAX_PAYLOAD_SIZE];
    u8_t tx_payload[NRF_MAX_PAYLOAD_SIZE];
    u8_t rx_packet_num;
    u8_t tx_packet_num;
    u32_t tx_timeouts;
} RF_MGR_rf_data_store_st;









/***************************************************************************************************
**                              Exported Globals                                                  **
***************************************************************************************************/
/* None */



/***************************************************************************************************
**                              Function Prototypes                                               **
***************************************************************************************************/
void RF_MGR_init( void );
void RF_MGR_tick( void );
void RF_MGR_populate_rf_frame( void );
void RF_MGR_send_rf_frame( void );
void RF_MGR_set_state( RF_MGR_rf_state_et state );
RF_MGR_rf_state_et RF_MGR_get_state( void );




#endif /* RF_MGR_H multiple inclusion guard */

/****************************** END OF FILE *******************************************************/
