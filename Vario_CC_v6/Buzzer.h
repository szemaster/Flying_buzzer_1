#ifndef BUZZER_H_
#define BUZZER_H_

#include "Vario_CC_board.h"
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_tim.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_misc.h"                                  //it contains the nvic corresponding functions

#define TIM_BUZZTICK_FREQ                        10000       //10kHz
#define TIM_BUZZOVERFLOW_FREQ_HIGH               2500
#define TIM_BUZZOVERFLOW_FREQ_LOW                700

#define TIM_PULSETICK_FREQ                       2000        //Sets the prescaler value, prescaler=(timclock - TIM_PULSETICK_FREQ) / TIM_PULSETICK_FREQ.
                                                             //The actual frequency is TIM_PULSETICK_FREQ/TIM_PULSEPERIOD
                                                             //Note: prescaler uses 16 bits
#define TIM_PULSEPERIOD                          2           //The counter will start over if it reaches this value
#define TIM_PULSECHANNELCOMPARE                  70          //the channel will be on (or actually off) \ 
                                                             //(TIM_PULSECHANNELCOMPARE*(TIM_PULSETICK_FREQ/TIM_PULSEPERIOD) sec long
volatile uint32_t intbuzzcounter;
volatile uint8_t intbuzzenable;
volatile uint16_t intbuzzthreshold;

void BuzzerGPIOInit();
void BuzzerTimerInit(uint32_t timclock, uint16_t freq);
void BuzzerChannelInit(uint16_t freq);
void BuzzerInit(uint16_t freq);
void BuzzerEnable(FunctionalState NewState);
void BuzzerTimerandChannelReInit(uint16_t freq);

void PulseTimerInit(uint32_t timclock);
void PulseChannelInit();
void PulseTimerEnableInterrupts(FunctionalState NewState);
void TIM1_BRK_UP_TRG_COM_IRQHandler();
void TIM1_CC_IRQHandler();
void PulseInit(uint16_t pfreq);
void PulseSetFreq(uint16_t pfreq);

#endif /* BUZZER_H_ */