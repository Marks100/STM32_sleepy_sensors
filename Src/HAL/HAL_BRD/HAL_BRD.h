#ifndef HAL_BRD_H
#define HAL_BRD_H



/***************************************************************************************************
**                              Includes                                                          **
***************************************************************************************************/
#include "C_defs.h"
#include "COMPILER_defs.h"

#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"


/***************************************************************************************************
**                              Defines                                                           **
***************************************************************************************************/
/* None */



/***************************************************************************************************
**                              Constants                                                         **
***************************************************************************************************/
/* None */


/***************************************************************************************************
**                              Data Types and Enums                                              **
***************************************************************************************************/
/* None */



/***************************************************************************************************
**                              Exported Globals                                                  **
***************************************************************************************************/
/* None */



/***************************************************************************************************
**                              Function Prototypes                                               **
***************************************************************************************************/

void HAL_BRD_init( void );
low_high_et HAL_BRD_Read_Pin_state( GPIO_TypeDef * port, u16_t pin );
void HAL_BRD_set_rotary_interrupt_state( disable_enable_et state);
void HAL_BRD_Set_Pin_state(  GPIO_TypeDef * port, u16_t pin, low_high_et state );
void HAL_BRD_debounce_completed( void );
void HAL_BRD_set_vreg_enable_pin( low_high_et state );
void HAL_BRD_set_weld_pin( low_high_et state );
void HAL_BRD_Toggle_heartbeat_pin(  void );
void HAL_BRD_set_pwmPin_x_axis_state( low_high_et state );
void HAL_BRD_set_pwmPin_y_axis_state( low_high_et state );
void HAL_BRD_set_laser_Pin_state( off_on_et state );
low_high_et HAL_BRD_read_switch_input_laser( void );
low_high_et HAL_BRD_read_switch_input_rotary_encoder( void );
low_high_et HAL_BRD_read_switch_input_power( void );



#endif
