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
#ifdef PRESS_SENS_DEBUGMODE
	static uint16_t cnt = 0;
#endif
	static uint16_t i = 0;
	static uint16_t startuptime = 0;
	float velocity;
	/*if (i == 0){
	GPIO_SetBits(PLED_GPIO, PLED_PIN);
	}*/

	if (intcounter == 0){
		//i++;
		pressure = PressureSensor_GetRealPressure();
#ifdef PRESS_SENS_DEBUGMODE
		datap[cnt++] = (int16_t)(pressure - 100000);
#endif
		PressureSense_InsertData(pressure);
		PressureSensor_GetUTIfNeeded();
		PRESS_SENS_START_UPMEAS(PRESS_SENS_RESOLUTION);
		if (startuptime > PRESS_SENS_DEVICESTARTUPTIME){
			if (i++ == 8){
				PressureSense_DetermineVelocity();
				i = 0;
			}
		}
		else{
			startuptime++;
		}
	}
	/*if (i == 1999){
	GPIO_ResetBits(PLED_GPIO, PLED_PIN);
	}*/
#ifdef PRESS_SENS_DEBUGMODE
	if (cnt == 1620){
		cnt = 0;
	}
#endif
}

//Determines which velocity (fast/slow) should be used for the calculation of the beeping frequency
void PressureSense_DetermineVelocity(){
	static char slowingdown = 0;
	int32_t velofast, veloslow, veloveryslow;
	char positive = 1;
	PRESS_SENS_VELOCALCSARRAY_INIT;
	static int i = 0;

	velofast = PressureSense_CalculateVelocity(&velocalc[0]);/*PressureSense_CalculateVelocity_Fast()*/;
	//VELOCITYFAST[i] = velofast;
	velofast = PressureSense_IsPositive(velofast, &positive);
	if (velofast > PRESS_SENS_LIMIT_FAST){
		slowingdown = 0;
		PressureSense_BuzzerControl(velofast, positive);
	}
	else{
		veloslow = PressureSense_CalculateVelocity(&velocalc[1])/*PressureSense_CalculateVelocity_Slow()*/;
		//		VELOCITYSLOW[i] = veloslow;

		veloslow = PressureSense_IsPositive(veloslow, &positive);
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
		else{
			veloveryslow = /*PressureSense_CalculateVelocity_VerySlow()*/PressureSense_CalculateVelocity(&velocalc[2]);
			//			VELOCITYVERYSLOW[i] = veloveryslow;
			veloveryslow = PressureSense_IsPositive(veloveryslow, &positive);
			if (veloveryslow > PRESS_SENS_LIMIT_VERYSLOW && veloveryslow < PRESS_SENS_LIMIT_SLOW){
				if (veloslow > veloveryslow){
					slowingdown = 0;
				}
				if (slowingdown == 0){
					PressureSense_BuzzerControl(veloveryslow, positive);
				}
				else if (slowingdown == 1 && veloslow > PRESS_SENS_LIMIT_VERYSLOW){
					PressureSense_BuzzerControl(veloslow, positive);
				}
			}
			else if (veloveryslow > PRESS_SENS_LIMIT_SLOW){
				slowingdown = 1;
				PressureSense_BuzzerControl(veloslow, positive);
			}
			else //if (veloveryslow < PRESS_SENS_LIMIT_VERYSLOW)
			{
				//Interesting bug: If you uncomment the condition above the VarioCC_Delay funcion does not work in PressureSensor_Init() function
				//It is quite unbelievable, but still. I have no idea what is the problem
				slowingdown = 0;
				intbuzzenable = DISABLE;
			}
		}
	}
	/*	i++;
	if (i == 150){
	for (;;){
	asm("nop");
	}
	}*/
}

int32_t PressureSense_IsPositive(int32_t velocity, char* positive){
	if (velocity < 0){
		velocity = velocity * (-1);
		*positive = 0;
	}
	else{
		*positive = 1;
	}
	return velocity;
}

void PressureSense_BuzzerControl(int32_t velocity, char positive){
	static char posold = 1;
	uint8_t i;
	PRESS_SENS_STAGEARRAY_INIT;
	PRESS_SENS_BUZZARRAY_INIT;

	if (positive == 0 && posold == 1){
		BuzzerInit(TIM_BUZZOVERFLOW_FREQ_HIGH);
		posold = positive;
	}
	else if (positive == 1 && posold == 0){
		BuzzerInit(TIM_BUZZOVERFLOW_FREQ_LOW);
		posold = positive;
	}
	if (velocity < stagethresholds[0]){
		intbuzzenable = DISABLE;
	}
	else{
		for (i = 1; i < PRESS_SENS_BUZZ_STAGES_NUM; i++){
			if (velocity < stagethresholds[i]){
				intbuzzthreshold = buzzthresholds[i - 1];
				break;
			}
		}
		if (velocity > stagethresholds[PRESS_SENS_BUZZ_STAGES_NUM - 1]){
			intbuzzthreshold = buzzthresholds[PRESS_SENS_BUZZ_STAGES_NUM - 1];
		}
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
		result += pressens_pressarray[i - 1] * (length - (i - 1));
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
//int32_t PressureSense_CalculateVelocity_Fast(){
//	uint64_t sumxityi;                      //means: sum(xi*yi)
//	float yiavg;
//	float velocity;
////	static int i = 1;
//	sumxityi = PressureSense_CalculateSumXiTYi(PRESS_SENS_DATAPOINTNUM_FAST);
//	yiavg = PressureSense_CalculateAvgYi(PRESS_SENS_DATAPOINTNUM_FAST);
//	velocity = ((float)yiavg*PRESS_SENS_CALC_SUMXI_FAST - (float)sumxityi) / ((float)(PRESS_SENS_CALC_XAVGTIMESSUM_FAST - (float)PRESS_SENS_CALC_SUMXISQUARED_FAST));
//	
//
//	/*	if (i >= 300){
////		asm("nop");
//		VELOCITY[i - 300] = velocity*1000;
//	}
//	i++;
//	if (i == 1600){
//		asm("nop");
//	}*/
//	return velocity*1000;
//}

//calculates the vertical velocity according to the last few pressure values (lats few ic defined as "PRESS_SENS_DATAPOINTNUM_SLOW")
//int32_t PressureSense_CalculateVelocity_Slow(){
//	uint64_t sumxityi;                      //means: sum(xi*yi)
//	float yiavg;
//	float velocity;
////	static int i = 1;
//	sumxityi = PressureSense_CalculateSumXiTYi(PRESS_SENS_DATAPOINTNUM_SLOW);
//	yiavg = PressureSense_CalculateAvgYi(PRESS_SENS_DATAPOINTNUM_SLOW);
//	velocity = ((float)yiavg*PRESS_SENS_CALC_SUMXI_SLOW - (float)sumxityi) / ((float)(PRESS_SENS_CALC_XAVGTIMESSUM_SLOW - (float)PRESS_SENS_CALC_SUMXISQUARED_SLOW));
//
//	
//
//	/*if (i >= 375){
//		VELOCITY[i - 375] = velocity * 1000;
//	}
//	i++;
//	if (i == 1675){
//		asm("nop");
//	}*/
//	return velocity*1000;
//}

//int32_t PressureSense_CalculateVelocity_VerySlow(){
//	uint64_t sumxityi;                      //means: sum(xi*yi)
//	float yiavg;
//	float velocity;
////static int i = 1;
//	sumxityi = PressureSense_CalculateSumXiTYi(PRESS_SENS_DATAPOINTNUM_VERYSLOW);
//	yiavg = PressureSense_CalculateAvgYi(PRESS_SENS_DATAPOINTNUM_VERYSLOW);
//	velocity = ((double)yiavg*PRESS_SENS_CALC_SUMXI_VERYSLOW - (double)sumxityi) / ((double)(PRESS_SENS_CALC_XAVGTIMESSUM_VERYSLOW - (double)PRESS_SENS_CALC_SUMXISQUARED_VERYSLOW));
//	
//	/*if (i++ >= 300){
//		asm("nop");
//	}*/
//
///*	if (i >= 800){
//		VELOCITY[i - 800] = velocity * 1000;
//	}
//	i++;
//	if (i == 2100){
//		asm("nop");
//	}*/
//	return velocity*1000;
//}

int32_t PressureSense_CalculateVelocity(ps_velocalcconts* velocalcconsts){
	uint64_t sumxityi;                      //means: sum(xi*yi)
	float yiavg;
	float velocity;
	sumxityi = PressureSense_CalculateSumXiTYi(velocalcconsts->datapointnum);
	yiavg = PressureSense_CalculateAvgYi(velocalcconsts->datapointnum);
	velocity = ((double)yiavg*velocalcconsts->sumxi - (double)sumxityi) / ((double)(velocalcconsts->avgtsum - (double)velocalcconsts->sumxi2));
	return velocity * 1000;
}

//Interrupt funcion for "VarioCC_Delay" and for "VarioCC_NonBlockingDelay_Start" functions
void TIM14_IRQHandler(){
	if (TIM_GetITStatus(DELAY_TIM, TIM_IT_Update) != RESET){
		TIM_ClearITPendingBit(DELAY_TIM, TIM_IT_Update);
		if (intcounter>0)
			intcounter--;
		/*if (doesitsleep == 1 && intcounter>0){
			PWR_EnterSleepMode(PWR_SLEEPEntry_WFI);
		}
		else{
			doesitsleep = 0;
		}*/
	}
}
