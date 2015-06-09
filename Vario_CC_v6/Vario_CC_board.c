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

	VarioCC_DelayEnableInterrupts();
}

//The function enables the NVIC lines for the timer that is used as "DELAY_TIM"
void VarioCC_DelayEnableInterrupts(){
	NVIC_InitTypeDef nvicinitstructure;
	nvicinitstructure.NVIC_IRQChannel = DELAY_IRQN;
	nvicinitstructure.NVIC_IRQChannelPriority = 2;
	nvicinitstructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvicinitstructure);
}

//This is a blocking wait funcion. The program can't get over until the delaytime expires.
//input: delay: time to be delayed. Its unit is equal to (1/"DELAY_FREQ") is sec.
//Note: to be able to use this function, first of all you should call "VarioCC_DelayTIMInit()" function
void VarioCC_Delay(uint16_t delay){
	TIM_SetCounter(DELAY_TIM, 0);
	intcounter = delay;
	TIM_Cmd(DELAY_TIM, ENABLE);
	while (intcounter > 0){
		asm("nop");
	}
	TIM_Cmd(DELAY_TIM, DISABLE);
}

//This is a non-blocking timer function. You can do anything else, until the specified time expires.
//input: delay: time to be delayed. Its unit is equal to (1/"DELAY_FREQ") in sec.
//     "intcounter" is going to be 0, if the specified time has elapsed. 
//      intcounter is a volatile variable defined in "Vario_CC_board.h"
//Note: to be able to use this function, first of all you should call "VarioCC_DelayTIMInit()" function
void VarioCC_NonBlockingDelay_Start(uint16_t delay){
	TIM_Cmd(DELAY_TIM, DISABLE);
	TIM_SetCounter(DELAY_TIM, 0);
	intcounter = delay;
	TIM_Cmd(DELAY_TIM, ENABLE);
}



