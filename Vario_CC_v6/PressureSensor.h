#ifndef PRESSURESENSOR_H_
#define PRESSURESENSOR_H_

#include "Vario_CC_board.h"
#include "BMP180_api.h"
#include "Buzzer.h"


//functions and constants came from the API of the pressure sensor
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

#define PRESS_SENS_FREQOFTEMPRMEASUREMENT        15
//constants that specifies the way of altitude averaging and so on
#define PRESS_SENS_DATAPOINTNUM_FAST             75
#define PRESS_SENS_CALC_SUMXI_FAST               2850              //from 1 to "PRESS_SENS_DATAPOINTNUM_FAST" sum(x)
#define PRESS_SENS_CALC_SUMXISQUARED_FAST        143450            //from 1 to "PRESS_SENS_DATAPOINTNUM_FAST" sum(x^2)
#define PRESS_SENS_CALC_XAVGTIMESSUM_FAST        108300            //x(avg)*PRESS_SENS_CALC_SUMXI_60, where x(avg)=(1+2+3+...+PRESS_SENS_DATAPOINTNUM_FAST)/PRESS_SENS_DATAPOINTNUM_FAST
#define PRESS_SENS_LIMIT_FAST                    (float)0.2
//uint32_t pressens_pressarray_fast[PRESS_SENS_DATAPOINTNUM_FAST];
#define PRESS_SENS_DATAPOINTNUM_SLOW             150
#define PRESS_SENS_CALC_SUMXI_SLOW               11325             //from 1 to "PRESS_SENS_DATAPOINTNUM_SLOW" sum(x)
#define PRESS_SENS_CALC_SUMXISQUARED_SLOW        1136275           //from 1 to "PRESS_SENS_DATAPOINTNUM_SLOW" sum(x^2)
#define PRESS_SENS_CALC_XAVGTIMESSUM_SLOW        855037.5          //x(avg)*PRESS_SENS_CALC_SUMXI_150, where x(avg)=(1+2+3+...+PRESS_SENS_DATAPOINTNUM_SLOW)/PRESS_SENS_DATAPOINTNUM_SLOW
#define PRESS_SENS_LIMIT_SLOW                    (float)0.1
//uint32_t pressens_pressarray_slow[PRESS_SENS_DATAPOINTNUM_SLOW];
#define PRESS_SENS_DATAPOINTNUM_VERYSLOW         300
#define PRESS_SENS_CALC_SUMXI_VERYSLOW           45150             //from 1 to "PRESS_SENS_DATAPOINTNUM_VERYSLOW" sum(x)
#define PRESS_SENS_CALC_SUMXISQUARED_VERYSLOW    9045050           //from 1 to "PRESS_SENS_DATAPOINTNUM_VERYSLOW" sum(x^2)
#define PRESS_SENS_CALC_XAVGTIMESSUM_VERYSLOW    6795075           //x(avg)*PRESS_SENS_CALC_SUMXI_300, where x(avg)=(1+2+3+...+PRESS_SENS_DATAPOINTNUM_VERYSLOW)/PRESS_SENS_DATAPOINTNUM_VERYSLOW
#define PRESS_SENS_LIMIT_SLOW                    (float)0.08

#define PRESS_SENS_ARRAYLENGTH                   PRESS_SENS_DATAPOINTNUM_VERYSLOW
uint32_t pressens_pressarray[PRESS_SENS_ARRAYLENGTH];

//long datap[400];
//float datav[340];
int16_t VELOCITY[1300];

void PressureSensor_Reset();
void PressureSensor_Init();
short PressureSensor_GetRealTempr();
long PressureSensor_GetRealPressure();

void PressureSensor_GetUTIfNeeded();

void PressureSense_Main();
//These functions are responsible for calculating the vertical velocity
void PressureSense_InsertData(uint32_t newdata);
uint64_t PressureSense_CalculateSumXiTYi(uint16_t length);
float PressureSense_CalculateAvgYi(uint16_t length);
float PressureSense_CalculateVelocity_Fast();
float PressureSense_CalculateVelocity_Slow();
float PressureSense_CalculateVelocity_VerySlow();

#endif