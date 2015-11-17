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

	//LED
	gpioinitstruct.GPIO_Pin = PLED_PIN;
	gpioinitstruct.GPIO_Mode = GPIO_Mode_OUT;
	gpioinitstruct.GPIO_OType = GPIO_OType_PP;
	gpioinitstruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	gpioinitstruct.GPIO_Speed = GPIO_SPEED_USED;
	GPIO_Init(PLED_GPIO, &gpioinitstruct);

	//ADC
	gpioinitstruct.GPIO_Pin = PADC_PIN;
	gpioinitstruct.GPIO_Mode = GPIO_Mode_AN;
	gpioinitstruct.GPIO_OType = GPIO_OType_PP;
	gpioinitstruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	gpioinitstruct.GPIO_Speed = GPIO_SPEED_USED;
	GPIO_Init(PADC_GPIO, &gpioinitstruct);
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
	adcinitstruct.ADC_ScanDirection = ADC_ScanDirection_Upward;
	ADC_Init(PADC_ADC, &adcinitstruct);
	ADC_AutoPowerOffCmd(PADC_ADC, ENABLE);
	ADC_ChannelConfig(PADC_ADC, ADC_Channel_1, PADC_ADC_SAMPLETIME);
	ADC_GetCalibrationFactor(PADC_ADC);
	ADC_Cmd(PADC_ADC, ENABLE);

	PADC_RCC_ADC_CMD(DISABLE);
}

//starts a conversion and returns with the result of the conversion
//output: result of the conversion
uint16_t PowerLed_ADCGetRefVoltValue(){
	ADC_StartOfConversion(PADC_ADC);     //it doesn't checks the ADRDY flag, because the flag is not set in AutoPowerOff mode

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
	return (PADC_LOWVOLTAGE_LIMIT <= voltage);
}

//initializes the ADC that will convert the value of Vrefint
//initializes the GPIO pin of the led
//initializes the counter that will schedule the conversions
void PowerLed_Init(){
	PowerLed_ADCADCInit();
	PowerLed_GPIOInit();
	adcconvcounter = 0;
}

//checks if it is time to convert the value of Vrefint
//regularly recalibrates the ADC (frequency is defined in PowerLed.h)
//converts the value of Vrefint
//according to the result of conversion it sets/resets the led
void PowerLed_Main(){
	static uint8_t i = 0;
	uint16_t voltage;
	if (adcconvcounter <= 0){

		adcconvcounter = PADC_TIMERTURNS;
		PADC_RCC_ADC_CMD(ENABLE);
		if (!(i++ & PADC_RECALIBFREQ)){
			ADC_GetCalibrationFactor(PADC_ADC);
		}

		voltage = PowerLed_ADCGetRefVoltValue();
		if (!PowerLed_IsVoltageOK(voltage)){
			GPIO_SetBits(PLED_GPIO, PLED_PIN);
			if( voltage < PADC_SHUTDOWN_VOLTAGE){
				SHUTDOWN;
			}
		}
		else{
			GPIO_ResetBits(PLED_GPIO, PLED_PIN);
		}

		PADC_RCC_ADC_CMD(DISABLE);
	}
}



