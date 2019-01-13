#ifndef PROJ_CONFIG_H
#define PROJ_CONFIG_H

#define STM32 							 1
#define RFM69W
#define RFM69_DEFAULT_TX_POWER_LEVEL     0
#define RTC_EXT_DEFAULT_WAKEUP_TIME_SEC  4

#define AUTO_DEBUG_MODE					 0
#define MULTI_SPI_WRITE_CONFIG			 1
#define SENSOR_ID 						 1
#define SENSOR_TYPE						 2


#if !defined ( RFM69HW ) && !defined ( RFM69W ) && !defined ( RFM69HW_SUPER )
	#error "Please define one of the following RFM69W || RFM69HW || RFM69HW_SUPER"
#endif


#endif /* PROJ_CONFIG_H multiple inclusion guard */
