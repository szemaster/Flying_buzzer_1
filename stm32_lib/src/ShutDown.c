#include "ShutDown.h"

//initializes the input and output pins, defined in Vario_CC_board.h
//- enables clk
//- initializes the pins
//- enables power supply
void SHD_Init(){
	GPIO_InitTypeDef initdef;
	SHD_RCC(ENABLE);
	GPIO_StructInit(&initdef);
	initdef.GPIO_Mode=GPIO_Mode_OUT;
	initdef.GPIO_Pin = SHD_PIN_EN;
	initdef.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(SHD_GPIO, &initdef);
	GPIO_SetBits(SHD_GPIO, SHD_PIN_EN);
	GPIO_StructInit(&initdef);
	initdef.GPIO_Mode=GPIO_Mode_IN;
	initdef.GPIO_PuPd=GPIO_PuPd_NOPULL;
	initdef.GPIO_Pin=SHD_PIN_READ;
	GPIO_Init(SHD_GPIO, &initdef);
}

//decides if it'd time to turn off the power supply or not
//- pollings id the pushbutton is pressed or not
//- if it is pressed, starts to count time
//- if it is pressed for a certain time (defined in ShutDown.h)
//  it turns on the LED for a while (the LED and the time are defined in ShutDown.h)
//  the disables the power supply
void SHD_Main(){
	static uint8_t machinestate=0; //0=turning on; 1=already turned on and running
	static uint32_t counter=COUNTER_RVALUE;
	volatile static uint32_t counterled = COUNTER_LEDTIME;
	while(GPIO_ReadInputDataBit(SHD_GPIO, SHD_PIN_READ)){
	   if(machinestate==1)
	    	counter--;
	   if(counter==0){             //turning off sequence

		   //for(;counterled != 0;counterled--);
		   SHUTDOWN;
		   SHD_LEDON;
		   while(1){
			   asm("nop");
		   }
	   }
	}
	if(!GPIO_ReadInputDataBit(SHD_GPIO, SHD_PIN_READ)){
	   machinestate=1;
	   counter=COUNTER_RVALUE;
	}
}
