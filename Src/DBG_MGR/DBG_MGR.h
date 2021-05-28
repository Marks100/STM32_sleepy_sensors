#ifndef DBG_MGR_H
#define DBG_MGR_H

#include "C_defs.h"
#include "COMPILER_defs.h"




/*******************************************************************************
**                              Defines                                       **
*******************************************************************************/
#define DBG_MGR_LOG_MSG( message ) DBG_MGR_log_msg( __FILE__, __LINE__, message )
#define DBG_MGR_TIMING_WINDOWS ( 10u )

/*******************************************************************************
**                              Typedefs                                      **
*******************************************************************************/
typedef struct 
{
    u32_t cycle_start;
    u32_t cycle_stop;
    u32_t cycle_delta;
    float elapsed_time_us;
} DBG_MGR_dwt_timing_st;


typedef struct 
{   DBG_MGR_dwt_timing_st windows[DBG_MGR_TIMING_WINDOWS];
    float cycle_time_us;
} DBG_MGR_dwt_st;



void  DBG_MGR_init( void );
void  DBG_MGR_log_msg( char* file, u16_t line, u8_t* message );
void  DBG_MGR_start_time_log( u8_t window );
u32_t DBG_MGR_stop_time_log( u8_t window );



#endif  // Multiple Inclusion Trap
