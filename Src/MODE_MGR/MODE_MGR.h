#ifndef MODE_MGR_H
#define MODE_MGR_H

#include "C_defs.h"
#include "COMPILER_defs.h"




/*******************************************************************************
**                              Defines                                       **
*******************************************************************************/
#define MODE_MGR_BATTERY_CRITICAL_THRESHOLD    3300u

#define MODE_MGR_MAX_TICK_CYCLE_VALUE 	      1000u
#define MODE_MGR_TICK_RATE_MSECS		      10u

#define SECS_TO_MINS(x)                       ( x >= (60u) ? TRUE : FALSE )



/*******************************************************************************
**                              Typedefs                                      **
*******************************************************************************/


typedef enum
{
	MODE_MGR_NORMAL_MODE = 0u,
	MODE_MGR_DEBUG_MODE
} MODE_MGR_mode_et;



void             MODE_MGR_init( void );
void             MODE_MGR_tick( void );
void             MODE_MGR_set_operating_mode( MODE_MGR_mode_et mode );
MODE_MGR_mode_et MODE_MGR_get_operating_mode( void );
void             MODE_MGR_read_operating_mode( void );
void             MODE_MGR_prepare_for_lowpower_mode( void );
void             MODE_MGR_enter_lowpower_mode( void );
void 			 MODE_MGR_run_activity( void );
void 			 MODE_MGR_setup_for_bl( void );


#endif  // Multiple Inclusion Trap
