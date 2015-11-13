#ifndef I2C_EXTENSION_H_
#define I2C_EXTENSION_H_

#include "Vario_CC_board.h"
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_i2c.h"

#define I2C_TIMEOUT_MAX        500
#define SUCCESSFUL             0
#define FAILED                 1

#define GPIO_RCC(NEWSTATE)     I2C_RCC_GPIO_CMD(NEWSTATE)
#define GPIO_I2C               I2C_GPIO_SCL
#define PIN_SDA                I2C_PIN_SDA
#define PIN_SCL                I2C_PIN_SCL
#define GPIO_SPEED             GPIO_SPEED_USED
#define ALTERNATE_FUNC_SCL     GPIO_AF_4
#define ALTERNATE_FUNC_SDA     GPIO_AF_4
#define PINSOURCE_SCL          I2C_PINSOURCE_SCL
#define PINSOURCE_SDA          I2C_PINSOURCE_SDA

#define I2C_RCC(NEWSTATE)      I2C_RCC_I2C_CMD(NEWSTATE)
#define REG_TIMINGR            I2C_TIMINGSETUP    // calculated by the app of stm in the application note AN4235
                                                  // must be recalculated every time you change the frequency of the clock source
#define I2C_USED               I2C_I2C_USED

void I2CLine_GPIOInit();
void I2CLine_I2CInit();
void I2CLine_Init();

void I2CLine_Read(I2C_TypeDef* I2Cx, unsigned char device_addr, unsigned char reg_addr, unsigned char reg_data[], unsigned char cnt);
void I2CLine_Write(I2C_TypeDef* I2Cx, unsigned char device_addr, unsigned char reg_addr, unsigned char reg_data);

#endif