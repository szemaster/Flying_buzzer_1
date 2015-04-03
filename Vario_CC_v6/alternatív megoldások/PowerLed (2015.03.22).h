/*
* PowerLed.h
*
*  Created on: 2015.03.16.
*      Author: Cusi
*/

#ifndef POWERLED_H_
#define POWERLED_H_

#include "Vario_CC_board.h"
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_tim.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_adc.h"

#define PLED_TIM_TICK_FREQ          100000  //100kHz
#define PLED_TIM_OVERFLOW_FREQ      2000
#define PLED_TIM_DUTY               80      //determines the duty circle in percentage
#define PADC_SECBETWEENCONVERSIONS  1      //the time between two consecutive conversion in second
#define PADC_TIM_PRESCALER          60000  //this is the maximum value of the prescaler
#define PADC_ADC_SAMPLETIME         ADC_SampleTime_41_5Cycles
#define PADC_LOWVOLTAGE_LIMIT       534    //PADC_LOWVOLTAGE_LIMIT = 1,2*2^Res/Ulimit, 
//    where Ulimit is the critical voltage in V
//    Res is the resolution in bits (For example 10)

void PowerLed_GPIOInit();
void PowerLed_LedTimerInit(uint32_t timclock);
void PowerLed_ChannelInit();
void PowerLed_LedInit();
void PowerLed_LedEnable(FunctionalState NewState);

void PowerLed_ADCTimerInitandStart();
void PowerLed_ADCADCInit();
uint16_t PowerLed_ADCGetRefVoltValue();
uint8_t PowerLed_IsVoltageOK(uint16_t voltage);

#endif /* POWERLED_H_ */
