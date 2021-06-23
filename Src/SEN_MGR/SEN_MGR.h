#ifndef SEN_MGR_H
#define SEN_MGR_H

/***************************************************************************************************
**                              Includes                                                          **
***************************************************************************************************/
#include "C_defs.h"
#include "STDC.h"


/***************************************************************************************************
**                              Defines                                                           **
***************************************************************************************************/
#define SEN_MGR_DEFAULT_SEN_ID          ( 0xFFFFFFFFu )
#define SEN_MGR_SST_SEN_TYPE            ( 0x1u )
#define SEN_MGR_DEFAULT_WAKEUP_TIME_SEC ( 254u )
#define SEN_MGR_MAX_WAKEUP_TIME_SEC 	( 15300u )

/***************************************************************************************************
**                              Constants                                                         **
***************************************************************************************************/
/* None */


/***************************************************************************************************
**                              Data Types and Enums                                              **
***************************************************************************************************/
typedef struct 
{
    u32_t sensor_id;
    u32_t runtime_secs;
    u8_t  sensor_type;
    u32_t wakeup_period_sec;
    u8_t  packet_type;
    float temperature;
    u16_t battery_mv;
} SEN_MGR_data_st;






/***************************************************************************************************
**                              Exported Globals                                                  **
***************************************************************************************************/
/* None */



/***************************************************************************************************
**                              Function Prototypes                                               **
******       *********************************************************************************************/
void         SEN_MGR_init( void );
void         SEN_MGR_update_sensor_id( void );
void         SEN_MGR_update_stored_params( void );
pass_fail_et SEN_MGR_set_sensor_type( u8_t sensor_type );
void         SEN_MGR_update_batery_voltage( void ); 
u8_t         SEN_MGR_get_sensor_type( void );
pass_fail_et SEN_MGR_set_sensor_id( u32_t id );
u32_t        SEN_MGR_get_sensor_id( void );
u16_t        SEN_MGR_get_batery_voltage( void );
void         SEN_MGR_set_wakeup_time_sec( u16_t wakeup_time );
u16_t        SEN_MGR_get_wakeup_time_sec( void );
u8_t         SEN_MGR_get_packet_type( void );
void         SEN_MGR_update_run_time( void );
void         SEN_MGR_update_temperature( void );
float        SEN_MGR_get_temperature( void );


#endif /* SEN_MGR_H multiple inclusion guard */

/****************************** END OF FILE *******************************************************/
