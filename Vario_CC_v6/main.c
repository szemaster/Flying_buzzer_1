#include "Buzzer.h"
#include "PowerLed.h"
#include "PressureSensor.h"

int main()
{
	int i = 0;
	/*short temp;
	long pressure;*/
	//unsigned char data[2];


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

	}
}