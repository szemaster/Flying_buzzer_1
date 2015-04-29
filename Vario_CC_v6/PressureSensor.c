#include "PressureSensor.h"


void PressureSensor_Init(){
	I2CLine_Init(I2C_I2C_USED);

	VarioCC_DelayTIMInit();

	VarioCC_Delay(PRESS_SENS_STARTUPTIME);
	PressureSensor_Reset();

	VarioCC_Delay(PRESS_SENS_STARTUPTIME);
	PRESS_SENS_READ_CALIBR_DATA();

    BMP180_READ_UP_START(PRESS_SENS_RESOLUTION);
}

//reads the the uncompensated temerature value and calcuates the real temperature value
short PressureSensor_GetRealTempr(){
	PRESS_SENS_GET_UTEMPR();
	return PRESS_SENS_CALC_REALTEMP();
}

//reads the the uncompensated pressure value and calcuates the real pressure value
long PressureSensor_GetRealPressure(){
	PRESS_SENS_GET_UPRESS();
	return PRESS_SENS_CALC_REALPRESSURE(PRESS_SENS_RESOLUTION);
}

//sends the reset command to the sensor
void PressureSensor_Reset(){
	I2CLine_Write(I2C_I2C_USED, PRESS_SENS_DEV_ADDR, PRESS_SENS_RESET_ADDR, PRESS_SENS_RESET_COMMAND);
}

//This function checks if it's time to remeasure the temperature and if it is, than it remeasures the temperature.
void PressureSensor_GetUTIfNeeded(){
	static uint8_t i = PRESS_SENS_FREQOFTEMPRMEASUREMENT;
	if (i == PRESS_SENS_FREQOFTEMPRMEASUREMENT){
		PRESS_SENS_GET_UTEMPR();
		i = 0;
	}
	i++;
}

void PressureSense_Main(){
	uint32_t pressure;
//	static uint16_t cnt = 0;
//	static int i=0;
	float velocity;
	/*if (i == 0){
		GPIO_SetBits(PLED_GPIO, PLED_PIN);
	}*/

		if (intcounter == 0){
			//i++;
			pressure = PressureSensor_GetRealPressure();
			//	datap[cnt++] = pressure;
			PressureSense_InsertData(pressure);
			//velocity = PressureSense_CalculateVelocity_Fast();
			velocity = PressureSense_CalculateVelocity_VerySlow();
			//velocity = PressureSense_CalculateVelocity_Slow();
			PressureSensor_GetUTIfNeeded();
			PRESS_SENS_START_UPMEAS(PRESS_SENS_RESOLUTION);

			/*if (velocity > PRESS_SENS_LIMIT_FAST || velocity < (-PRESS_SENS_LIMIT_FAST)){
		//ezek itt abszolút buta értékek
		BuzzerEnable(ENABLE);
		PulseSetFreq(5);
		}
		/*else{
		velocity = PressureSense_CalculateVelocity_Slow();
		if (velocity>PRESS_SENS_LIMIT_SLOW || velocity < (-PRESS_SENS_LIMIT_SLOW)){
		//ezek itt abszolút buta értékek
		TIM_Cmd(PULSE_TIM, ENABLE);
		PulseSetFreq(10);
		}
		else{
		BuzzerEnable(DISABLE);
		//}
		}*/
	
	}
	/*if (i == 1999){
		GPIO_ResetBits(PLED_GPIO, PLED_PIN);
	}*/

}

void PressureSense_InsertData(uint32_t newdata){
	uint16_t i;
	for (i = (PRESS_SENS_ARRAYLENGTH - 1); i > 0; i--){
		pressens_pressarray[i] = pressens_pressarray[i - 1];
	}
	pressens_pressarray[0] = newdata;
}


uint64_t PressureSense_CalculateSumXiTYi(uint16_t length){
	uint16_t i = 0;
	uint64_t result = 0;
	for (i = length; i > 0; i--){
		result += pressens_pressarray[i-1] * (length - (i-1));
	}
	return result;
}


float PressureSense_CalculateAvgYi(uint16_t length){
	uint16_t i = 0;
	uint32_t sum = 0;
	float avg = 0;
	for (; i < length; i++){
		sum += pressens_pressarray[i];
	}
	avg = (float)sum / (float)length;
	return avg;
}


//calculates the vertical velocity according to the last few pressure values (lats few ic defined as "PRESS_SENS_DATAPOINTNUM_FAST")
float PressureSense_CalculateVelocity_Fast(){
	uint64_t sumxityi;                      //means: sum(xi*yi)
	float yiavg;
	float velocity;
	static int i = 1;
	sumxityi = PressureSense_CalculateSumXiTYi(PRESS_SENS_DATAPOINTNUM_FAST);
	yiavg = PressureSense_CalculateAvgYi(PRESS_SENS_DATAPOINTNUM_FAST);
	velocity = ((float)yiavg*PRESS_SENS_CALC_SUMXI_FAST - (float)sumxityi) / ((float)(PRESS_SENS_CALC_XAVGTIMESSUM_FAST - (float)PRESS_SENS_CALC_SUMXISQUARED_FAST));
	

		if (i >= 300){
//		asm("nop");
		VELOCITY[i - 300] = velocity*1000;
	}
	i++;
	if (i == 1600){
		asm("nop");
	}
	return velocity;
}

//calculates the vertical velocity according to the last few pressure values (lats few ic defined as "PRESS_SENS_DATAPOINTNUM_SLOW")
float PressureSense_CalculateVelocity_Slow(){
	uint64_t sumxityi;                      //means: sum(xi*yi)
	float yiavg;
	float velocity;
	static int i = 1;
	sumxityi = PressureSense_CalculateSumXiTYi(PRESS_SENS_DATAPOINTNUM_SLOW);
	yiavg = PressureSense_CalculateAvgYi(PRESS_SENS_DATAPOINTNUM_SLOW);
	velocity = ((float)yiavg*PRESS_SENS_CALC_SUMXI_SLOW - (float)sumxityi) / ((float)(PRESS_SENS_CALC_XAVGTIMESSUM_SLOW - (float)PRESS_SENS_CALC_SUMXISQUARED_SLOW));

	

	if (i >= 375){
		VELOCITY[i - 375] = velocity * 1000;
	}
	i++;
	if (i == 1675){
		asm("nop");
	}
	return velocity;
}

float PressureSense_CalculateVelocity_VerySlow(){
	uint64_t sumxityi;                      //means: sum(xi*yi)
	float yiavg;
	float velocity;
static int i = 1;
	sumxityi = PressureSense_CalculateSumXiTYi(PRESS_SENS_DATAPOINTNUM_VERYSLOW);
	yiavg = PressureSense_CalculateAvgYi(PRESS_SENS_DATAPOINTNUM_VERYSLOW);
	velocity = ((double)yiavg*PRESS_SENS_CALC_SUMXI_VERYSLOW - (double)sumxityi) / ((double)(PRESS_SENS_CALC_XAVGTIMESSUM_VERYSLOW - (double)PRESS_SENS_CALC_SUMXISQUARED_VERYSLOW));
	
	/*if (i++ >= 300){
		asm("nop");
	}*/

/*	if (i >= 800){
		VELOCITY[i - 800] = velocity * 1000;
	}
	i++;
	if (i == 2100){
		asm("nop");
	}*/
	return velocity;
}
