/*
* Buzzer.c
*
*  Created on: 2015.03.16.
*      Author: Cusi
*/

#include "Buzzer.h"

//Initializes GPIOs for the buzzer
//  - enables periphclock for GPIOs that are used
//  - sets GPIOs as alternate function pins
void BuzzerGPIOInit(){
	GPIO_InitTypeDef gpioinitstruct;

	BUZZ_RCC_GPIO_CMD(ENABLE);

	gpioinitstruct.GPIO_Pin = BUZZ_PIN_NEG;
	gpioinitstruct.GPIO_Mode = GPIO_Mode_AF;
	gpioinitstruct.GPIO_OType = GPIO_OType_PP;
	gpioinitstruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	gpioinitstruct.GPIO_Speed = GPIO_SPEED_USED;
	GPIO_Init(BUZZ_GPIO_NEG, &gpioinitstruct);
	gpioinitstruct.GPIO_Pin = BUZZ_PIN_POS;
	GPIO_Init(BUZZ_GPIO_POS, &gpioinitstruct);
	GPIO_PinAFConfig(BUZZ_GPIO_NEG, BUZZ_PINSOURCE_NEG, BUZZ_AF_NEG);
	GPIO_PinAFConfig(BUZZ_GPIO_POS, BUZZ_PINSOURCE_POS, BUZZ_AF_POS);
}

//Initializes the timer for the buzzer
//  - enables periphclock for timer that is used
//  - initializes timer, prescaler and period values are computed
//        according to the values defined in Buzzer.h and to the value of PCLK
//input: the frequency of the bus that drives the driver
void BuzzerTimerInit(uint32_t timclock, uint16_t freq){
	TIM_TimeBaseInitTypeDef timerinitstruct;

	BUZZ_RCC_TIM_CMD(ENABLE);

	TIM_TimeBaseStructInit(&timerinitstruct);
	timerinitstruct.TIM_Prescaler = (timclock / TIM_BUZZTICK_FREQ) - 1;
	timerinitstruct.TIM_Period = TIM_BUZZTICK_FREQ / freq - 1;
	TIM_TimeBaseInit(BUZZ_TIM_POS, &timerinitstruct);
	TIM_TimeBaseInit(BUZZ_TIM_NEG, &timerinitstruct);
}

//initializes timer Channels for BUZZER Poz and Buzzer Neg
void BuzzerChannelInit(uint16_t freq){
	TIM_OCInitTypeDef ocinitstruct;

	TIM_OCStructInit(&ocinitstruct);
	ocinitstruct.TIM_OCMode = TIM_OCMode_Toggle;
	ocinitstruct.TIM_OCPolarity = TIM_OCPolarity_High;
	ocinitstruct.TIM_OutputState = TIM_OutputState_Enable;
	ocinitstruct.TIM_Pulse = (TIM_BUZZTICK_FREQ / freq - 1) >> 1;     //50% duty circle
	BUZZ_TIM_OC_POSINIT(&ocinitstruct);
	ocinitstruct.TIM_OCPolarity = TIM_OCPolarity_Low;
	BUZZ_TIM_OC_NEGINIT(&ocinitstruct);
	BUZZ_TIM_OC_POSPRECONF(TIM_OCPreload_Enable);
	BUZZ_TIM_OC_NEGPRECONF(TIM_OCPreload_Enable);
}

//initializes BUZZER
//  - enables periphclock for timer that is used
//  - enables periphclock for GPIOs that are used
//  - sets GPIOs as alternate function pins
//  - initializes timer, prescaler and period values are computed
//        according to the values defined in Buzzer.h and to the value of PCLK
//  - initializes timer Channels for BUZZER Poz and Buzzer Neg
//  - IT DOES NOT ENABLES THE USED TIMER
void BuzzerInit(uint16_t freq){
	uint32_t timclock;
	BuzzerGPIOInit();
	timclock = VarioCC_GetTimBusClockFreq();
	BuzzerTimerInit(timclock, freq);
	BuzzerChannelInit(freq);
}

//Enables the timer of the Buzzer
void BuzzerEnable(FunctionalState NewState){
	TIM_Cmd(BUZZ_TIM_POS, NewState);
	TIM_Cmd(BUZZ_TIM_NEG, NewState);
}

//reinitializes the timer of "BUZZ_TIM_POS" with new prescaler and channel pulse value
//duty circle remains the smae (50%)
void BuzzerTimerandChannelReInit(uint16_t freq){
	uint32_t timclock;
	timclock = VarioCC_GetTimBusClockFreq();
	BuzzerTimerInit(timclock, freq);
	BuzzerChannelInit(freq);
}

//Initializes the timer ("PULSE_TIM"), that is to make beep noise on the Buzzer
//  - enables periphclock for timer that is used
//  - initializes timer, prescaler and period values are computed
//        according to the values defined in Buzzer.h and to the value of PCLK
//  - initializes a timer Channel for PULSE_TIM_OC
//  - enables TIM_IT_Update interrupt for the timer
//void PulseTimerInit(uint32_t timclock, uint16_t pfreq){
//	TIM_TimeBaseInitTypeDef timerinitstruct;
//
//	PULSE_RCC_TIM_CMD(ENABLE);
//
//	TIM_TimeBaseStructInit(&timerinitstruct);
//	timerinitstruct.TIM_Prescaler = (timclock / TIM_PULSETICK_FREQ) - 1;//(timclock - TIM_PULSETICK_FREQ)/TIM_PULSETICK_FREQ
//	timerinitstruct.TIM_Period = (pfreq*TIM_PULSETICK_FREQ) / 10 - 1;
//	TIM_TimeBaseInit(PULSE_TIM, &timerinitstruct);
//	TIM_Cmd(PULSE_TIM, ENABLE);
//	TIM_ITConfig(PULSE_TIM, TIM_IT_Update, ENABLE);
//}
void PulseTimerInit(uint32_t timclock, uint16_t pfreq){
	TIM_TimeBaseInitTypeDef timerinitstruct;

	PULSE_RCC_TIM_CMD(ENABLE);

	TIM_TimeBaseStructInit(&timerinitstruct);
	timerinitstruct.TIM_Prescaler = (timclock - TIM_PULSETICK_FREQ) / TIM_PULSETICK_FREQ;
	timerinitstruct.TIM_Period = TIM_PULSEPERIOD - 1;
	TIM_TimeBaseInit(PULSE_TIM, &timerinitstruct);
	TIM_Cmd(PULSE_TIM, ENABLE);
	TIM_ITConfig(PULSE_TIM, TIM_IT_Update, ENABLE);
}

//initializes a channel for "PULSE_TIM" in Vario_CC_board.h
//  - enables TIM_IT_CCx interrupt for the timer
//        TIM_IT_CCx in defined in Vario_CC_board.h as PULSE_TIM_CC_USED
void PulseChannelInit(){
	TIM_OCInitTypeDef ocinitstruct;

	TIM_OCStructInit(&ocinitstruct);
	ocinitstruct.TIM_Pulse = TIM_PULSECHANNELCOMPARE;
	PULSE_TIM_OC_INIT(&ocinitstruct);
	PULSE_TIM_OC_PRECONF(TIM_OCPreload_Enable);
	TIM_ITConfig(PULSE_TIM, PULSE_TIM_CC_USED, ENABLE);
}

//Enables interrupt in NVIC for both the channel (TIM_IT_CCx) and the timer (TIM_IT_Update)
void PulseTimerEnableInterrupts(){
	NVIC_InitTypeDef nvicinitstructure;
	nvicinitstructure.NVIC_IRQChannel = TIM1_BRK_UP_TRG_COM_IRQn;
	nvicinitstructure.NVIC_IRQChannelPriority = 1;
	nvicinitstructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvicinitstructure);

	nvicinitstructure.NVIC_IRQChannel = TIM1_CC_IRQn;
	nvicinitstructure.NVIC_IRQChannelPriority = 0;
	nvicinitstructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvicinitstructure);
}

//Initializes everything about the timer that is needed to make a beep noise on the buzzer
//  - initializes the timer
//  - initializes the channel
//  - sets the NVIC interrupt vector for them
void PulseInit(uint16_t pfreq){
	uint32_t timclock;
	timclock = VarioCC_GetTimBusClockFreq();
	PulseTimerInit(timclock, pfreq);
//	PulseChannelInit();
	PulseTimerEnableInterrupts();
	intbuzzcounter = 0;
	intbuzzenable = DISABLE;
	intbuzzthreshold = 25;
}

//Sets the frequency of the beep noise by resetting the timer "PULSE_TIM"
//input: pfreq: thatermines the frequency of beep noise
//            frequency=1/(TIM_PULSETICK_FREQ*pfreq/10)
//            so if TIM_PULSETICK_FREQ = 1000 and pfreq = 1, then frequency = 10Hz
//            se beep is gonna happen every 100ms
//void PulseSetFreq(uint16_t pfreq){
//	uint32_t timclock;
//	uint32_t counter;
//	timclock = VarioCC_GetTimBusClockFreq();
//	TIM_Cmd(PULSE_TIM, DISABLE);
//	counter = TIM_GetCounter(PULSE_TIM);
//	if (counter > (pfreq*TIM_PULSETICK_FREQ) / 10 - 1){
//		TIM_SetCounter(PULSE_TIM, 0);
//	}
//	PulseTimerInit(timclock, pfreq);
//}

//interrupt handler for "PULSE_TIM"
//it is called of the counter of the timer has reached the period value (set during initialization)
void TIM1_BRK_UP_TRG_COM_IRQHandler(){
	if (TIM_GetITStatus(PULSE_TIM, TIM_IT_Update) != RESET){
		TIM_ClearITPendingBit(PULSE_TIM, TIM_IT_Update);
		if (intbuzzcounter > intbuzzthreshold && intbuzzenable == ENABLE){
			BuzzerEnable(ENABLE);
			intbuzzcounter = 0;
		}
		else{
			intbuzzcounter++;
			if (intbuzzcounter == TIM_PULSECHANNELCOMPARE){
				BuzzerEnable(DISABLE);
			}
		}
		/*if (doesitsleep == 1){
			PWR_EnterSleepMode(PWR_SLEEPEntry_WFI);
		}*/
	}
}

//interrupt handler for "PULSE_TIM"
//it is called of the counter of the timer has reached the pulse value (set during initialization of the channel)
//void TIM1_CC_IRQHandler(){
//	if (TIM_GetITStatus(PULSE_TIM, PULSE_TIM_CC_USED) != RESET){
//		TIM_ClearITPendingBit(PULSE_TIM, PULSE_TIM_CC_USED);
//		BuzzerEnable(DISABLE);
//		/*if (doesitsleep == 1){
//			PWR_EnterSleepMode(PWR_SLEEPEntry_WFI);
//		}*/
//	}
//}