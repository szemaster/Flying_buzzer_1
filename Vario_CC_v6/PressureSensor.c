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
	float velocity;
	if (intcounter == 0){

		pressure = PressureSensor_GetRealPressure();
		//	datap[cnt++] = pressure;
		PressureSense_InsertData(pressure);
		velocity = PressureSense_CalculateVelocity_Fast();
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
}

void PressureSense_InsertData(uint32_t newdata){
	uint16_t i;
	for (i = (PRESS_SENS_ARRAYLENGTH - 1); i > 0; i--){
		pressens_pressarray[i] = pressens_pressarray[i - 1];
	}
	pressens_pressarray[0] = newdata;
}

/*void PressureSense_InsertData(uint16_t length, uint32_t dataarray[], uint32_t newdata){
	uint16_t i;
	for (i = 0; i < (length - 1); i++){
		dataarray[i] = dataarray[i + 1];
	}
	dataarray[i] = newdata;
}*/

/*void PressureSense_InsertDataAll(uint32_t newdata){
	PressureSense_InsertData(PRESS_SENS_DATAPOINTNUM_FAST, pressens_pressarray_fast, newdata);
	PressureSense_InsertData(PRESS_SENS_DATAPOINTNUM_SLOW, pressens_pressarray_slow, newdata);
}*/

uint32_t PressureSense_CalculateSumXiTYi(uint16_t length){
	uint16_t i = 0;
	uint32_t result = 0;
	for (i = length; i > 0; i--){
		result += pressens_pressarray[i-1] * (length - (i-1));
	}
	return result;
}

/*//calculates the value of sum(Xi*Yi) from i=1 to i=length
uint32_t PressureSense_CalculateSumXiTYi(uint16_t length, uint32_t dataarray[]){
	uint16_t i = 0;
	uint32_t result = 0;
	for (; i < length; i++){
		result += dataarray[i] * (i + 1);
	}
	return result;
}*/

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

/*//calculates the average of last pressure value "length" long
float PressureSense_CalculateAvgYi(uint16_t length, uint32_t dataarray[]){
	uint16_t i = 0;
	uint32_t sum = 0;
	float avg = 0;
	for (; i < length; i++){
		sum += dataarray[i];
	}
	avg = (float)sum/(float)length;
	return avg;
}*/

//calculates the vertical velocity according to the last few pressure values (lats few ic defined as "PRESS_SENS_DATAPOINTNUM_FAST")
float PressureSense_CalculateVelocity_Fast(){
	uint32_t sumxityi;                      //means: sum(xi*yi)
	float yiavg;
	float velocity;
	static int i = 1;
	sumxityi = PressureSense_CalculateSumXiTYi(PRESS_SENS_DATAPOINTNUM_FAST);
	yiavg = PressureSense_CalculateAvgYi(PRESS_SENS_DATAPOINTNUM_FAST);
	velocity = ((float)yiavg*PRESS_SENS_CALC_SUMXI_FAST - (float)sumxityi) / ((float)(PRESS_SENS_CALC_XAVGTIMESSUM_FAST - (float)PRESS_SENS_CALC_SUMXISQUARED_FAST));
	

		if (i++ >= 75)//{
		asm("nop");
/*		datav[i - 75] = velocity;
	}
	i++;
	if (i == 400){
		asm("nop");
	}*/
	return velocity;
}

//calculates the vertical velocity according to the last few pressure values (lats few ic defined as "PRESS_SENS_DATAPOINTNUM_SLOW")
float PressureSense_CalculateVelocity_Slow(){
	uint32_t sumxityi;                      //means: sum(xi*yi)
	float yiavg;
	float velocity;
//static int i = 1;
	sumxityi = PressureSense_CalculateSumXiTYi2(PRESS_SENS_DATAPOINTNUM_SLOW);
	yiavg = PressureSense_CalculateAvgYi2(PRESS_SENS_DATAPOINTNUM_SLOW);
	velocity = ((float)yiavg*PRESS_SENS_CALC_SUMXI_SLOW - (float)sumxityi) / ((float)(PRESS_SENS_CALC_XAVGTIMESSUM_SLOW - (float)PRESS_SENS_CALC_SUMXISQUARED_SLOW));

	

/*	if (i >= 150){
		asm("nop");
		datav[i - 150] = velocity;
	}
	i++;
	if (i == 400){
		asm("nop");
	}*/
	return velocity;
}
