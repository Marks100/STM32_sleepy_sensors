#include "stm32f10x_adc.h"
#include "stm32f10x_rcc.h"
#include "COMPILER_defs.h"
#include "C_defs.h"
#include "STDC.h"
#include "HAL_ADC.h"

u16_t adc_result_s;



void HAL_ADC_init( void )
{
	ADC_InitTypeDef ADC_InitStructure;

	/* Enable the ADC clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE_);

	RCC_ADCCLKConfig(RCC_PCLK2_Div4);

	ADC_DeInit(ADC1);

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init ( ADC1, &ADC_InitStructure);	//set config of ADC1

	ADC_Cmd(ADC1, ENABLE);

	/* Enable ADC1 reset calibaration register */
	ADC_ResetCalibration(ADC1);
	/* Check the end of ADC1 reset calibration register */
	while(ADC_GetResetCalibrationStatus(ADC1));
	/* Start ADC1 calibaration */
	ADC_StartCalibration(ADC1);
	/* Check the end of ADC1 calibration */
	while(ADC_GetCalibrationStatus(ADC1));
}



void HAL_ADC_de_init( void )
{
    /* Reset the ADC module  */
	ADC_DeInit( ADC1 );

	/* Disable ADC system clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, DISABLE);
}




u16_t HAL_ADC_measure_vref_internal( void )
{
    u16_t result = 0;

    // Enable Temperature sensor
    ADC_TempSensorVrefintCmd(ENABLE);

    ADC_RegularChannelConfig(ADC1, ADC_Channel_Vrefint, 1, ADC_SampleTime_239Cycles5);

    /* Start ADC1 Software Conversion */
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);

    /* wait for conversion complete */
    while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);

    /* read ADC value */
    result = ADC_GetConversionValue(ADC1);

    /* clear EOC flag */
    ADC_ClearFlag(ADC1, ADC_FLAG_EOC);

    ADC_TempSensorVrefintCmd(DISABLE);

    return (result);
}



u16_t HAL_ADC_measure_NTC_temp_raw( void )
{
    u16_t result = 0;

    ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 1, ADC_SampleTime_239Cycles5);

    /* Start ADC1 Software Conversion */
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);

    /* wait for conversion complete */
    while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);

    /* read ADC value */
    result = ADC_GetConversionValue(ADC1);

    /* clear EOC flag */
    ADC_ClearFlag(ADC1, ADC_FLAG_EOC);

    return (result);
}


u16_t HAL_ADC_measure_batt_voltage( void )
{
	u16_t reference;
    u32_t result;

    reference = HAL_ADC_measure_vref_internal();

    result = ( ( 1200.0 * 4096.0 )/(float)reference );

    return ((u16_t)result);
}


