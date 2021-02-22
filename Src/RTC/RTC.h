#ifndef RTC_H
#define RTC_H


/***************************************************************************************************
**                              Includes                                                          **
***************************************************************************************************/
#include "C_defs.h"
#include "COMPILER_defs.h"

/***************************************************************************************************
**                              Defines                                                           **
***************************************************************************************************/
#define RTC_EXT_I2C_ADDRESS				0x51	/* This value gets Left Shifted in the driver */
#define RTC_EXT_MAX_NUM_REGS			16u
#define RTC_TIME_ARRAY_SIZE				7u

#define RTC_EXT_TIMER_INT_ENABLE_BIT (1<<0)
#define RTC_EXT_ALARM_INT_ENABLE_BIT (1<<1)
#define RTC_EXT_TIMER_INT_ACTIVE_BIT (1<<2)
#define RTC_EXT_ALARM_INT_ACTIVE_BIT (1<<3)

#define RTC_EXT_ALARM_1HZ_BIT        (1<<1)
#define RTC_EXT_ALARM_1_OVER60HZ_BIT (1<<0)
#define RTC_EXT_ALARM_1_OVER60HZ_VAL (60u)

#define RTC_SECONDS_PER_MIN          (60u)
#define RTC_SECONDS_PER_HOUR         (3600u)
#define RTC_SECONDS_PER_DAY          (86400)
#define RTC_SECONDS_PER_YEAR         (31536000u)

#define RTC_DEFAULT_CTRL_STATUS1_REG_VAL (0x80u)
#define RTC_DEFAULT_CTRL_STATUS2_REG_VAL (0x00)


/***************************************************************************************************
**                              Constants                                                         **
***************************************************************************************************/
/* None */


/***************************************************************************************************
**                              Data Types and Enums                                              **
***************************************************************************************************/
typedef enum
{
	Control_status_1 = 0u,
	Control_status_2,
	VL_seconds,
	Minutes,
	Hours,
	Days,
	Weekdays,
	Century_months,
	Years,
	Minute_alarm,
	Hour_alarm,
	Day_alarm,
	Weekday_alarm,
	CLKOUT_control,
	Timer_control,
	Timer
} RTC_EXT_registers_et;



typedef enum
{
	ClK_OUT_32768_HZ = 0u,
	ClK_OUT_1024_HZ,
	ClK_OUT_32_HZ,
	ClK_OUT_1_HZ,
} RTC_clk_out_setting_et;

typedef struct
{
	u8_t seconds;
	u8_t minutes;
	u8_t hours;
	u8_t days;
	u8_t years;
} RTC_time_st;





/***************************************************************************************************
**                              Exported Globals                                                  **
***************************************************************************************************/
/* None */



/***************************************************************************************************
**                              Function Prototypes                                               **
***************************************************************************************************/
void	      RTC_init( void );
void 		  RTC_setup_default_config( void );
void          RTC_set_wakeup_time( u32_t seconds );
void		  RTC_update_current_rtc_time( void );
u32_t         RTC_get_current_running_time_secs( void );
pass_fail_et  RTC_get_failure_status( void );
u8_t          RTC_bcd_to_int( u8_t input );
void          RTC_reset_date_and_time( void );
void 		  RTC_alarm_triggered( void );
void 		  RTC_clear_alarm( void );
false_true_et RTC_get_alarm_status( void );

#endif /* RTC_H multiple inclusion guard */

/****************************** END OF FILE *******************************************************/
