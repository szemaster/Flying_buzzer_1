#ifndef VARIO_CC_BOARD_H_
#define VARIO_CC_BOARD_H_

#include "stm32f0xx_rcc.h"
#include "stm32f0xx_tim.h"
#include "stm32f0xx_misc.h"                                  //it contains the nvic corresponding functions
#include "stm32f0xx_pwr.h"

//common
#define GPIO_SPEED_USED                   GPIO_Speed_2MHz

//usart
#define USART_USED                        USART1
#define USART_GPIO_TX                     GPIOA
#define USART_GPIO_RX                     GPIOA
#define USART_PIN_TX                      GPIO_Pin_2
#define USART_PIN_RX                      GPIO_Pin_3
#define USART_AF_TX                       GPIO_AF_1
#define USART_AF_RX                       GPIO_AF_1
#define USART_PINSOURCE_TX                GPIO_PinSource2
#define USART_PINSOURCE_RX                GPIO_PinSource3
#define USART_USART_RCC_CMD(NEWSTATE)     RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, NEWSTATE)
#define USART_GPIO_RCC_CMD(NEWSTATE)      RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, NEWSTATE)

//power LED (PLED)
#define PLED_GPIO                         GPIOA
#define PLED_PIN                          GPIO_Pin_4
#define PLED_RCC_GPIO_CMD(NEWSTATE)       RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, NEWSTATE)
//power ADC that checks the supply voltage
#define PADC_TIM                          TIM16
#define PADC_RCC_TIM_CMD(NEWSTATE)        RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM16, NEWSTATE)
#define PADC_ADC                          ADC1
#define PADC_RCC_ADC_CMD(NEWSTATE)        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, NEWSTATE)
#define PADC_ADC_CHANNEL                  ADC_Channel_17

//buzzer
#define BUZZ_GPIO_POS                     GPIOA
#define BUZZ_GPIO_NEG                     GPIOA
#define BUZZ_PIN_POS                      GPIO_Pin_6
#define BUZZ_PIN_NEG                      GPIO_Pin_7
#define BUZZ_PINSOURCE_POS                GPIO_PinSource6
#define BUZZ_PINSOURCE_NEG                GPIO_PinSource7
#define BUZZ_AF_POS                       GPIO_AF_1
#define BUZZ_AF_NEG                       GPIO_AF_1
#define BUZZ_TIM_POS                      TIM3
#define BUZZ_TIM_NEG                      TIM3
#define BUZZ_TIMCH_POS                    1
#define BUZZ_TIMCH_NEG                    2
#define BUZZ_RCC_TIM_CMD(NEWSTATE)        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, NEWSTATE)
#define BUZZ_RCC_GPIO_CMD(NEWSTATE)       RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, NEWSTATE)
#define BUZZ_TIM_OC_POSINIT(STRUCT)       TIM_OC1Init(BUZZ_TIM_POS, STRUCT)
#define BUZZ_TIM_OC_NEGINIT(STRUCT)       TIM_OC2Init(BUZZ_TIM_NEG, STRUCT)
#define BUZZ_TIM_OC_POSPRECONF(NEWSTATE)  TIM_OC1PreloadConfig(BUZZ_TIM_POS, NEWSTATE)
#define BUZZ_TIM_OC_NEGPRECONF(NEWSTATE)  TIM_OC2PreloadConfig(BUZZ_TIM_NEG, NEWSTATE)
//pulse generator for the buzzer (as well in Buzzer.h and Buzzer.c)
#define PULSE_TIM                         TIM1
#define PULSE_RCC_TIM_CMD(NEWSTATE)       RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, NEWSTATE)
#define PULSE_TIM_OC_INIT(STRUCT)         TIM_OC1Init(PULSE_TIM, STRUCT)
#define PULSE_TIM_OC_PRECONF(NEWSTATE)    TIM_OC1PreloadConfig(PULSE_TIM, NEWSTATE)
#define PULSE_TIM_CC_USED                 TIM_IT_CC1

//I2C
#define I2C_GPIO_SCL                      GPIOA
#define I2C_GPIO_SDA                      GPIOA
#define I2C_PIN_SCL                       GPIO_Pin_9
#define I2C_PIN_SDA                       GPIO_Pin_10
#define I2C_RCC_GPIO_CMD(NEWSTATE)        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, NEWSTATE)
#define I2C_AF_SCL                        GPIO_AF_4
#define I2C_AF_SDA                        GPIO_AF_4
#define I2C_PINSOURCE_SCL                 GPIO_PinSource9
#define I2C_PINSOURCE_SDA                 GPIO_PinSource10
#define I2C_RCC_I2C_CMD(NEWSTATE)         RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, NEWSTATE)
#define I2C_TIMINGSETUP                   0x00201D7B   // calculated by the app of stm in the application note AN4235
                                                       // must be recalculated every time you change the frequency of the clock source
#define I2C_I2C_USED                      I2C1

//Delay Timer
#define DELAY_TIM                         TIM14
#define DELAY_RCC_TIM_CMD(NEWSTATE)       RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, NEWSTATE)
#define DELAY_TIM_TICK_FREQ               100000
#define DELAY_FREQ                        1000         // Delay time is 1/DELAY_FREQ if you call the function VarioCC_Delay(1)
#define DELAY_IRQN                        TIM14_IRQn

volatile uint32_t intcounter;
volatile uint8_t doesitsleep;             //doesitsleep = 1 if it was sleeping when the interrupt woke it up.

uint32_t VarioCC_GetTimBusClockFreq();
void VarioCC_DelayEnableInterrupts();
void VarioCC_DelayTIMInit();
void VarioCC_Delay(uint16_t delay);
void VarioCC_NonBlockingDelay_Start(uint16_t delay);



#endif /* VARIO_CC_BOARD_H_ */