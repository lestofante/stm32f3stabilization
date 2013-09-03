/**
 ******************************************************************************
 * @file    USB_Example/main.c
 * @author  MCD Application Team
 * @version V1.1.0
 * @date    20-September-2012
 * @brief   Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
 *
 * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *        http://www.st.com/software_license_agreement_liberty_v2
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usb_main.h"
#include "sensors.h"
#include "common.h"
#include "stm32f3_discovery.h"
//#include "arm_math.h"

/** @addtogroup STM32F3_Discovery_Peripheral_Examples
 * @
 */

/** @addtogroup USB_Example
 * @{
 */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
RCC_ClocksTypeDef RCC_Clocks;

//BECAUSE 800
#define PWM_MIN 8
#define PWM_MAX 18

int init_pwm2(int pwm_freq) {
	GPIO_InitTypeDef GPIO_InitStruct;

	/*Enable or disable the AHB peripheral clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);

	/*Configure GPIO pin */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOD, &GPIO_InitStruct);

	/* Connect TIM4 pins to AF2 */
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_2 );
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_2 );
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_2 );
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_2 );

	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable the TIM4 gloabal Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	uint16_t prescaler = (uint16_t) ((RCC_Clocks.HCLK_Frequency / 1000000) - 1); // Shooting for 1 MHz, (1us)

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

	TIM_OCInitStructure.TIM_Pulse = 800; // preset pulse width 0..pwm_period
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; // Pulse polarity
	//	  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;

	// These settings must be applied on the timer 1.
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Set;

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

	return pwm_period;
}

void setPwm(uint8_t eng1, uint8_t eng2, uint8_t eng3, uint8_t eng4) {
	if (eng1 < PWM_MIN) {
		eng1 = PWM_MIN;
	}
	if (eng1 > PWM_MAX) {
		eng1 = PWM_MAX;
	}
	TIM_SetCompare1(TIM4, eng1*100);

	if (eng2 < PWM_MIN) {
		eng2 = PWM_MIN;
	}
	if (eng2 > PWM_MAX) {
		eng2 = PWM_MAX;
	}
	TIM_SetCompare2(TIM4, eng2*100);

	if (eng3 < PWM_MIN) {
		eng3 = PWM_MIN;
	}
	if (eng3 > PWM_MAX) {
		eng3 = PWM_MAX;
	}
	TIM_SetCompare3(TIM4, eng3*100);

	if (eng4 < PWM_MIN) {
		eng4 = PWM_MIN;
	}
	if (eng4 > PWM_MAX) {
		eng4 = PWM_MAX;
	}
	TIM_SetCompare4(TIM4, eng4*100);
}

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
 * @brief  Main program.
 * @param  None
 * @retval None
 */
int main(void) {
	/* SysTick end of count event each 1us */
	RCC_GetClocksFreq(&RCC_Clocks);
	SysTick_Config((RCC_Clocks.HCLK_Frequency / 1000000UL));

	STM_EVAL_LEDInit(LED3); //used has heartBeat
	STM_EVAL_LEDInit(LED4);
	STM_EVAL_LEDInit(LED5);
	STM_EVAL_LEDInit(LED6);
	/*
	 STM_EVAL_LEDOff(LED3);
	 STM_EVAL_LEDOff(LED4);
	 STM_EVAL_LEDOff(LED5);
	 */

	//test();
	init_pwm2(5); //50Hz pwm

	uint8_t pwmDeciso = PWM_MIN;
	while(1){
		pwmDeciso++;
		if (pwmDeciso > PWM_MAX) {
			pwmDeciso = PWM_MIN; //restart!
		}
		setPwm(pwmDeciso, pwmDeciso, pwmDeciso, pwmDeciso);
		DelayMs(1000);
	}

	/* Configure the USB */
	USB_Config();

	//saluta!
	USB_write((uint8_t*) "ciao", 5, STRING);

	//led check
	STM_EVAL_LEDOn(LED4);
	DelayMs(1000);
	STM_EVAL_LEDOff(LED4);
	DelayMs(1000);
	STM_EVAL_LEDOn(LED5);
	DelayMs(1000);
	STM_EVAL_LEDOff(LED5);
	DelayMs(1000);
	STM_EVAL_LEDOn(LED6);
	//DelayMs(1000);
	//STM_EVAL_LEDOff(LED6);

	/* Infinite loop */

	Gyro_Config();

	Compass_Config();

	uint16_t toWrite[3];

	int count = 0;
	while (1) {
		if (Gyro_ReadAngRate(toWrite))
			USB_write((uint8_t*) toWrite, 6, SENSOR_GYR);
		if (Compass_ReadAcc(toWrite))
			USB_write((uint8_t*) toWrite, 6, SENSOR_ACC);
		if (Compass_ReadMag(toWrite)) {
			USB_write((uint8_t*) toWrite, 6, SENSOR_MAG);
			count++;
			if (count >= 160) {
				STM_EVAL_LEDToggle(LED6);
				count = 0;
			}
		}
	}
}

#ifdef  USE_FULL_ASSERT

/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while (1)
	{
	}
}
#endif

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
