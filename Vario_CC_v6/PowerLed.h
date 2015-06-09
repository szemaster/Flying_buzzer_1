/*
* PowerLed.h
*
*  Created on: 2015.03.16.
*      Author: Cusi
*/

//Note: This application hasn't got additional timer for scheduling ADC converion, 
//      it uses the timer of the buzzer. This way we can spare with consumption.

#ifndef POWERLED_H_
#define POWERLED_H_



#include "Vario_CC_board.h"
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_tim.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_adc.h"


#define PADC_SECBETWEENCONVERSIONS  1      //the time between two consecutive conversion in second
#define PADC_TIM_PRESCALER          60000  //this is the maximum value of the prescaler
#define PADC_ADC_SAMPLETIME         ADC_SampleTime_41_5Cycles
#define PADC_LOWVOLTAGE_LIMIT       534    //PADC_LOWVOLTAGE_LIMIT = 1,2*2^Res/Ulimit, 
                                           //    where Ulimit is the critical voltage in V
                                           //    Res is the resolution in bits (For example 10)
#define PADC_RECALIBFREQ            0x3    //Frequency of recalibration (MUST BE (POWER OF TWO)-1, 2^n-1)

#define PADC_TIMERTURNS             1000   //A timer is set and executes an interrupt routin in every x ms (right now x=10ms)
                                           //The value of adcconvcounter is dectremented in each circle
                                           //If its value is 0 a conversion will be executed and its initial value will be restored
                                           //Note: for this application there is no additional timer it uses the timer of the buzzer
                                           //       This way we can spare with consumption.
volatile int32_t adcconvcounter;

void PowerLed_GPIOInit();
void PowerLed_ADCTimerInitandStart();
void PowerLed_ADCADCInit();
uint16_t PowerLed_ADCGetRefVoltValue();
uint8_t PowerLed_IsVoltageOK(uint16_t voltage);

void PowerLed_Init();
void PowerLed_Main();

#endif /* POWERLED_H_ */
