#ifndef UNIT_TEST
    #include "stm32f10x_adc.h"
    #include "stm32f10x_rcc.h"
#endif

#include "COMPILER_defs.h"
#include "C_defs.h"
#include "STDC.h"
#include "HAL_ADC.h"


void HAL_ADC_init( void )
{
	ADC_InitTypeDef ADC_InitStructure;

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
}




u16_t HAL_ADC_measure_joystick_x_axis( void )
{
    u16_t result = 0;

    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_1Cycles5);

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



u16_t HAL_ADC_measure_joystick_y_axis( void )
{
    u16_t result = 0;

    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_1Cycles5);

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

