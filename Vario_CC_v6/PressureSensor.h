#ifndef PRESSURESENSOR_H_
#define PRESSURESENSOR_H_

#include "Vario_CC_board.h"
#include "BMP180_api.h"
#include "Buzzer.h"

//#define PRESS_SENS_DEBUGMODE

//Functions and constants came from the API of the pressure sensor
#define PRESS_SENS_READ_CALIBR_DATA()            BMP180_READ_CALDATA()
#define	PRESS_SENS_GET_UTEMPR()                  BMP180_READ_UT()
#define PRESS_SENS_CALC_REALTEMP(UT)             BMP180_CALC_TEMP(UT)
#define PRESS_SENS_GET_UPRESS(OSS)               BMP180_READ_UP_END(OSS)
#define PRESS_SENS_CALC_REALPRESSURE(OSS, UP)    BMP180_CALC_PRESS(OSS, UP)
#define PRESS_SENS_START_UPMEAS(OSS)             BMP180_READ_UP_START(OSS)

#define PRESS_SENS_RESOLUTION                    BMP180_OSS_ULTRAHIGHRES
#define PRESS_SENS_RESET_ADDR                    BMP180_ADDR_SOFT_RESET
#define PRESS_SENS_RESET_COMMAND                 BMP180_COM_SOFT_RESET
#define PRESS_SENS_DEV_ADDR                      BMP180_DEV_ADDR
#define PRESS_SENS_STARTUPTIME                   BMP180_STARTUPTIME
//End of functions and constants came from the API of the pressure sensor

//Constants and definitions for line regression stages
#define PRESS_SENS_FREQOFTEMPRMEASUREMENT        0
//constants that specifies the way of altitude regression
#define PRESS_SENS_STAGENUM                      3
//stage veryfast 
#define PRESS_SENS_DATAPOINTNUM_VERYFAST         35
#define PRESS_SENS_CALC_SUMXI_VERYFAST           630               //from 1 to "PRESS_SENS_DATAPOINTNUM_FAST" sum(x)
#define PRESS_SENS_CALC_SUMXISQUARED_VERYFAST    14910             //from 1 to "PRESS_SENS_DATAPOINTNUM_FAST" sum(x^2)
#define PRESS_SENS_CALC_XAVGTIMESSUM_VERYFAST    11340             //x(avg)*PRESS_SENS_CALC_SUMXI_60, where x(avg)=(1+2+3+...+PRESS_SENS_DATAPOINTNUM_FAST)/PRESS_SENS_DATAPOINTNUM_FAST
#define PRESS_SENS_LIMIT_VERYFAST                700               //velocity*1000 in mPa/timequantum. 312 means 12Pa/s @ 0.0.26s/sample
//stage fast 
#define PRESS_SENS_DATAPOINTNUM_FAST             55
#define PRESS_SENS_CALC_SUMXI_FAST               1540              //from 1 to "PRESS_SENS_DATAPOINTNUM_FAST" sum(x)
#define PRESS_SENS_CALC_SUMXISQUARED_FAST        56980             //from 1 to "PRESS_SENS_DATAPOINTNUM_FAST" sum(x^2)
#define PRESS_SENS_CALC_XAVGTIMESSUM_FAST        43120             //x(avg)*PRESS_SENS_CALC_SUMXI_60, where x(avg)=(1+2+3+...+PRESS_SENS_DATAPOINTNUM_FAST)/PRESS_SENS_DATAPOINTNUM_FAST
#define PRESS_SENS_LIMIT_FAST                    315               //velocity*1000 in mPa/timequantum. 312 means 12Pa/s @ 0.0.26s/sample
//stage normal 
#define PRESS_SENS_DATAPOINTNUM_NORMAL           70
#define PRESS_SENS_CALC_SUMXI_NORMAL             2485              //from 1 to "PRESS_SENS_DATAPOINTNUM_FAST" sum(x)
#define PRESS_SENS_CALC_SUMXISQUARED_NORMAL      116795            //from 1 to "PRESS_SENS_DATAPOINTNUM_FAST" sum(x^2)
#define PRESS_SENS_CALC_XAVGTIMESSUM_NORMAL      88218             //x(avg)*PRESS_SENS_CALC_SUMXI_60, where x(avg)=(1+2+3+...+PRESS_SENS_DATAPOINTNUM_FAST)/PRESS_SENS_DATAPOINTNUM_FAST
#define PRESS_SENS_LIMIT_NORMAL                  245               //velocity*1000 in mPa/timequantum. 312 means 12Pa/s @ 0.0.26s/sample
//stage slow
#define PRESS_SENS_DATAPOINTNUM_SLOW             105
#define PRESS_SENS_CALC_SUMXI_SLOW               5565              //from 1 to "PRESS_SENS_DATAPOINTNUM_SLOW" sum(x)
#define PRESS_SENS_CALC_SUMXISQUARED_SLOW        391405            //from 1 to "PRESS_SENS_DATAPOINTNUM_SLOW" sum(x^2)
#define PRESS_SENS_CALC_XAVGTIMESSUM_SLOW        294945            //x(avg)*PRESS_SENS_CALC_SUMXI_150, where x(avg)=(1+2+3+...+PRESS_SENS_DATAPOINTNUM_SLOW)/PRESS_SENS_DATAPOINTNUM_SLOW
#define PRESS_SENS_LIMIT_SLOW                    140               //velocity*1000 in mPa/timequantum. 154 means 4Pa/s @ 0.0.26s/sample
//stage veryslow
#define PRESS_SENS_DATAPOINTNUM_VERYSLOW         140
#define PRESS_SENS_CALC_SUMXI_VERYSLOW           9870              //from 1 to "PRESS_SENS_DATAPOINTNUM_VERYSLOW" sum(x)
#define PRESS_SENS_CALC_SUMXISQUARED_VERYSLOW    924490            //from 1 to "PRESS_SENS_DATAPOINTNUM_VERYSLOW" sum(x^2)
#define PRESS_SENS_CALC_XAVGTIMESSUM_VERYSLOW    695835            //x(avg)*PRESS_SENS_CALC_SUMXI_300, where x(avg)=(1+2+3+...+PRESS_SENS_DATAPOINTNUM_VERYSLOW)/PRESS_SENS_DATAPOINTNUM_VERYSLOW
#define PRESS_SENS_LIMIT_VERYSLOW                105               //velocity*1000 in mPa/timequantum. 312 means 12Pa/s @ 0.0.26s/sample
//constants and definitions for line regression handler functions
#define PRESS_SENS_DEVICESTARTUPTIME             200               //Number pf UP measurements until the first time it will calculates the velocity
#define PRESS_SENS_ARRAYLENGTH                   PRESS_SENS_DATAPOINTNUM_VERYSLOW
//definition of the stucture that is used in "PressureSense_CalculateVelocity(...)" functions
typedef struct ps_velocalcconts{
	uint16_t datapointnum;
	uint32_t sumxi;
	uint32_t sumxi2;
	float avgtsum;
	uint16_t limit;
}ps_velocalcconts;
//initialization of the stuctarray that is used in "PressureSense_CalculateVelocity(...)" functions
#define PRESS_SENS_VELOCALCSARRAY_INIT          static ps_velocalcconts velocalc[PRESS_SENS_STAGENUM] = \
                   {/*{ .datapointnum = PRESS_SENS_DATAPOINTNUM_VERYFAST, .sumxi = PRESS_SENS_CALC_SUMXI_VERYFAST, .sumxi2 = PRESS_SENS_CALC_SUMXISQUARED_VERYFAST, .avgtsum = PRESS_SENS_CALC_XAVGTIMESSUM_VERYFAST, .limit = PRESS_SENS_LIMIT_VERYFAST },*/ \
                    {.datapointnum = PRESS_SENS_DATAPOINTNUM_FAST, .sumxi = PRESS_SENS_CALC_SUMXI_FAST, .sumxi2 = PRESS_SENS_CALC_SUMXISQUARED_FAST, .avgtsum = PRESS_SENS_CALC_XAVGTIMESSUM_FAST, .limit = PRESS_SENS_LIMIT_FAST }, \
                   /* {.datapointnum = PRESS_SENS_DATAPOINTNUM_NORMAL, .sumxi = PRESS_SENS_CALC_SUMXI_NORMAL, .sumxi2 = PRESS_SENS_CALC_SUMXISQUARED_NORMAL, .avgtsum = PRESS_SENS_CALC_XAVGTIMESSUM_NORMAL, .limit=PRESS_SENS_LIMIT_NORMAL },*/ \
                    {.datapointnum = PRESS_SENS_DATAPOINTNUM_SLOW, .sumxi = PRESS_SENS_CALC_SUMXI_SLOW, .sumxi2 = PRESS_SENS_CALC_SUMXISQUARED_SLOW, .avgtsum = PRESS_SENS_CALC_XAVGTIMESSUM_SLOW, .limit = PRESS_SENS_LIMIT_SLOW}, \
                    {.datapointnum = PRESS_SENS_DATAPOINTNUM_VERYSLOW, .sumxi = PRESS_SENS_CALC_SUMXI_VERYSLOW, .sumxi2 = PRESS_SENS_CALC_SUMXISQUARED_VERYSLOW, .avgtsum = PRESS_SENS_CALC_XAVGTIMESSUM_VERYSLOW, .limit = PRESS_SENS_LIMIT_VERYSLOW } }
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
#define PRESS_SENS_BUZZ_STAGES_NUM               18                 //defines the length of the array that containts the stages of the buzzer
//velocitythresholds	
#define PRESS_SENS_BUZZ_STAGETHRESHOLD_0         PRESS_SENS_LIMIT_VERYSLOW
#define PRESS_SENS_BUZZ_STAGETHRESHOLD_1         150
#define PRESS_SENS_BUZZ_STAGETHRESHOLD_2         200
#define PRESS_SENS_BUZZ_STAGETHRESHOLD_3         250
#define PRESS_SENS_BUZZ_STAGETHRESHOLD_4         300
#define PRESS_SENS_BUZZ_STAGETHRESHOLD_5         350
#define PRESS_SENS_BUZZ_STAGETHRESHOLD_6         400
#define PRESS_SENS_BUZZ_STAGETHRESHOLD_7         460
#define PRESS_SENS_BUZZ_STAGETHRESHOLD_8         500
#define PRESS_SENS_BUZZ_STAGETHRESHOLD_9         550
#define PRESS_SENS_BUZZ_STAGETHRESHOLD_10        600
#define PRESS_SENS_BUZZ_STAGETHRESHOLD_11        700
#define PRESS_SENS_BUZZ_STAGETHRESHOLD_12        750
#define PRESS_SENS_BUZZ_STAGETHRESHOLD_13        800
#define PRESS_SENS_BUZZ_STAGETHRESHOLD_14        850
#define PRESS_SENS_BUZZ_STAGETHRESHOLD_15        900
#define PRESS_SENS_BUZZ_STAGETHRESHOLD_16        950
#define PRESS_SENS_BUZZ_STAGETHRESHOLD_17        1000
//intbuzz thresholds
#define PRESS_SENS_BUZZ_INTBUZZTHRESHOLD_1       2500
#define PRESS_SENS_BUZZ_INTBUZZTHRESHOLD_2       2200
#define PRESS_SENS_BUZZ_INTBUZZTHRESHOLD_3       2000
#define PRESS_SENS_BUZZ_INTBUZZTHRESHOLD_4       1800
#define PRESS_SENS_BUZZ_INTBUZZTHRESHOLD_5       1600
#define PRESS_SENS_BUZZ_INTBUZZTHRESHOLD_6       1450
#define PRESS_SENS_BUZZ_INTBUZZTHRESHOLD_7       1300
#define PRESS_SENS_BUZZ_INTBUZZTHRESHOLD_8       1150
#define PRESS_SENS_BUZZ_INTBUZZTHRESHOLD_9       1050
#define PRESS_SENS_BUZZ_INTBUZZTHRESHOLD_10      950
#define PRESS_SENS_BUZZ_INTBUZZTHRESHOLD_11      850
#define PRESS_SENS_BUZZ_INTBUZZTHRESHOLD_12      700
#define PRESS_SENS_BUZZ_INTBUZZTHRESHOLD_13      650
#define PRESS_SENS_BUZZ_INTBUZZTHRESHOLD_14      600
#define PRESS_SENS_BUZZ_INTBUZZTHRESHOLD_15      550
#define PRESS_SENS_BUZZ_INTBUZZTHRESHOLD_16      500
#define PRESS_SENS_BUZZ_INTBUZZTHRESHOLD_17      450
#define PRESS_SENS_BUZZ_INTBUZZTHRESHOLD_18      400
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
												   PRESS_SENS_BUZZ_STAGETHRESHOLD_9, \
												   PRESS_SENS_BUZZ_STAGETHRESHOLD_10, \
                                                   PRESS_SENS_BUZZ_STAGETHRESHOLD_11, \
												   PRESS_SENS_BUZZ_STAGETHRESHOLD_12, \
												   PRESS_SENS_BUZZ_STAGETHRESHOLD_13, \
												   PRESS_SENS_BUZZ_STAGETHRESHOLD_14, \
												   PRESS_SENS_BUZZ_STAGETHRESHOLD_15, \
												   PRESS_SENS_BUZZ_STAGETHRESHOLD_16, \
												   PRESS_SENS_BUZZ_STAGETHRESHOLD_17}
#define PRESS_SENS_BUZZARRAY_INIT                static uint16_t buzzthresholds[PRESS_SENS_BUZZ_STAGES_NUM] = \
                                                 { PRESS_SENS_BUZZ_INTBUZZTHRESHOLD_1, \
	                                               PRESS_SENS_BUZZ_INTBUZZTHRESHOLD_2, \
												   PRESS_SENS_BUZZ_INTBUZZTHRESHOLD_3, \
												   PRESS_SENS_BUZZ_INTBUZZTHRESHOLD_4, \
												   PRESS_SENS_BUZZ_INTBUZZTHRESHOLD_5, \
												   PRESS_SENS_BUZZ_INTBUZZTHRESHOLD_6, \
												   PRESS_SENS_BUZZ_INTBUZZTHRESHOLD_7, \
												   PRESS_SENS_BUZZ_INTBUZZTHRESHOLD_8, \
												   PRESS_SENS_BUZZ_INTBUZZTHRESHOLD_9, \
												   PRESS_SENS_BUZZ_INTBUZZTHRESHOLD_10, \
												   PRESS_SENS_BUZZ_INTBUZZTHRESHOLD_11, \
												   PRESS_SENS_BUZZ_INTBUZZTHRESHOLD_12, \
												   PRESS_SENS_BUZZ_INTBUZZTHRESHOLD_13, \
												   PRESS_SENS_BUZZ_INTBUZZTHRESHOLD_14, \
												   PRESS_SENS_BUZZ_INTBUZZTHRESHOLD_15, \
												   PRESS_SENS_BUZZ_INTBUZZTHRESHOLD_16, \
												   PRESS_SENS_BUZZ_INTBUZZTHRESHOLD_17, \
												   PRESS_SENS_BUZZ_INTBUZZTHRESHOLD_18}
//End of Constants for controlling the Buzzer



//#ifdef PRESS_SENS_DEBUGMODE
//int16_t datap[1300];
//#endif

//float datav[340];
int16_t VELOCITYVERYFAST[150];
int16_t VELOCITYFAST[150];
int16_t VELOCITYNORMAL[150];
int16_t VELOCITYSLOW[150];
int16_t VELOCITYVERYSLOW[150];

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

//void TIM14_IRQHandler();

#endif