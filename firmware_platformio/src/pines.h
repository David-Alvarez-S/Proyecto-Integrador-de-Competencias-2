#ifndef BOARD_PINS_H
#define BOARD_PINS_H

#include "driver/gpio.h"
#include "driver/spi_master.h"

//==========================
//-------- Bus SPI --------- 
//==========================
#define PIN_SPI_MOSI        GPIO_NUM_23
#define PIN_SPI_CLK         GPIO_NUM_18
#define PIN_SR_CS           GPIO_NUM_26
#define PIN_SR_OE           GPIO_NUM_4 
#define SPI_SHARED_HOST     SPI2_HOST
#define PIN_TFT_DC          GPIO_NUM_14
#define PIN_TFT_CS          GPIO_NUM_27
#define PIN_SPI_MISO        GPIO_NUM_19
#define PIN_TFT_RST         GPIO_NUM_33
#define PIN_TOUCH_CS        GPIO_NUM_12
#define PIN_TOUCH_IRQ       GPIO_NUM_35 
//=====================================
//--- Sensores Ultrasónicos HC-SR04 --- 
//=====================================
#define PIN_US1_TRIG        GPIO_NUM_13 
#define PIN_US1_ECHO        GPIO_NUM_39   
#define PIN_US2_TRIG        GPIO_NUM_2    
#define PIN_US2_ECHO        GPIO_NUM_36   
//================================
//--- Sensor de Color TCS3200 --- 
//================================
#define PIN_TCS_OUT         GPIO_NUM_34 
//====================================================
//--------------- Mapeo Primer 74HC595 --------------- 
//====================================================
#define SR_M2_IN1           3
#define SR_M2_IN2           2
#define SR_M2_IN3           1
#define SR_M2_IN4           0

#define SR_M1_IN1           7
#define SR_M1_IN2           6
#define SR_M1_IN3           5
#define SR_M1_IN4           4
//=====================================================
//--------------- Mapeo Segundo 74HC595 ---------------
//=====================================================
#define SR2_QA              8
#define SR2_QB              9
#define SR2_QC              10
#define SR2_QD              11
#define SR2_QE              12
#define SR2_QF              13
#define SR2_QG              14
#define SR2_QH              15

#define SR_TCS_S0           SR2_QC
#define SR_TCS_S1           SR2_QB
#define SR_TCS_S2           SR2_QA
#define SR_TCS_S3           SR2_QH

#define SR_LED_YELLOW       SR2_QD
#define SR_LED_BLUE         SR2_QE
#define SR_LED_RED          SR2_QF
#define SR_LED_GREEN        SR2_QG
//=====================================================
//-------------------  BUS I2C ------------------------ 
//=====================================================
#define PIN_I2C_SDA       GPIO_NUM_21
#define PIN_I2C_SCL       GPIO_NUM_22
#define I2C_PCA_PORT      I2C_NUM_0
#define PIN_PCA_OE        GPIO_NUM_NC
#define PCA9685_ADDR      PCA9685_DEFAULT_ADDR
#define PCA_CH_PISTON     0U 
#define PIN_AS5600_2_SCL  GPIO_NUM_5
#define PIN_AS5600_4_SCL  GPIO_NUM_17
#define PIN_AS5600_3_SCL  GPIO_NUM_16
#endif /* BOARD_PINS_H */
