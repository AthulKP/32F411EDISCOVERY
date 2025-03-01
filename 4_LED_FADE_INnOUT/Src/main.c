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
#include "timer_hal.h"
#include "GPIO_hal.h"

#define RCC_AHB1ENR_Base_Address 0x40023830
#define RCC_APB1ENR_Base_Address 0x40023840
#define PWM_INTERVAL	50 // Delay in ms
void delay_ms(volatile TIMx *, uint32_t);

int main(void)
{
  uint32_t volatile *p_RCC_AHB1ENR = (uint32_t *)RCC_AHB1ENR_Base_Address;
  uint32_t volatile *p_RCC_APB1ENR = (uint32_t *)RCC_APB1ENR_Base_Address;

  // Enable clock for GPIOD peripheral
  *p_RCC_AHB1ENR |= GPIOD_EN;
  // Enable clock for Timer 4
  *p_RCC_APB1ENR |= TIMER4_EN;
  // Enable clock for Timer 2
  *p_RCC_APB1ENR |= TIMER2_EN;

  // GPIOD pointer
  GPIOx volatile *p_GPIOD = (GPIOx *)GPIOD_Base_Address;
  // Timer 2 and 4 pointer
  TIMx volatile *p_TIMER4 = (TIMx *)TIM4_Base_Address;
  TIMx volatile *p_TIMER2 = (TIMx *)TIM2_Base_Address;

  // GPIO configuration : Mode - alternate function, AF type - AF2 and output speed - high
  p_GPIOD->MODER |= ((GP_AF_MODE << GREEN_LED * BIT_FIELD_2) | (GP_AF_MODE << ORANGE_LED * BIT_FIELD_2) | (GP_AF_MODE << RED_LED * BIT_FIELD_2) | (GP_AF_MODE << BLUE_LED * BIT_FIELD_2));
  p_GPIOD->AFRH |= ((AF_MODE_AF2 << (GREEN_LED - AF_HIGH_OFFSET) * BIT_FIELD_4) | (AF_MODE_AF2 << (ORANGE_LED - AF_HIGH_OFFSET) * BIT_FIELD_4) | (AF_MODE_AF2 << (RED_LED - AF_HIGH_OFFSET) * BIT_FIELD_4) | (AF_MODE_AF2 << (BLUE_LED - AF_HIGH_OFFSET) * BIT_FIELD_4));
  p_GPIOD->OSPEEDR |= ((OS_HIGH << GREEN_LED * BIT_FIELD_2) | (OS_HIGH << ORANGE_LED * BIT_FIELD_2) | (OS_HIGH << RED_LED * BIT_FIELD_2) | (OS_HIGH << BLUE_LED * BIT_FIELD_2));

  // Enable timer 2 for delay
  // Prescale by 16000 : 16MHz clock - 1KHz => o.1ms/count
  p_TIMER2->PSC = PRESCALE_16000 - 1;
  // Set auto-reload to max
  p_TIMER2->ARR = ARR_MAX - 1;
  //Start the timer
  p_TIMER2->CR1   |= CEN_EN;


  // Generate a 10KHz clock : a counter goes from 0 to 9999 in one second 
  p_TIMER4->PSC = PRESCALE_1600 - 1;
  // Generate a PWMfrq = Counter frequency / auto-reload = 10KHz / 100 = 100Hz
  p_TIMER4->ARR = ARR_100 - 1;

  // Set each channel mode as PWM mode 1 and then enable output compare for the channel
  p_TIMER4->CCMR1 |= OC1M_PWM_1;                             
  p_TIMER4->CCMR1 |= OC1PE_EN;                               
  p_TIMER4->CCMR1 |= OC2M_PWM_1;                             
  p_TIMER4->CCMR1 |= OC2PE_EN;                               
  p_TIMER4->CCMR2 |= OC3M_PWM_1;                             
  p_TIMER4->CCMR2 |= OC3PE_EN;                               
  p_TIMER4->CCMR2 |= OC4M_PWM_1;                             
  p_TIMER4->CCMR2 |= OC4PE_EN;

  // Turn ON all channel output
  p_TIMER4->CCER |= (CC1E_EN | CC2E_EN | CC3E_EN | CC4E_EN); 
  p_TIMER4->CR1 |= ARPE_EN;         // Auto-reload pre-load enable
  p_TIMER4->EGR |= UG_REINITIALIZE; // Re-initialize the counter and generates an update of the registers

  // Start the timer 4
  p_TIMER4->CR1 |= CEN_EN;

  /* The value in CCRx is considered for turning ON and OFF of the GPIO pin
	 Since counter goes from 0-100 (ARR) and CCRx = 50 => GPIO pin will be High for timer count 0-50, then turns OFF until next reset that is the timer count is 100.
   */
  while (1)
  {
    for (int i = 0; i < 100; i++)
    {
      p_TIMER4->CCR1 = i;
      p_TIMER4->CCR2 = i;
      p_TIMER4->CCR3 = i;
      p_TIMER4->CCR4 = i;
      delay_ms(p_TIMER2,PWM_INTERVAL);
    }
    for (int i = 100; i >= 0; i--)
    {
      p_TIMER4->CCR1 = i;
      p_TIMER4->CCR2 = i;
      p_TIMER4->CCR3 = i;
      p_TIMER4->CCR4 = i;
      delay_ms(p_TIMER2,PWM_INTERVAL);
    }
  }
}

void delay_ms(volatile TIMx *timer, uint32_t delay_time)
{
  uint32_t current_count = timer->CNT;
  while ((timer->CNT - current_count) <= delay_time);
}
