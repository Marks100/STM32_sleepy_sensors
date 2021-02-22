#ifndef DBG_LOG_MGR_H
#define DBG_LOG_MGR_H

#include "C_defs.h"
#include "COMPILER_defs.h"




/*******************************************************************************
**                              Defines                                       **
*******************************************************************************/
#define DBG_LOG_MGR_LOGGER( message ) DBG_LOG_MGR_LOG( __FILE__, __LINE__, message )

/*******************************************************************************
**                              Typedefs                                      **
*******************************************************************************/
void DBG_LOG_MGR_init( void );
void DBG_LOG_MGR_LOG( char* file, u16_t line, u8_t* message );



#endif  // Multiple Inclusion Trap
