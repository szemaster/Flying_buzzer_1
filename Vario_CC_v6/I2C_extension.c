#include "I2C_extension.h"

void I2CLine_Init(I2C_TypeDef* I2Cx){
	I2CLine_GPIOInit();
	I2CLine_I2CInit(I2Cx);
}

//initializes the GPIOs for the I2C port
//  - enables periphclock for GPIOs that are used
//  - sets GPIOs as alternate function pins
//pins, port, alternatefunction are define in I2C_extension.h
void I2CLine_GPIOInit(){
	GPIO_InitTypeDef gpioinitstruct;

	GPIO_RCC(ENABLE);

	gpioinitstruct.GPIO_Pin = (PIN_SCL | PIN_SDA);
	gpioinitstruct.GPIO_Mode = GPIO_Mode_AF;
	gpioinitstruct.GPIO_OType = GPIO_OType_OD;
	gpioinitstruct.GPIO_PuPd = GPIO_PuPd_UP;
	gpioinitstruct.GPIO_Speed = GPIO_SPEED;
	GPIO_Init(GPIO_I2C, &gpioinitstruct);
	GPIO_PinAFConfig(GPIO_I2C, PINSOURCE_SCL, ALTERNATE_FUNC_SCL);
	GPIO_PinAFConfig(GPIO_I2C, PINSOURCE_SDA, ALTERNATE_FUNC_SDA);
}

//initializes the I2C port
//  - enables periphclock for I2C that is used
//  - initializes I2C port
//  - enables I2C port
//input: I2Cx: I2C1 or I2C2
//NOTE: REG_TIMINGR REGISTER MUST BE RECALCULATED EVERY TIME YOU CHANGE ANY SETTINGS OF THE I2C COMMUNICATION
//I2C_RCC() and REG_TIMINGR are defined in I2C_extension.h
void I2CLine_I2CInit(I2C_TypeDef* I2Cx){
	I2C_InitTypeDef i2cinitstruct;

	I2C_RCC(ENABLE);

	I2C_StructInit(&i2cinitstruct);
	i2cinitstruct.I2C_Timing = REG_TIMINGR;
	i2cinitstruct.I2C_Ack = I2C_Ack_Enable;
	I2C_Init(I2Cx, &i2cinitstruct);
	I2C_Cmd(I2Cx, ENABLE);
}

//reads the register values of the periphery started from reg_addr "cnt" long
//input: - I2Cx: I2C1 or I2C2
//       - device_addr: the 7bit long address of the periphery shifted to the left by one
//                      if the device address is 0b1110111, than device_addr=0xEE
//       - reg_addr: the starting point of the reading
//       - reg_data[]: this array is going to contain the readed bytes
//       - cnt: this long will the function read the memory of the periphery
//              if reg_addr=0xAA, and cnt=2, than the function will read the 0xAA and 0xAB bytes of the memory
void I2CLine_Read(I2C_TypeDef* I2Cx, unsigned char device_addr, unsigned char reg_addr, unsigned char reg_data[], unsigned char cnt){
	uint8_t i = 0;
	while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY) == SET){;}
	I2C_TransferHandling(I2Cx, device_addr, 1, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);
	while (!I2C_GetFlagStatus(I2Cx, I2C_FLAG_TXE)){;}
	I2C_SendData(I2Cx, reg_addr);
	while (!I2C_GetFlagStatus(I2Cx, I2C_FLAG_TC)){;}
	I2C_TransferHandling(I2Cx, device_addr, cnt, I2C_AutoEnd_Mode, I2C_Generate_Start_Read);
	
	for (; cnt > i; i++){
		while (!I2C_GetFlagStatus(I2Cx, I2C_FLAG_RXNE)){;}
		reg_data[i] = I2C_ReceiveData(I2Cx);
	}
}

//writes the register values of the periphery started from reg_addr
//input: - I2Cx: I2C1 or I2C2
//       - device_addr: the 7bit long address of the periphery shifted to the left by one
//                      if the device address is 0b1110111, than device_addr=0xEE
//       - reg_addr: the starting point of the reading
//       - reg_data: this byte is going to be written in the periphery
void I2CLine_Write(I2C_TypeDef* I2Cx, unsigned char device_addr, unsigned char reg_addr, unsigned char reg_data){
	while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY) == SET){;}
	I2C_TransferHandling(I2Cx, device_addr, 2, I2C_AutoEnd_Mode, I2C_Generate_Start_Write);
	while (!I2C_GetFlagStatus(I2Cx, I2C_FLAG_TXE)){;}
	I2C_SendData(I2Cx, reg_addr);
	while (!I2C_GetFlagStatus(I2Cx, I2C_FLAG_TXE)){;}
	I2C_SendData(I2Cx, reg_data);
	while (!I2C_GetFlagStatus(I2Cx, I2C_FLAG_STOPF)){;}
	I2C_ClearFlag(I2Cx, I2C_FLAG_STOPF);
}