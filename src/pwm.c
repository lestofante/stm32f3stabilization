/*
 * pwm.c
 *
 *  Created on: 06/set/2013
 *      Author: mauro
 */

#include "pwm.h"

void init_pwm_tim2() {
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* TIM2 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	/* GPIOB clock enable */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	/* TIM2 chennel2 configuration : PA.01 */
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP ;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Connect TIM pin to AF1 */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_1);

	/* Enable the TIM2 global Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void init_pwm_tim4() {
	GPIO_InitTypeDef GPIO_InitStruct;

	/*Enable or disable the AHB peripheral clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);

	/*Configure GPIO pin */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStruct);

	/* Connect TIM4 pins to AF2 */
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_2 );
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_2 );
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_2 );
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_2 );



	/* Here you can enable the TIM4 gloabal Interrupt */
	//NVIC_InitTypeDef NVIC_InitStructure;
	//NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	//NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	//NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	//NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	//NVIC_Init(&NVIC_InitStructure);

	uint16_t prescaler = (uint16_t) ((72000000 / 1000000) - 1); // Shooting for 1 MHz, (1us)

	uint32_t pwm_period = 20000;

	//  Enable the TIM4 peripherie
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	// Setup the timing and configure the TIM1 timer
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Prescaler = prescaler;
	TIM_TimeBaseStructure.TIM_Period = pwm_period - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up ;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

	// Initialise the timer channels
	TIM_OCInitTypeDef TIM_OCInitStructure;

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;

	TIM_OCInitStructure.TIM_Pulse = PWM_STOP; // preset pulse width 0..pwm_period
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; // Pulse polarity
	//	  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;

	// Setup four channels
	// Channel 1, pin PD10?
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable );

	// Channel 2, pin PD11?
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable );

	// Channel 3, pin PD12?
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable );

	// Channel 4, pin PD13?
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable );

	// Starup the timer
	//TIM_ARRPreloadConfig(TIM1, DISABLE);
	TIM_ARRPreloadConfig(TIM4, DISABLE);
	TIM_CtrlPWMOutputs(TIM4, ENABLE);
	TIM_Cmd(TIM4, ENABLE);

	// The PWM is running now. The pulse width can be set by
	// TIM1->CCR1 = [0..pwm_period] -> 0..100% duty cycle
	//
	// For example:
	// int pulse_width = 3000;
	// TIM4->CCR1 = pulse_width;
	//
	// The firmware offers a API to do this:
	//TIM_SetCompare1(TIM4 , pulse_width); // This is a wrapper for TIM4->CCR1, the same as TIM4->CCR1=pulse_width;

	//return pwm_period;
}

void setPwm(uint16_t eng1, uint16_t eng2, uint16_t eng3, uint16_t eng4) {
	if (eng1 < PWM_MIN) {
		eng1 = PWM_MIN;
	}
	if (eng1 > PWM_MAX) {
		eng1 = PWM_MAX;
	}
	TIM_SetCompare1(TIM4, eng1);

	if (eng2 < PWM_MIN) {
		eng2 = PWM_MIN;
	}
	if (eng2 > PWM_MAX) {
		eng2 = PWM_MAX;
	}
	TIM_SetCompare2(TIM4, eng2);

	if (eng3 < PWM_MIN) {
		eng3 = PWM_MIN;
	}
	if (eng3 > PWM_MAX) {
		eng3 = PWM_MAX;
	}
	TIM_SetCompare3(TIM4, eng3);

	if (eng4 < PWM_MIN) {
		eng4 = PWM_MIN;
	}
	if (eng4 > PWM_MAX) {
		eng4 = PWM_MAX;
	}
	TIM_SetCompare4(TIM4, eng4);
}
