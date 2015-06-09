#include "BMP180_api.h"

//reads and stores the calibration register values
//the function stores the calibration data into "BMP180_CALREGS" (defined in BMP180_api.h)
void BMP180_READ_CALDATA(){
	unsigned char data[22];

	//reading out the calibration registers
	BMP_READ(I2C_USED, BMP180_DEV_ADDR, BMP180_CALREGS_BASE_ADDRESS, data, 22);

	/*parameters AC1-AC6*/
	BMP180_CALREGS.AC1 = (data[0] << 8) | data[1];
	BMP180_CALREGS.AC2 = (data[2] << 8) | data[3];
	BMP180_CALREGS.AC3 = (data[4] << 8) | data[5];
	BMP180_CALREGS.AC4 = (data[6] << 8) | data[7];
	BMP180_CALREGS.AC5 = (data[8] << 8) | data[9];
	BMP180_CALREGS.AC6 = (data[10] << 8) | data[11];

	/*parameters B1,B2*/
	BMP180_CALREGS.B1 = (data[12] << 8) | data[13];
	BMP180_CALREGS.B2 = (data[14] << 8) | data[15];

	/*parameters MB,MC,MD*/
	BMP180_CALREGS.MB = (data[16] << 8) | data[17];
	BMP180_CALREGS.MC = (data[18] << 8) | data[19];
	BMP180_CALREGS.MD = (data[20] << 8) | data[21];
}

//Reads out the uncompensated temperature value
//the uncompensated temperature value will be stored in BMP180_CTRLREGS.MSB and BMP180_CTRLREGS.LSB (defined in BMP180_api.h)
unsigned long BMP180_READ_UT(){
	unsigned char data[2];

	BMP_WRITE(I2C_USED, BMP180_DEV_ADDR, BMP180_ADDR_CTRL_MEAS, BMP180_COM_CTRL_MEAS_TEMP);

	DELAY(BMP180_WAIT_UT);

	BMP_READ(I2C_USED, BMP180_DEV_ADDR, BMP180_ADDR_OUT_MSB, data, 2);
	BMP180_CTRLREGS.MSB = data[0];
	BMP180_CTRLREGS.LSB = data[1];
	return (BMP180_CTRLREGS.MSB << 8) | BMP180_CTRLREGS.LSB;
}

//calculates valid temperature from uncompnsated temperature
//output: short, the calculated temperature in 0.1C°
short BMP180_CALC_TEMP(unsigned long ut){
	short temperature;
//	unsigned long ut;
	long x1, x2;
//	static long calregb5old = 4500;

	//ut = (BMP180_CTRLREGS.MSB << 8) | BMP180_CTRLREGS.LSB;

	x1 = (((long)ut - (long)BMP180_CALREGS.AC6) * (long)BMP180_CALREGS.AC5) >> 15;
	x2 = ((long)BMP180_CALREGS.MC << 11) / (x1 + BMP180_CALREGS.MD);
	BMP180_CALREGS.B5 = x1 + x2;
	temperature = (BMP180_CALREGS.B5 + 8) >> 4;

//	BMP180_CALREGS.B5 = (BMP180_CALREGS.B5 + calregb5old) / 2;
//	calregb5old = x1 + x2;

	return temperature;
}

//The function starts a conversion for pressure measurement
//If you define "DELAY_NONBLOCKING(...)" as a timer that uses interrupt
//    then you can do anything else, while the sensor executes the conversion
//To get the results of this conversion, use "BMP180_READ_UP_END()" function
//input: oss: specifies the resoluion mode
//            BMP180_OSS_LOWPOWER; BMP180_OSS_STANDARD; BMP180_OSS_HIGHRES; BMP180_OSS_ULTRAHIGHRES
void BMP180_READ_UP_START(unsigned char oss){
	BMP_WRITE(I2C_USED, BMP180_DEV_ADDR, BMP180_ADDR_CTRL_MEAS, BMP180_COM_CTRL_MEAS_PRESS | (oss << BMP180_BIT_CTRL_MEAS_OSS_SHIFT));
	
	switch (oss){
	case BMP180_OSS_LOWPOWER:
		DELAY_NONBLOCKING(BMP180_WAIT_UP_LOWPOWER);
		break;
	case BMP180_OSS_STANDARD:
		DELAY_NONBLOCKING(BMP180_WAIT_UP_STANDARD);
		break;
	case BMP180_OSS_HIGHRES:
		DELAY_NONBLOCKING(BMP180_WAIT_UP_HIHGRES);
		break;
	case BMP180_OSS_ULTRAHIGHRES:
		DELAY_NONBLOCKING(BMP180_WAIT_UP_ULTRAHIGHRES);
		break;
	}
}

//The funcion gets the result of the last pressureconversion.
//The uncompensated pressure value will be stored in BMP180_CTRLREGS.MSB, BMP180_CTRLREGS.LSB and BMP180_CTRLREGS.XLSB (defined in BMP180_api.h)
unsigned long BMP180_READ_UP_END(unsigned char oss){
	volatile unsigned char data[3];
	static int y = 0;
		BMP_READ(I2C_USED, BMP180_DEV_ADDR, BMP180_ADDR_OUT_MSB, data, 3);
		BMP180_CTRLREGS.MSB = data[0];
		BMP180_CTRLREGS.LSB = data[1];
		BMP180_CTRLREGS.XLSB = data[2];
		return ((BMP180_CTRLREGS.MSB << 16) | (BMP180_CTRLREGS.LSB << 8) | BMP180_CTRLREGS.XLSB) >> (8 - oss);
}


//calculates valid pressure from uncompnsated pressure
//input: - unsigned char oss
//             typical values of oss:     BMP180_OSS_LOWPOWER
//                                        BMP180_OSS_STANDARD
//                                        BMP180_OSS_HIGHRES
//                                        BMP180_OSS_ULTRAHIGHRES
//output: long, the calculated pressure in Pa
int32_t BMP180_CALC_PRESS(unsigned char oss, unsigned long up){
	long pressure, x1, x2, x3, b3, b6;
	unsigned long b4, b7/*, up*/;
	
//	up = ((BMP180_CTRLREGS.MSB << 16) | (BMP180_CTRLREGS.LSB << 8) | BMP180_CTRLREGS.XLSB) >> (8 - oss);
	b6 = BMP180_CALREGS.B5 - 4000;

	/*****calculate B3************/
	x1 = (b6*b6) >> 12;
	x1 *= BMP180_CALREGS.B2;
	x1 >>= 11;
	x2 = (BMP180_CALREGS.AC2*b6);
	x2 >>= 11;
	x3 = x1 + x2;
	b3 = ((((((long)BMP180_CALREGS.AC1) << 2) + x3) << oss) + 2) >> 2;

	/*****calculate B4************/
	x1 = (BMP180_CALREGS.AC3 * b6) >> 13;
	x2 = (BMP180_CALREGS.B1 * ((b6*b6) >> 12)) >> 16;
	x3 = ((x1 + x2) + 2) >> 2;
	b4 = (BMP180_CALREGS.AC4 * (unsigned long)(x3 + 32768)) >> 15;
	b7 = ((unsigned long)(up - b3) * (50000 >> oss));
	if (b7 < 0x80000000)
		pressure = (b7 << 1) / b4;
	else
		pressure = (b7 / b4) << 1;
	x1 = pressure >> 8;
	x1 *= x1;
	x1 = (x1 * 3038) >> 16;
	x2 = (pressure * -7357) >> 16;
	pressure += (x1 + x2 + 3791) >> 4;/* pressure in Pa*/
	return pressure;
}
