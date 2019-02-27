#ifndef MAIN_H
#define MAIN_H

#include "c_defs.h"
#include "compiler_defs.h"

void delay_ms(u16_t);
void delay_us(u16_t us);
u8_t generate_random_number( void );

s8_t calculate_NTC_temperature( void );
u8_t calculate_humidity( void );


#endif
