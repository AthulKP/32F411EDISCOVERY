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
#include "gpio_hal.h"

#define RCC_AHB1ENR_Address	((uint32_t)0x40023830)
#define Delay_counter 0x147260 // 0d1340000 approx 1sec delay
#define LED_Pin	Green_Led //Replace with required led output
int main(void)
{
	uint32_t volatile *const p_gpioClkReg = (uint32_t*) RCC_AHB1ENR_Address;
	GPIOx *p_gpiod = (GPIOx*) GPIOD_Base_Address;

	//Enable clock for the GPIOD port
	*p_gpioClkReg |= GPIOD_EN;

	//Configure the GPIOD port
	//Set Led pin as output
	p_gpiod->MODER |= GP_Output_Mode<<LED_Pin*2; //2 bit positions for each GPIO

	//Pull Led pin low
	p_gpiod->PUPDR |= Pull_Down<<LED_Pin*2;

	//forever loop
	while(1){
		p_gpiod->ODR |= Led_On<<LED_Pin;
		for(int i = 0;i<=Delay_counter;i++);//Making the processor to count thereby creating a delay
		p_gpiod->ODR &= Led_Off<<LED_Pin;
		for(int i = 0;i<=Delay_counter;i++);
	}
}
