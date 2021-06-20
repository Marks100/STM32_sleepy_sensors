#ifndef HAL_CONFIG_H
#define HAL_CONFIG_H

/***************************************************************************************************
**                              Includes                                                          **
***************************************************************************************************/
#include "C_defs.h"
#include "COMPILER_defs.h"

#define HW_WAKEUP_PORT      GPIOA
#define HW_WAKEUP_PIN       GPIO_Pin_0
#define HW_WAKEUP_PORT_SRC  GPIO_PortSourceGPIOA
#define HW_WAKEUP_PIN_SRC   GPIO_PinSource0
#define HW_WAKEUP_EXT_LINE  EXTI_Line0

#define USART2_TX_PORT      GPIOA
#define USART2_TX_PIN       GPIO_Pin_2

#define USART2_RX_PORT      GPIOA
#define USART2_RX_PIN       GPIO_Pin_3
 
#define SPI1_PORT           GPIOA      
#define SPI1_SCK_PIN        GPIO_Pin_5    
#define SPI1_MISO_PIN       GPIO_Pin_6  
#define SPI1_MOSI_PIN       GPIO_Pin_7

#define DEBUG_MODE_LED_PORT GPIOA
#define DEBUG_MODE_LED_PIN  GPIO_Pin_8

#define NRF_CS_PORT         GPIOA
#define NRF_CS_PIN          GPIO_Pin_9

#define NRF_CE_PORT         GPIOA
#define NRF_CE_PIN          GPIO_Pin_10

#define NRF_PWR_EN_PORT     GPIOA
#define NRF_PWR_EN_PIN      GPIO_Pin_11

#define NRF_IRQ_PORT        GPIOA
#define NRF_IRQ_PIN         GPIO_Pin_12

#define BL_REQUEST_PORT     GPIOB
#define BL_REQUEST_PIN      GPIO_Pin_4

#define DEBUG_SEL_PORT      GPIOB
#define DEBUG_SEL_PIN       GPIO_Pin_5
 
#define I2C1_PORT           GPIOB   
#define I2C1_SDA_PIN        GPIO_Pin_6  
#define I2C1_SCL_PIN        GPIO_Pin_7 

#define BMP280_PWR_EN_PORT  GPIOB
#define BMP280_PWR_EN_PIN   GPIO_Pin_9
 

#define PANEL_1_BTN_PORT
#define PANEL_1_BTN_PIN 

#define PANEL_2_BTN_PORT
#define PANEL_2_BTN_PIN 

#define PANEL_3_BTN_PORT
#define PANEL_3_BTN_PIN 

#define PWR_EN_PORT        GPIOB
#define PWR_EN_PIN         GPIO_Pin_11

#define NRF24_CE_PORT      GPIOB
#define NRF24_CE_PIN       GPIO_Pin_12

#define ONBOARD_LED_PORT   GPIOC
#define ONBOARD_LED_PIN    GPIO_Pin_13

#endif /* HAL_CONFIG_H multiple inclusion guard */
