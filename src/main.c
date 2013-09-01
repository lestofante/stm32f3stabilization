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


int init_pwm2(int pwm_freq)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	/*Enable or disable the AHB peripheral clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);

	/*Configure GPIO pin */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOD, &GPIO_InitStruct);

	/* Connect TIM4 pins to AF2 */
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_2);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource11, GPIO_AF_2);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_2);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_2);

	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable the TIM4 gloabal Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// Calculates the timing. This is common for all channels
	uint32_t clk = RCC_Clocks.HCLK_Frequency; // 72MHz -> system core clock. This is default on the stm32f3 discovery
	//RCC_Clocks.PCLK1_Frequency;
	uint32_t tim_freq = 2e6; // in Hz (2MHz) Base frequency of the pwm timer
	uint32_t prescaler = ((clk / tim_freq) - 1);

	// Calculate the period for a given pwm frequency
	//		int pwm_freq = 200; // in Hz
	uint32_t pwm_period = tim_freq/pwm_freq;		// 2MHz / 200Hz = 10000
												// For 50Hz we get: 2MHz / 50Hz = 40000

	// Calculate a number of pulses per millisecond.
	// Not used in this rutine but I put it here just as an example
	uint32_t ms_pulses = (float)pwm_period / (1000.0/pwm_freq); // for 200Hz we get: 10000 / (1/200 * 1000) = 2000


//  Enable the TIM1 peripherie
	RCC_APB2PeriphClockCmd(RCC_APB1Periph_TIM4 , ENABLE );

// Setup the timing and configure the TIM1 timer
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_TimeBaseStructInit(& TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Prescaler = prescaler;
	TIM_TimeBaseStructure.TIM_Period = pwm_period - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up ;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);


// Initialise the timer channels
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;

	TIM_OCInitStructure.TIM_Pulse = ms_pulses/2; // preset pulse width 0..pwm_period
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
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);

	// Channel 2, pin PD11?
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);

	// Channel 3, pin PD12?
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);

	// Channel 4, pin PD13?
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);

	// Starup the timer
	//TIM_ARRPreloadConfig(TIM1, DISABLE);
	TIM_ARRPreloadConfig(TIM4, DISABLE);
	TIM_CtrlPWMOutputs(TIM4, ENABLE);
	TIM_Cmd(TIM4 , ENABLE);

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


/**
  * @brief  Initializes the putput pins for the Timer one channels. Currently
  * 		hard coded to drive the LED3 (channel 1), LED7 (channel 2), and
  * 		LED10 (channel 3) on the STM32F3 Discovery board.
  * @param  pwm_freq: Frequency of the PWM in Hz
  * @retval Number of the timer pulses per one PWM period
  */
void init_pwm_gpio()
{
	// The Timer 1 channels 1,2 and 3 are connected to the LED pins on the Discovery board
	// To drive the ECSs they could to be connected to other pins if needed.

	// Setup the LEDs
	// Check out the Discovery schematics
	// http://www.st.com/st-web-ui/static/active/en/resource/technical/document/user_manual/DM00063382.pdf
		//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE);
	GPIO_InitStructure.GPIO_Pin = LED3_PIN | LED7_PIN | LED10_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; // Use the alternative pin functions
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // GPIO speed - has nothing to do with the timer timing
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; // Push-pull
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; // Setup pull-up resistors
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	// Connect the timer output to the LED pins
	// Check the alternative function mapping in the CPU doc
	// http://www.st.com/st-web-ui/static/active/en/resource/technical/document/datasheet/DM00058181.pdf
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF_2); // TIM1_CH1 -> LED3
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF_2); // TIM1_CH2 -> LED7
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF_2); // TIM1_CH3 -> LED10


	/**/


	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; // Use the alternative pin functions
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // GPIO speed - has nothing to do with the timer timing
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; // Push-pull
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; // Setup pull-up resistors
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	// Connect the timer output to the LED pins
	// Check the alternative function mapping in the CPU doc
	// http://www.st.com/st-web-ui/static/active/en/resource/technical/document/datasheet/DM00058181.pdf
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_2);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource11, GPIO_AF_2);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_2);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_2);
}

/**
  * @brief  Initializes PWM
  * @param  pwm_freq: Frequency of the PWM in Hz
  * @retval Number of the timer pulses per one PWM period
  */
int init_pwm(int pwm_freq)
{
	// Calculates the timing. This is common for all channels
	int clk = 72e6; // 72MHz -> system core clock. This is default on the stm32f3 discovery
	int tim_freq = 2e6; // in Hz (2MHz) Base frequency of the pwm timer
	int prescaler = ((clk / tim_freq) - 1);

	// Calculate the period for a given pwm frequency
	//		int pwm_freq = 200; // in Hz
	int pwm_period = tim_freq/pwm_freq;		// 2MHz / 200Hz = 10000
												// For 50Hz we get: 2MHz / 50Hz = 40000

	// Calculate a number of pulses per millisecond.
	// Not used in this rutine but I put it here just as an example
	int ms_pulses = (float)pwm_period / (1000.0/pwm_freq); // for 200Hz we get: 10000 / (1/200 * 1000) = 2000


//  Enable the TIM1 peripherie
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 , ENABLE );

// Setup the timing and configure the TIM1 timer
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_TimeBaseStructInit(& TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Prescaler = prescaler;
	TIM_TimeBaseStructure.TIM_Period = pwm_period - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up ;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);


// Initialise the timer channels
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;

	TIM_OCInitStructure.TIM_Pulse = ms_pulses*2; // preset pulse width 0..pwm_period
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; // Pulse polarity
	//	  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;

	// These settings must be applied on the timer 1.
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Set;

// Setup four channels
	// Channel 1
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);

	// Channel 2
	TIM_OC2Init(TIM1, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);

	// Channel 3
	TIM_OC3Init(TIM1, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);

	// Channel 4
	TIM_OC4Init(TIM1, &TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);

	// Starup the timer
	//TIM_ARRPreloadConfig(TIM1, DISABLE);
	TIM_ARRPreloadConfig(TIM1, DISABLE);
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
	TIM_Cmd(TIM1 , ENABLE);

	// The PWM is running now. The pulse width can be set by
	// TIM1->CCR1 = [0..pwm_period] -> 0..100% duty cycle
	//
	// For example:
	// int pulse_width = 3000;
	// TIM1->CCR1 = pulse_width;
	//
	// The firmware offers a API to do this:
	// TIM_SetCompare1(TIM1 , pulse_width); // This is a wrapper for TIM1->CCR1, the same as TIM1->CCR1=pulse_width;



	//  Enable the TIM1 peripherie
		RCC_APB2PeriphClockCmd(RCC_APB1Periph_TIM4 , ENABLE );

	// Setup the timing and configure the TIM1 timer
		TIM_TimeBaseStructInit(& TIM_TimeBaseStructure);
		TIM_TimeBaseStructure.TIM_Prescaler = prescaler;
		TIM_TimeBaseStructure.TIM_Period = pwm_period - 1;
		TIM_TimeBaseStructure.TIM_ClockDivision = 0;
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up ;
		TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);


	// Initialise the timer channels

		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;

		TIM_OCInitStructure.TIM_Pulse = ms_pulses*2; // preset pulse width 0..pwm_period
		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; // Pulse polarity
		//	  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
		TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;

		// These settings must be applied on the timer 1.
		TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
		TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
		TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Set;

	// Setup four channels
		// Channel 1
		TIM_OC1Init(TIM4, &TIM_OCInitStructure);
		TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);

		// Channel 2
		TIM_OC2Init(TIM4, &TIM_OCInitStructure);
		TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);

		// Channel 3
		TIM_OC3Init(TIM4, &TIM_OCInitStructure);
		TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);

		// Channel 4
		TIM_OC4Init(TIM4, &TIM_OCInitStructure);
		TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);

		// Starup the timer
		//TIM_ARRPreloadConfig(TIM1, DISABLE);
		TIM_ARRPreloadConfig(TIM4, DISABLE);
		TIM_CtrlPWMOutputs(TIM4, ENABLE);
		TIM_Cmd(TIM4 , ENABLE);


	return pwm_period;
}

volatile uint16_t countaOverflowTimer4 = 0;
void TIM4_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET){
	  TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	  countaOverflowTimer4 ++;
	  if (countaOverflowTimer4%50 == 2000UL*1000UL){//every sec?
		  STM_EVAL_LEDToggle(LED3);
		  //USB_write((uint8_t*)"Inte", 5, STRING);
	  }
  }
}


/**
  * @brief  Sets the PWM duty cycle per channel
  * @param  channel:  PWM channel index [1..4]
  * @param  duty_cycle:  PWM duty cycle in percents (integer) [0..100]
  * @retval None
  */
void set_pwm_width(int channel, int pwm_period, int duty_cycle)
{
	int pwm_pulses = pwm_period*(float)duty_cycle/100.0;
	switch (channel){
		case 1: TIM_SetCompare1(TIM1, pwm_pulses); break;
		case 2: TIM_SetCompare2(TIM1, pwm_pulses); break;
		case 3: TIM_SetCompare3(TIM1, pwm_pulses); break;
		case 4: TIM_SetCompare4(TIM1, pwm_pulses); break;
	}
}

/**
  * @brief  Sets the PWM duty cycle per channel
  * @param  channel:  PWM channel index [1..4]
  * @param  duty_cycle:  PWM duty cycle (float) [0..1]
  * @retval None
  */
void set_pwm_width_norm(int channel, int pwm_period, float duty_cycle)
{
	int pwm_pulses = pwm_period*(float)duty_cycle;
	switch (channel){
		case 1: TIM_SetCompare1(TIM1, pwm_pulses); break;
		case 2: TIM_SetCompare2(TIM1, pwm_pulses); break;
		case 3: TIM_SetCompare3(TIM1, pwm_pulses); break;
		case 4: TIM_SetCompare4(TIM1, pwm_pulses); break;
	}
}

// Just simple exponential mapping
// Not important for this demo
float gammaCorrect(int b, int c)
{
	double f = ((double)b/(float)c);
	return f*f*f*f*f; // gamma = 5
}


/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
 * @brief  Main program.
 * @param  None
 * @retval None
 */
int main(void)
{
	/* SysTick end of count event each 1us */
	RCC_GetClocksFreq(&RCC_Clocks);
	SysTick_Config( (RCC_Clocks.HCLK_Frequency / 1000000UL) );

	STM_EVAL_LEDInit(LED3); //used has heartBeat
	STM_EVAL_LEDInit(LED4);
	STM_EVAL_LEDInit(LED5);
	STM_EVAL_LEDInit(LED6);
/*
	STM_EVAL_LEDOff(LED3);
	STM_EVAL_LEDOff(LED4);
	STM_EVAL_LEDOff(LED5);
*/

	init_pwm_gpio();
		// Config the PWM freq to 200Hz
		int pwm_period = init_pwm(50);

		// Change the brightness of the LED3,7 and 10
		int brightness = 0;
		int up = 1;
		while (1) {

			up ? brightness++ : brightness--;
			if (brightness >= pwm_period) up = 0;
			if (brightness <= 0) up = 1;

			// The gammaCorrect() just compensates for the non linear brightness perception of human eye.
			// I's just an eye candy :-)
			float level = gammaCorrect(brightness, pwm_period);
			set_pwm_width_norm(1, pwm_period, level);
			set_pwm_width_norm(3, pwm_period, level);

			// Make the LED7 to fade inversely
			level = gammaCorrect(pwm_period - brightness, pwm_period);
			set_pwm_width_norm(2, pwm_period, level);

			// Delay
			int i;
			for (i = 0; i < 300; i++);
		}



	//test();
	init_pwm2(50);//50Hz pwm

	/* Configure the USB */
	USB_Config();

	//saluta!
	USB_write((uint8_t*)"ciao", 5, STRING);

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
	while (1)
	{
		if(Gyro_ReadAngRate(toWrite))
			USB_write((uint8_t*)toWrite, 6, SENSOR_GYR);
		if(Compass_ReadAcc(toWrite))
			USB_write((uint8_t*)toWrite, 6, SENSOR_ACC);
		if(Compass_ReadMag(toWrite)){
			USB_write((uint8_t*)toWrite, 6, SENSOR_MAG);
			count++;
			if (count >= 500 ){
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
