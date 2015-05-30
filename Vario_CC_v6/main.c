#include "Vario_CC_board.h"
#include "Buzzer.h"
#include "PowerLed.h"
#include "PressureSensor.h"

int main()
{
	/*short temp;
	long pressure;*/
	//unsigned char data[2];

//	doesitsleep = 0;
	PulseInit(1);
	BuzzerInit(TIM_BUZZOVERFLOW_FREQ_LOW);
	BuzzerEnable(ENABLE);

	PowerLed_Init();
//	VarioCC_DelayTIMInit();
	PressureSensor_Init();	

	while (1)
	{


	PowerLed_Main();

	PressureSense_Main();

	/*doesitsleep = 1;
	PWR_EnterSleepMode(PWR_SLEEPEntry_WFI);*/
	}
}