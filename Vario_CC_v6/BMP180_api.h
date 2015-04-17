#ifndef BMP180_API_H
#define BMP180_API_H

#include "I2C_extension.h"

#define BMP180_DEV_ADDR                0xEE
#define BMP180_STARTUPTIME             100  // in ms

#define BMP180_WAIT_UT                 5   //waiting time in ms
#define BMP180_WAIT_UP_LOWPOWER        5   //...WAIT_UP values must be under 256,
#define BMP180_WAIT_UP_STANDARD        8   //   because the function "BMP180_READ_UP(...)"
#define BMP180_WAIT_UP_HIHGRES         14  //   uses them as unsigned char
#define BMP180_WAIT_UP_ULTRAHIGHRES    26

//read-write-wait functions
//the header files of the defined functions must be included
#define BMP_READ(I2Cx, device_addr, register_addr, register_data, read_length) \
	I2CLine_Read(I2Cx, device_addr, register_addr, register_data, read_length)
#define BMP_WRITE(I2Cx, device_addr, register_addr, register_data) \
	I2CLine_Write(I2Cx, device_addr, register_addr, register_data)
#define DELAY(MSEC)                    VarioCC_Delay(MSEC)
#define DELAY_NONBLOCKING(MSEC)        VarioCC_NonBlockingDelay_Start(MSEC)


//oversampling settings
#define BMP180_OSS_LOWPOWER            0
#define BMP180_OSS_STANDARD            1
#define BMP180_OSS_HIGHRES             2
#define BMP180_OSS_ULTRAHIGHRES        3

//commands
#define BMP180_COM_CTRL_MEAS_TEMP      0x2E
#define BMP180_COM_CTRL_MEAS_PRESS     0x34
#define BMP180_COM_SOFT_RESET          0xB6

//control reg bits
#define BMP180_BIT_CTRL_MEAS_SCO       (1<<5)
#define BMP180_BIT_CTRL_MEAS_SCO_SHIFT 5
#define BMP180_BIT_CTRL_MEAS_OSS       (0x3<<6)
#define BMP180_BIT_CTRL_MEAS_OSS_SHIFT 6

//register values
#define BMP180_VALUE_ID                0x55

#define BMP180_CALREGS_BASE_ADDRESS    0xAA
#define BMP180_CTRLREGS_BASE_ADDRESS   0xF4

//addresses of control registers
#define BMP180_ADDR_OUT_XLSB           0xF8
#define BMP180_ADDR_OUT_LSB            0xF7
#define BMP180_ADDR_OUT_MSB            0xF6
#define BMP180_ADDR_CTRL_MEAS          0xF4
#define BMP180_ADDR_SOFT_RESET         0xE0
#define BMP180_ADDR_ID                 0xD0

//calibration registers
typedef struct BMP180_CAL_TypeDef{
	//integrated registers
	short AC1;
	short AC2;
	short AC3;
	unsigned short AC4;
	unsigned short AC5;
	unsigned short AC6;
	short B1;
	short B2;
	short MB;
	short MC;
	short MD;
	//calculated parameters
	long B5;
}BMP180_CAL_TypeDef;

typedef struct BMP180_CTRL_TypeDef{
	unsigned char CTRL;
	unsigned char DUMP;
	unsigned char MSB;
	unsigned char LSB;
	unsigned char XLSB;
}BMP180_CTRL_TypeDef;

//#define BMP180_CALREGS                 ((BMP180_CAL_TypeDef*) BMP180_CALREGS_BASE_ADDRESS)
//#define BMP180_CTRLREGS                ((BMP180_CTRL_TypeDef*) BMP180_CTRLREGS_BASE_ADDRESS)

volatile BMP180_CAL_TypeDef                     BMP180_CALREGS;
volatile BMP180_CTRL_TypeDef                    BMP180_CTRLREGS;

void BMP180_READ_CALDATA();
void BMP180_READ_UT();
void BMP180_READ_UP_START(unsigned char oss);
void BMP180_READ_UP_END();
short BMP180_CALC_TEMP();
long BMP180_CALC_PRESS(unsigned char oss);

#endif