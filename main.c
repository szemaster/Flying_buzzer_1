#include "Vario_CC_board.h"
#include "Buzzer.h"
#include "PowerLed.h"
#include "PressureSensor.h"
#include "ShutDown.h"



int main(void)
{
	SHD_Init();

	RCC_HCLKConfig(RCC_SYSCLK_Div4);

	PulseInit(1);
	BuzzerInit(TIM_BUZZOVERFLOW_FREQ_LOW);
	BuzzerEnable(ENABLE);

	PowerLed_Init();

	PressureSensor_Init();


    while(1)
    {
    	//PowerLed_Main();
    	PressureSense_Main();
    	SHD_Main();
    }
}


