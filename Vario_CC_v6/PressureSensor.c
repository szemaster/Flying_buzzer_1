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
		PRESS_SENS_CALC_REALTEMP();
		i = 0;
	}
	i++;
}

void PressureSense_Main(){
	uint32_t pressure;
//	static uint16_t cnt = 0;
	static int i=0;
	static uint16_t startuptime = 0;
	float velocity;
	/*if (i == 0){
		GPIO_SetBits(PLED_GPIO, PLED_PIN);
	}*/

		if (intcounter == 0){
			//i++;
			pressure = PressureSensor_GetRealPressure();
			//	datap[cnt++] = pressure;
			PressureSense_InsertData(pressure);
			if (startuptime > PRESS_SENS_STARTUPTIME){
				if (i++ == 8){
					PressureSense_DetermineVelocity();
					i = 0;
				}
			}
			else{
				startuptime++;
			}
			PressureSensor_GetUTIfNeeded();
			PRESS_SENS_START_UPMEAS(PRESS_SENS_RESOLUTION);			
	}
	/*if (i == 1999){
		GPIO_ResetBits(PLED_GPIO, PLED_PIN);
	}*/

}

//Determines which velocity (fast/slow) should be used for the calculation of the beeping frequency
void PressureSense_DetermineVelocity(){
	static char slowingdown = 0;
	int32_t velofast, veloslow;
	char positive = 1;
static int i = 0;

	velofast = PressureSense_CalculateVelocity_Fast();
	VELOCITYFAST[i] = velofast;
	if (velofast < 0){
		velofast = velofast * (-1);
		positive = 0;
	}
	else{
		positive = 1;
	}
	if (velofast > PRESS_SENS_LIMIT_FAST){
		slowingdown = 0;
		PressureSense_BuzzerControl(velofast, positive);
	}
	else{
		veloslow = PressureSense_CalculateVelocity_Slow();
		VELOCITYSLOW[i] = veloslow;
		if (veloslow<0){
			veloslow = veloslow * (-1);
			positive = 0;
		}
		else{
			positive = 1;
		}
		if (veloslow > PRESS_SENS_LIMIT_SLOW && veloslow < PRESS_SENS_LIMIT_FAST){
			if (velofast > veloslow){
				slowingdown = 0;
			}
			if (slowingdown == 0){
				PressureSense_BuzzerControl(veloslow, positive);
			}
			else if (slowingdown == 1 && velofast > PRESS_SENS_LIMIT_SLOW){
				PressureSense_BuzzerControl(velofast, positive);
			}
		}
		else if (veloslow > PRESS_SENS_LIMIT_FAST){
			slowingdown = 1;
			PressureSense_BuzzerControl(velofast, positive);
		}
		else if (veloslow < PRESS_SENS_LIMIT_SLOW){
			slowingdown = 0;
			intbuzzenable = DISABLE;
		}
	}
//	i++;
	if (i == 650){
		for (;;){
			asm("nop");
		}
	}
}

void PressureSense_BuzzerControl(int32_t velocity, char positive){
	static char posold = 1;

	if (positive == 0 && posold == 1){
		BuzzerInit(TIM_BUZZOVERFLOW_FREQ_HIGH);
		posold = positive;
	}
	else if (positive == 1 && posold == 0){
		BuzzerInit(TIM_BUZZOVERFLOW_FREQ_LOW);
		posold = positive;
	}
	if (velocity < 100){
		intbuzzenable = DISABLE;
	}
	else{
		if (velocity < 200)
			intbuzzthreshold = 25;
		else if (velocity < 300)
			intbuzzthreshold = 20;
		else if (velocity < 400)
			intbuzzthreshold = 16;
		else if (velocity < 500)
			intbuzzthreshold = 13;
		else if (velocity < 600)
			intbuzzthreshold = 11;
		else if (velocity < 700)
			intbuzzthreshold = 9;
		else if (velocity < 800)
			intbuzzthreshold = 7;
		else if (velocity < 900)
			intbuzzthreshold = 6;
		else if (velocity < 1000)
			intbuzzthreshold = 5;
		else intbuzzthreshold = 4;
		intbuzzenable = ENABLE;
	}

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
int32_t PressureSense_CalculateVelocity_Fast(){
	uint64_t sumxityi;                      //means: sum(xi*yi)
	float yiavg;
	float velocity;
	static int i = 1;
	sumxityi = PressureSense_CalculateSumXiTYi(PRESS_SENS_DATAPOINTNUM_FAST);
	yiavg = PressureSense_CalculateAvgYi(PRESS_SENS_DATAPOINTNUM_FAST);
	velocity = ((float)yiavg*PRESS_SENS_CALC_SUMXI_FAST - (float)sumxityi) / ((float)(PRESS_SENS_CALC_XAVGTIMESSUM_FAST - (float)PRESS_SENS_CALC_SUMXISQUARED_FAST));
	

	/*	if (i >= 300){
//		asm("nop");
		VELOCITY[i - 300] = velocity*1000;
	}
	i++;
	if (i == 1600){
		asm("nop");
	}*/
	return velocity*1000;
}

//calculates the vertical velocity according to the last few pressure values (lats few ic defined as "PRESS_SENS_DATAPOINTNUM_SLOW")
int32_t PressureSense_CalculateVelocity_Slow(){
	uint64_t sumxityi;                      //means: sum(xi*yi)
	float yiavg;
	float velocity;
	static int i = 1;
	sumxityi = PressureSense_CalculateSumXiTYi(PRESS_SENS_DATAPOINTNUM_SLOW);
	yiavg = PressureSense_CalculateAvgYi(PRESS_SENS_DATAPOINTNUM_SLOW);
	velocity = ((float)yiavg*PRESS_SENS_CALC_SUMXI_SLOW - (float)sumxityi) / ((float)(PRESS_SENS_CALC_XAVGTIMESSUM_SLOW - (float)PRESS_SENS_CALC_SUMXISQUARED_SLOW));

	

	/*if (i >= 375){
		VELOCITY[i - 375] = velocity * 1000;
	}
	i++;
	if (i == 1675){
		asm("nop");
	}*/
	return velocity*1000;
}

int32_t PressureSense_CalculateVelocity_VerySlow(){
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
	return velocity*1000;
}

//Interrupt funcion for "VarioCC_Delay" and for "VarioCC_NonBlockingDelay_Start" functions
void TIM14_IRQHandler(){
	if (TIM_GetITStatus(DELAY_TIM, TIM_IT_Update) != RESET){
		TIM_ClearITPendingBit(DELAY_TIM, TIM_IT_Update);
		if (intcounter>0)
			intcounter--;
	}
}
