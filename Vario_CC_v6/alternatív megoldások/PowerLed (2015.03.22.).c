/*
* PowerLed.c
*
*  Created on: 2015.03.16.
*      Author: Cusi
*/


#include "PowerLed.h"


//Initializes GPIOs for the Power Led
//  - enables periphclock for GPIOs that are used
//  - sets GPIOs as alternate function pins
void PowerLed_GPIOInit(){
	GPIO_InitTypeDef gpioinitstruct;

	PLED_RCC_GPIO_CMD(ENABLE);

	gpioinitstruct.GPIO_Pin = PLED_PIN;
	gpioinitstruct.GPIO_Mode = GPIO_Mode_AF;
	gpioinitstruct.GPIO_OType = GPIO_OType_PP;
	gpioinitstruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	gpioinitstruct.GPIO_Speed = GPIO_SPEED_USED;
	GPIO_Init(PLED_GPIO, &gpioinitstruct);
	GPIO_PinAFConfig(PLED_GPIO, PLED_PINSOURCE, PLED_AF_TIM);
}

//Initializes the timer for the the Power Led
//  - enables periphclock for timer that is used
//  - initializes timer, prescaler and period values are computed
//        according to the values defined in PowerLed.h and to the value of PCLK
//input: the frequency of the bus that drives the driver
void PowerLed_LedTimerInit(uint32_t timclock){
	TIM_TimeBaseInitTypeDef timerinitstruct;

	PLED_RCC_TIM_CMD(ENABLE);

	TIM_TimeBaseStructInit(&timerinitstruct);
	timerinitstruct.TIM_Prescaler = (timclock / PLED_TIM_TICK_FREQ) - 1;
	timerinitstruct.TIM_Period = PLED_TIM_TICK_FREQ / PLED_TIM_OVERFLOW_FREQ - 1;
	TIM_TimeBaseInit(PLED_TIM, &timerinitstruct);
}


//initializes timer Channel for the Power Led
void PowerLed_ChannelInit(){
	TIM_OCInitTypeDef ocinitstruct;
	uint32_t pulse;

	TIM_OCStructInit(&ocinitstruct);
	ocinitstruct.TIM_OCMode = TIM_OCMode_PWM1;
	ocinitstruct.TIM_OCPolarity = TIM_OCPolarity_High;
	ocinitstruct.TIM_OutputState = TIM_OutputState_Enable;
	pulse = ((PLED_TIM_TICK_FREQ / PLED_TIM_OVERFLOW_FREQ - 1)*PLED_TIM_DUTY) / 100;
	ocinitstruct.TIM_Pulse = pulse;
	PLED_TIM_OC_INIT(&ocinitstruct);
	PLED_TIM_OC_PRECONF(TIM_OCPreload_Enable);
}


//initializes the Power Led
//  - enables periphclock for timer that is used
//  - enables periphclock for GPIOs that are used
//  - sets GPIOs as alternate function pins
//  - initializes timer, prescaler and period values are computed
//        according to the values defined in PowerLed.h and to the value of PCLK
//  - initializes timer Channel for Power Led
//  - IT DOES NOT ENABLES THE USED TIMER
void PowerLed_LedInit(){
	uint32_t timclock;

	PowerLed_GPIOInit();
	timclock = VarioCC_GetTimBusClockFreq();
	PowerLed_LedTimerInit(timclock);
	PowerLed_ChannelInit();
}

void PowerLed_LedEnable(FunctionalState NewState){
	TIM_Cmd(PLED_TIM, NewState);
	if (NewState == DISABLE){
		GPIO_ResetBits(PLED_GPIO, PLED_PIN);
	}
}

//initializes the timer for the ADC. This timer is to schedule the conversions of the ADC
//  - enables periphclock for timer that is used
//  - initializes timer, prescaler and period values are computed
//        according to the values defined in PowerLed.h and to the value of PCLK
//  - enables TIM_IT_Interrupt, but not enables NVIC
void PowerLed_ADCTimerInitandStart(){
	TIM_TimeBaseInitTypeDef timerinitstruct;
	uint32_t timclock;

	PADC_RCC_TIM_CMD(ENABLE);

	TIM_TimeBaseStructInit(&timerinitstruct);
	timerinitstruct.TIM_Prescaler = PADC_TIM_PRESCALER;
	timclock = VarioCC_GetTimBusClockFreq();
	timerinitstruct.TIM_Period = (timclock / (PADC_TIM_PRESCALER + 1))*PADC_SECBETWEENCONVERSIONS;
	TIM_TimeBaseInit(PADC_TIM, &timerinitstruct);
	TIM_Cmd(PADC_TIM, ENABLE);
	TIM_ITConfig(PADC_TIM, TIM_IT_Update, ENABLE);
}

//initializes and calibrates the ADC
//  - enables periphclock for ADC that is used
//  - configurates ADC and the channel that is used for measuring Vrefint
//  - enables AutoPowerOff
//  - calibrates ADC
//  - enables ADC
//note: does not preserves calibration factor
void PowerLed_ADCADCInit(){
	ADC_InitTypeDef adcinitstruct;


	PADC_RCC_ADC_CMD(ENABLE);
	ADC_StructInit(&adcinitstruct);
	adcinitstruct.ADC_Resolution = ADC_Resolution_10b;
	adcinitstruct.ADC_ScanDirection = ADC_ScanDirection_Backward;
	ADC_Init(PADC_ADC, &adcinitstruct);
	ADC_AutoPowerOffCmd(PADC_ADC, ENABLE);
	ADC_VrefintCmd(ENABLE);
	ADC_ChannelConfig(PADC_ADC, PADC_ADC_CHANNEL, PADC_ADC_SAMPLETIME);
	ADC_GetCalibrationFactor(PADC_ADC);
	ADC_Cmd(PADC_ADC, ENABLE);
}

//starts a conversion and returns with the result of the conversion
//output: result of the conversion
uint16_t PowerLed_ADCGetRefVoltValue(){
	ADC_StartOfConversion(PADC_ADC);

	while (!ADC_GetFlagStatus(PADC_ADC, ADC_FLAG_EOC))
	{
		asm("nop");
	}
	return ADC_GetConversionValue(PADC_ADC);
}

//checks if the measured voltage is higher than the minimal that baleongs to the minimal supplyvoltage
//output: 1: Voltage is OK
//        2: Voltage is not OK
//input: voltage: the result of the conversion measured on Vrefint
uint8_t PowerLed_IsVoltageOK(uint16_t voltage){
	return (PADC_LOWVOLTAGE_LIMIT >= voltage);
}



