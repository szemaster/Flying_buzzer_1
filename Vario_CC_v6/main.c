#include "Buzzer.h"
#include "PowerLed.h"
#include "PressureSensor.h"

int main()
{
	/*short temp;
	long pressure;*/
	//unsigned char data[2];
	BuzzerInit(TIM_BUZZOVERFLOW_FREQ_HIGH);
	BuzzerEnable(ENABLE);
	PowerLed_Init();
//	VarioCC_DelayTIMInit();
	PressureSensor_Init();
	PulseInit(10);

	while (1)
	{
		/*VarioCC_Delay(2000);
		BuzzerTimerandChannelReInit(TIM_BUZZOVERFLOW_FREQ_LOW);*/
		PowerLed_Main();
		/*temp = PressureSensor_GetRealTempr();
		pressure = PressureSensor_GetRealPressure();*/
		PressureSense_Main();
		/*VarioCC_Delay(2000);
		BuzzerTimerandChannelReInit(TIM_BUZZOVERFLOW_FREQ_HIGH);*/
	}
}