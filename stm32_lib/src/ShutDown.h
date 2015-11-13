#ifndef SHUTDOWN_H_
#define SHUTDOWN_H_

#include "Vario_CC_board.h"
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_gpio.h"

#define COUNTER_RVALUE       100000
#define COUNTER_LEDTIME      200000

void SHD_Init();
void SHD_Main();

#endif
