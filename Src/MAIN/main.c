/* STM32 specific includes */
#include "misc.h"
#include "stm32f10x_pwr.h"
#include "stm32f10x_bkp.h"

#include "C_defs.h"
#include "PROJ_config.h"
#include "HAL_BRD.h"
#include "HAL_ADC.h"
#include "HAL_SPI.h"
#include "HAL_I2C.h"
#include "HAL_UART.h"
#include "RTC.h"
#include "NRF24.h"
#include "BMP280.h"
#include "NVM.h"
#include "MODE_MGR.h"
#include "CLI_MGR.h"
#include "SYSTICK_MGR.h"
#include "RNG_MGR.h"
#include "RF_MGR.h"
#include "DBG_MGR.h"
#include "SEN_MGR.h"
#include "VER.h"
#include "MAIN.h"

int main(void)
{
	/* Before we do anything else,
	Check if we need to transition to the bootloader */
	Bootloader_check();

	RCC_ClocksTypeDef RCC_Clocks;
	
	RCC_GetClocksFreq (&RCC_Clocks);
	RCC_HCLKConfig(RCC_SYSCLK_Div1);
	RCC_PCLK1Config(RCC_HCLK_Div1);
	RCC_PCLK2Config(RCC_HCLK_Div1);

	/* Important that these modules get initialised early */
	DBG_MGR_init();
	SYSTICK_MGR_init();
	MODE_MGR_init();

	/* Init the HW */ 
	HAL_BRD_init();

	HAL_I2C1_init();
	HAL_SPI1_init();
	HAL_ADC_init();
	NVM_init();

	/* Init the rest of the Modules */
	RTC_init();
	SEN_MGR_init();	
	RNG_MGR_init();
	RF_MGR_init();
	BMP280_init();
	VER_init();
	
	if( MODE_MGR_get_operating_mode() == MODE_MGR_DEBUG_MODE )
	{
		/* In debug mode lets init the debug uart */
		HAL_USART2_init();
		CLI_MGR_init();
		CLI_MGR_display_startup_message();
	}

	/* Write the default config down to the RTC */
	RTC_set_configuration( RTC_EXT_SST_CONFIG );

	while (1)
	{
		MODE_MGR_tick(); 
	}
}






void JumpToBootloader(void) 
{
	void (*SysMemBootJump)(void);

	volatile uint32_t addr = 0x1FFFF000;
	
	/* Step: Disable RCC, set it to default (after reset) settings Internal clock, no PLL, etc */

	RCC_DeInit();
	
	/* Step: Disable systick timer and reset it to default values */
	SysTick->CTRL = 0;
	SysTick->LOAD = 0;
	SysTick->VAL = 0;

	/* Step: Disable all interrupts */
	__disable_irq();

	//SCB->VTOR = 0;
	
	/* Step: Set jump memory location for system memory
	Use address with 4 bytes offset which specifies jump location where program starts */
	SysMemBootJump = (void (*)(void)) (*((uint32_t *)(addr + 4)));
	
	/* Step: Set main stack pointer.
	This step must be done last otherwise local variables in this function
	don't have proper value since stack pointer is located on different position
	 
	 Set direct address location which specifies stack pointer in SRAM location */
	__set_MSP(*(uint32_t *)addr);
	
	/* Step: Actually call our function to jump to set location
	This will start system memory execution */
	SysMemBootJump();
}


void Bootloader_check( void )
{
	/* Check if we have written to the BACKUP register which would 
	signify that we want to jump to the bootloader */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	
	PWR_BackupAccessCmd(ENABLE);

	if( BKP_ReadBackupRegister( BOOTLOADER_BACKUP_REG ) == BOOTLOADER_REQUESTED )
	{
		BKP_WriteBackupRegister( BOOTLOADER_BACKUP_REG, BOOTLOADER_NOT_REQUESTED );
		JumpToBootloader();
	}
}
