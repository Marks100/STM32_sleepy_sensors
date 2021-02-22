#ifndef MAIN_H
#define MAIN_H

#include "c_defs.h"
#include "compiler_defs.h"

void MAIN_delay_ms(u16_t);
void MAIN_delay_us(u16_t us);
u8_t generate_random_number( void );
void check_failures( void );
void populate_rf_frame( void );


#endif
