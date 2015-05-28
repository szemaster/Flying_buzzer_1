#ifndef PRESSURESENSOR_H_
#define PRESSURESENSOR_H_

#include "Vario_CC_board.h"
#include "BMP180_api.h"
#include "Buzzer.h"

//#define PRESS_SENS_DEBUGMODE

//Functions and constants came from the API of the pressure sensor
	#define PRESS_SENS_READ_CALIBR_DATA()            BMP180_READ_CALDATA()
	#define	PRESS_SENS_GET_UTEMPR()                  BMP180_READ_UT()
	#define PRESS_SENS_CALC_REALTEMP()               BMP180_CALC_TEMP()
	#define PRESS_SENS_GET_UPRESS()                  BMP180_READ_UP_END()
	#define PRESS_SENS_CALC_REALPRESSURE(OSS)        BMP180_CALC_PRESS(OSS)
	#define PRESS_SENS_START_UPMEAS(OSS)             BMP180_READ_UP_START(OSS)

	#define PRESS_SENS_RESOLUTION                    BMP180_OSS_ULTRAHIGHRES
	#define PRESS_SENS_RESET_ADDR                    BMP180_ADDR_SOFT_RESET
	#define PRESS_SENS_RESET_COMMAND                 BMP180_COM_SOFT_RESET
	#define PRESS_SENS_DEV_ADDR                      BMP180_DEV_ADDR
	#define PRESS_SENS_STARTUPTIME                   BMP180_STARTUPTIME
//End of functions and constants came from the API of the pressure sensor

//Constants and definitions for line regression stages
	#define PRESS_SENS_FREQOFTEMPRMEASUREMENT        4
	//constants that specifies the way of altitude regression
	#define PRESS_SENS_STAGENUM                      3
	//stage fast
	#define PRESS_SENS_DATAPOINTNUM_FAST             70
	#define PRESS_SENS_CALC_SUMXI_FAST               2485              //from 1 to "PRESS_SENS_DATAPOINTNUM_FAST" sum(x)
	#define PRESS_SENS_CALC_SUMXISQUARED_FAST        116795            //from 1 to "PRESS_SENS_DATAPOINTNUM_FAST" sum(x^2)
	#define PRESS_SENS_CALC_XAVGTIMESSUM_FAST        88218             //x(avg)*PRESS_SENS_CALC_SUMXI_60, where x(avg)=(1+2+3+...+PRESS_SENS_DATAPOINTNUM_FAST)/PRESS_SENS_DATAPOINTNUM_FAST
	#define PRESS_SENS_LIMIT_FAST                    312               //velocity*1000 in mPa/timequantum. 312 means 12Pa/s @ 0.0.26s/sample
	//stage slow
	#define PRESS_SENS_DATAPOINTNUM_SLOW             140
	#define PRESS_SENS_CALC_SUMXI_SLOW               9870              //from 1 to "PRESS_SENS_DATAPOINTNUM_SLOW" sum(x)
	#define PRESS_SENS_CALC_SUMXISQUARED_SLOW        924490            //from 1 to "PRESS_SENS_DATAPOINTNUM_SLOW" sum(x^2)
	#define PRESS_SENS_CALC_XAVGTIMESSUM_SLOW        695835            //x(avg)*PRESS_SENS_CALC_SUMXI_150, where x(avg)=(1+2+3+...+PRESS_SENS_DATAPOINTNUM_SLOW)/PRESS_SENS_DATAPOINTNUM_SLOW
	#define PRESS_SENS_LIMIT_SLOW                    154               //velocity*1000 in mPa/timequantum. 154 means 4Pa/s @ 0.0.26s/sample
	//stage veryslow
	#define PRESS_SENS_DATAPOINTNUM_VERYSLOW         210
	#define PRESS_SENS_CALC_SUMXI_VERYSLOW           22155             //from 1 to "PRESS_SENS_DATAPOINTNUM_VERYSLOW" sum(x)
	#define PRESS_SENS_CALC_SUMXISQUARED_VERYSLOW    3109085           //from 1 to "PRESS_SENS_DATAPOINTNUM_VERYSLOW" sum(x^2)
	#define PRESS_SENS_CALC_XAVGTIMESSUM_VERYSLOW    2337352.5         //x(avg)*PRESS_SENS_CALC_SUMXI_300, where x(avg)=(1+2+3+...+PRESS_SENS_DATAPOINTNUM_VERYSLOW)/PRESS_SENS_DATAPOINTNUM_VERYSLOW
	#define PRESS_SENS_LIMIT_VERYSLOW                85                //velocity*1000 in mPa/timequantum. 312 means 12Pa/s @ 0.0.26s/sample
	//constants and definitions for line regression handler functions
	#define PRESS_SENS_DEVICESTARTUPTIME             200               //Number pf UP measurements until the first time it will calculates the velocity
	#define PRESS_SENS_ARRAYLENGTH                   PRESS_SENS_DATAPOINTNUM_VERYSLOW
	//definition of the stucture that is used in "PressureSense_CalculateVelocity(...)" functions
	typedef struct ps_velocalcconts{
		uint16_t datapointnum;
		uint32_t sumxi;
		uint32_t sumxi2;
		float avgtsum;
	}ps_velocalcconts;
	//initialization of the stuctarray that is used in "PressureSense_CalculateVelocity(...)" functions
	#define PRESS_SENS_VELOCALCSARRAY_INIT          static ps_velocalcconts velocalc[PRESS_SENS_STAGENUM] = \
      { {.datapointnum = PRESS_SENS_DATAPOINTNUM_FAST, .sumxi = PRESS_SENS_CALC_SUMXI_FAST, .sumxi2 = PRESS_SENS_CALC_SUMXISQUARED_FAST, .avgtsum = PRESS_SENS_CALC_XAVGTIMESSUM_FAST }, \
        {.datapointnum = PRESS_SENS_DATAPOINTNUM_SLOW, .sumxi = PRESS_SENS_CALC_SUMXI_SLOW, .sumxi2 = PRESS_SENS_CALC_SUMXISQUARED_SLOW, .avgtsum = PRESS_SENS_CALC_XAVGTIMESSUM_SLOW }, \
        {.datapointnum = PRESS_SENS_DATAPOINTNUM_VERYSLOW, .sumxi = PRESS_SENS_CALC_SUMXI_VERYSLOW, .sumxi2 = PRESS_SENS_CALC_SUMXISQUARED_VERYSLOW, .avgtsum = PRESS_SENS_CALC_XAVGTIMESSUM_VERYSLOW } }
	//declaration of the aray that is going to contain the measured pressure values
	uint32_t pressens_pressarray[PRESS_SENS_ARRAYLENGTH];
//End of Constants and definitions for line regression stages

//Constants and inicialization for controlling the Buzzer
	//Buzzer Frequency
	#define PRESS_SENS_BUZZER_PFREQ_MAX              25                 //max (1/freq)*10 of beeping (intbuzzthreshold will be adjusted according to this)
	#define PRESS_SENS_BUZZER_PFREQ_MIN              1                  //min (1/freq)*10 of beeping (intbuzzthreshold will be adjusted according to this)
	#define PRESS_SENS_MIN_SPEED                     PRESS_SENS_LIMIT_VERYSLOW
	#define PRESS_SENS_MAX_SPEED                     1720               //equal to 5m/s. v(in m/s)=PRESS_SENS_MAX_SPEED*8.43/(0.029*100*1000) at sea level
																		//where 0.029 is the sampling frequency
																		//      8.43 is the change of altitude in meter if the pressure changes 100Pa 
	#define PRESS_SENS_BUZZ_STAGES_NUM               10                 //defines the length of the array that containts the stages of the buzzer
	//velocitythresholds	
	#define PRESS_SENS_BUZZ_STAGETHRESHOLD_0         PRESS_SENS_LIMIT_VERYSLOW
	#define PRESS_SENS_BUZZ_STAGETHRESHOLD_1         200
	#define PRESS_SENS_BUZZ_STAGETHRESHOLD_2         300
	#define PRESS_SENS_BUZZ_STAGETHRESHOLD_3         400
	#define PRESS_SENS_BUZZ_STAGETHRESHOLD_4         500
	#define PRESS_SENS_BUZZ_STAGETHRESHOLD_5         600
	#define PRESS_SENS_BUZZ_STAGETHRESHOLD_6         700
	#define PRESS_SENS_BUZZ_STAGETHRESHOLD_7         800
	#define PRESS_SENS_BUZZ_STAGETHRESHOLD_8         900
	#define PRESS_SENS_BUZZ_STAGETHRESHOLD_9         1000
	//intbuzz thresholds
	#define PRESS_SENS_BUZZ_INTBUZZTHRESHOLD_1       25
	#define PRESS_SENS_BUZZ_INTBUZZTHRESHOLD_2       20
	#define PRESS_SENS_BUZZ_INTBUZZTHRESHOLD_3       16
	#define PRESS_SENS_BUZZ_INTBUZZTHRESHOLD_4       13
	#define PRESS_SENS_BUZZ_INTBUZZTHRESHOLD_5       11
	#define PRESS_SENS_BUZZ_INTBUZZTHRESHOLD_6       9
	#define PRESS_SENS_BUZZ_INTBUZZTHRESHOLD_7       7
	#define PRESS_SENS_BUZZ_INTBUZZTHRESHOLD_8       6
	#define PRESS_SENS_BUZZ_INTBUZZTHRESHOLD_9       5
	#define PRESS_SENS_BUZZ_INTBUZZTHRESHOLD_10      4
	//initialization of the arrays
	#define PRESS_SENS_STAGEARRAY_INIT               static uint16_t stagethresholds[PRESS_SENS_BUZZ_STAGES_NUM] = \
	                                                 { PRESS_SENS_BUZZ_STAGETHRESHOLD_0, \
													 PRESS_SENS_BUZZ_STAGETHRESHOLD_1, \
													 PRESS_SENS_BUZZ_STAGETHRESHOLD_2, \
													 PRESS_SENS_BUZZ_STAGETHRESHOLD_3, \
													 PRESS_SENS_BUZZ_STAGETHRESHOLD_4, \
													 PRESS_SENS_BUZZ_STAGETHRESHOLD_5, \
													 PRESS_SENS_BUZZ_STAGETHRESHOLD_6, \
													 PRESS_SENS_BUZZ_STAGETHRESHOLD_7, \
													 PRESS_SENS_BUZZ_STAGETHRESHOLD_8, \
													 PRESS_SENS_BUZZ_STAGETHRESHOLD_9 }
	#define PRESS_SENS_BUZZARRAY_INIT                static uint8_t buzzthresholds[PRESS_SENS_BUZZ_STAGES_NUM] = \
	                                                 { PRESS_SENS_BUZZ_INTBUZZTHRESHOLD_1, \
													 PRESS_SENS_BUZZ_INTBUZZTHRESHOLD_2, \
													 PRESS_SENS_BUZZ_INTBUZZTHRESHOLD_3, \
													 PRESS_SENS_BUZZ_INTBUZZTHRESHOLD_4, \
													 PRESS_SENS_BUZZ_INTBUZZTHRESHOLD_5, \
													 PRESS_SENS_BUZZ_INTBUZZTHRESHOLD_6, \
													 PRESS_SENS_BUZZ_INTBUZZTHRESHOLD_7, \
													 PRESS_SENS_BUZZ_INTBUZZTHRESHOLD_8, \
													 PRESS_SENS_BUZZ_INTBUZZTHRESHOLD_9, \
													 PRESS_SENS_BUZZ_INTBUZZTHRESHOLD_10}
//End of Constants for controlling the Buzzer




/*ps_velocalcconts VELOCALCCONTS[2] = { { .datapointnum = PRESS_SENS_DATAPOINTNUM_FAST, .sumxi = PRESS_SENS_CALC_SUMXI_FAST, \
.sumxi2 = PRESS_SENS_CALC_SUMXISQUARED_FAST, .avgtsum = PRESS_SENS_CALC_XAVGTIMESSUM_FAST }, \
{.datapointnum = PRESS_SENS_DATAPOINTNUM_SLOW, .sumxi = PRESS_SENS_CALC_SUMXI_SLOW, \
.sumxi2 = PRESS_SENS_CALC_SUMXISQUARED_SLOW, .avgtsum = PRESS_SENS_CALC_XAVGTIMESSUM_SLOW} };*/

#ifdef PRESS_SENS_DEBUGMODE
	int16_t datap[1620];
#endif
//float datav[340];
/*int16_t VELOCITYFAST[150];
int16_t VELOCITYSLOW[150];
int16_t VELOCITYVERYSLOW[150];*/

void PressureSensor_Reset();
void PressureSensor_Init();
short PressureSensor_GetRealTempr();
long PressureSensor_GetRealPressure();

void PressureSensor_GetUTIfNeeded();

void PressureSense_Main();
void PressureSense_DetermineVelocity();
void PressureSense_BuzzerControl(int32_t velocity, char positive);
int32_t PressureSense_IsPositive(int32_t velocity, char* positive);
//These functions are responsible for calculating the vertical velocity
void PressureSense_InsertData(uint32_t newdata);
uint64_t PressureSense_CalculateSumXiTYi(uint16_t length);
float PressureSense_CalculateAvgYi(uint16_t length);
int32_t PressureSense_CalculateVelocity(ps_velocalcconts* velocalcconsts);
int32_t PressureSense_CalculateVelocity_Fast();
int32_t PressureSense_CalculateVelocity_Slow();
int32_t PressureSense_CalculateVelocity_VerySlow();

void TIM14_IRQHandler();

#endif