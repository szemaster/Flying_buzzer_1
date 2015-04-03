/*
* Vario_CC_board.c
*
*  Created on: 2015.03.16.
*      Author: Cusi
*/


#include "Vario_CC_board.h"

//computes the frequency that the timers are driven from
//(in stm32f030 the timer prescaler depends on the prescaler of the PCLK bus)
uint32_t VarioCC_GetTimBusClockFreq(){
	RCC_ClocksTypeDef rccclocks;
	uint32_t timclock;

	RCC_GetClocksFreq(&rccclocks);
	if (rccclocks.HCLK_Frequency == rccclocks.PCLK_Frequency){  //this condition is because of the clocktree of the MCU
		timclock = rccclocks.PCLK_Frequency;
	}
	else{
		timclock = rccclocks.PCLK_Frequency << 1;
	}
	return timclock;
}

//initializes the timer that is used for the VarioCC_Delay(uint16_t delay) function
//The used TIMx periphery is define in Vario_CC_board.h as "DELAY_TIM"
//The prescaler and period values are computed according to the values of "DELAY_TIM_TICK_FREQ" and "DELAY_FREQ" in Vario_CC_board.h
void VarioCC_DelayTIMInit(){
	TIM_TimeBaseInitTypeDef timerinitstruct;
	uint32_t timclock;

	DELAY_RCC_TIM_CMD(ENABLE);

	timclock = VarioCC_GetTimBusClockFreq();

	TIM_TimeBaseStructInit(&timerinitstruct);
	timerinitstruct.TIM_Prescaler = (timclock / DELAY_TIM_TICK_FREQ) - 1;
	timerinitstruct.TIM_Period = DELAY_TIM_TICK_FREQ / DELAY_FREQ - 1;
	TIM_TimeBaseInit(DELAY_TIM, &timerinitstruct);
	TIM_ITConfig(DELAY_TIM, TIM_IT_Update, ENABLE);
}

//Delays one unit of time. The length of the delay is determined in Vario_CC_board.h as DELAY_FREQ
//(Delay in sec)=1/DELAY_FREQ
//input: - delay: number of time units to delay
void VarioCC_Delay(uint16_t delay){
	TIM_SetCounter(DELAY_TIM, 0);
	TIM_Cmd(DELAY_TIM, ENABLE);
	for (; delay > 0; delay--){
		while (TIM_GetITStatus(DELAY_TIM, TIM_IT_Update) == RESET){ ; }
		TIM_ClearITPendingBit(DELAY_TIM, TIM_IT_Update);
	}
	TIM_Cmd(DELAY_TIM, DISABLE);
}