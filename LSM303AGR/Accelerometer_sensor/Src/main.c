/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include <stdint.h>
#include <stdio.h>

#include "rcc_hal.h"
#include "gpio_hal.h"
#include "i2c_hal.h"
#include "lsm303agr_hal.h"

// Function declarations
void GPIO_Initialize(GPIOx*);
void print_register_data(uint8_t* ,uint8_t );

int main(void)
{
  I2Cx  *p_i2c1  = (I2Cx*) I2C1_Base_Address;
  GPIOx *p_gpiob = (GPIOx*) GPIOB_Base_Address;
  RCC   *p_rcc   = (RCC*) RCC_Base_Address;

  p_rcc->AHB1ENR |= GPIOB_EN;
  p_rcc->APB1ENR |= I2C1_EN;

  uint8_t temp_data[OUTPUT_DATA_REG_COUNT - 1];
  float x_acc_data, y_acc_data, z_acc_data;

  GPIO_Initialize(p_gpiob);
  printf("GPIO Initialized\n");
  I2C_Initialize(p_i2c1);
  printf("I2C Initialized\n");

  // Reset the previous configurations if any.
  i2c_write_single_reg(p_i2c1,Accelerometer,CFG_REG5_A_Reg,REBOOT);
  i2c_write_single_reg(p_i2c1,Accelerometer,CFG_REG5_A_Reg,0x0);
  printf("Accelerometer sensor reset\n");

  //Write initial configuration continuous mode at 20HZ with high resolution enabled
  i2c_write_single_reg(p_i2c1,Accelerometer,CFG_REG1_A_Reg,(ODR_100Hz | X_EN | Y_EN | Z_EN ));
  printf("Accelerometer sensor configured\n");

  // Enable the sensor.
  i2c_write_single_reg(p_i2c1,Accelerometer,CFG_REG4_A_Reg,(BDU_EN | FS_2));
  printf("Accelerometer sensor enabled\n");

  while(1){
	  while(!(i2c_read_single_reg(p_i2c1, Accelerometer, STATUS_REG_A_Reg) & XYZ_NEW_DATA_AVAILABLE));
	  i2c_read_multiple_reg(p_i2c1,  Accelerometer, OUT_X_L_A_Reg, temp_data, OUTPUT_DATA_REG_COUNT);
    /* 
    Raw data to physical G value
    ->High reg + low reg
    ->Remove the excess bits
    ->Multiply by sensitivity data
    ->Convert the mg value to G
    */
	  x_acc_data = (((int16_t)((temp_data[1]<<8) | temp_data[0])>>NR_MODE)*LA_SO_FS_2_NR)/MG_TO_G;
	  y_acc_data = (((int16_t)((temp_data[3]<<8) | temp_data[2])>>NR_MODE)*LA_SO_FS_2_NR)/MG_TO_G;
	  z_acc_data = (((int16_t)((temp_data[5]<<8) | temp_data[4])>>NR_MODE)*LA_SO_FS_2_NR)/MG_TO_G;

  }
  I2C_Terminate(p_i2c1);
}

void GPIO_Initialize(GPIOx* gpio){
  gpio->MODER   |= (0x2<<6*2 | 0x2<<9*2) ;     // Pin6 and Pin9 AF mode
  gpio->AFRL    |= 0x4<<6*4;                   // pin6 AF4 - I2C1
  gpio->AFRH    |= 0x4<<(9-8)*4;               // pin9 AF4 - I2C1
  gpio->OTYPER  |= (1<<6 | 1<<9);              // Pin6 and Pin9 open drain
  gpio->PUPDR   |= (0x1<<6*2 | 0x1<<9*2);      // Pin6 and Pin9 Pull up
  gpio->OSPEEDR |= (0x3<<6*2 | 0x3<<9*2);      // Pin6 and Pin9 High speed
}


void print_register_data(uint8_t* p_array,uint8_t count){
  for (int i = 0; i < count; i++)
  {
    printf("Data[%d] : 0x%X\n",i,p_array[i]);
  }
  printf("\n");
}
