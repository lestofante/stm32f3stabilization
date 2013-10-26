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
#include "pwm.h"
#include "dcm.h"
#include "pid.h"
#include "stm32f3_discovery.h"
#include <stdlib.h>

#define gyroToRad (2293.76/32768)*0.0174532925
#define SAMPLE_PID 5000

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
TIM_ICInitTypeDef  TIM_ICInitStructure;
__IO uint32_t CCR1;

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
	STM_EVAL_LEDInit(LED7);
	STM_EVAL_LEDInit(LED8);

	init_pwm_tim2();

	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICFilter = 0x0;

	TIM_PWMIConfig(TIM2, &TIM_ICInitStructure);

	/* Select the TIM2 Input Trigger: TI2FP2 */
	TIM_SelectInputTrigger(TIM2, TIM_TS_TI2FP2);

	/* Select the slave Mode: Reset Mode */
	TIM_SelectSlaveMode(TIM2, TIM_SlaveMode_Reset);
	TIM_SelectMasterSlaveMode(TIM2,TIM_MasterSlaveMode_Enable);

	/* TIM enable counter */
	TIM_Cmd(TIM2, ENABLE);

	/* Enable the CC2 Interrupt Request */
	TIM_ITConfig(TIM2, TIM_IT_CC2, ENABLE);
	/*
	 STM_EVAL_LEDOff(LED3);
	 STM_EVAL_LEDOff(LED4);
	 STM_EVAL_LEDOff(LED5);
	 */

	//test();
	init_pwm_tim4(); //50Hz pwm
	/*
	uint8_t pwmDeciso = PWM_MIN;
	while(1){
		pwmDeciso++;
		if (pwmDeciso > PWM_MAX) {
			pwmDeciso = PWM_MIN; //restart!
		}
		setPwm(pwmDeciso, pwmDeciso, pwmDeciso, pwmDeciso);
		DelayMs(1000);
	}
	 */
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

	uint16_t gyro[3], acc[3], magne[3];
	float quaternion[4];
	//uint8_t test[4];
	//int result = 0;

	float yprFromRx[] = {0,0,0}; //this data will come from RX
	uint16_t poweFromRx = 1100; // actually can go from 0 to 100 (MAX_PWM-MIN_PWM)

	float ypr[3];
	float pid_out[4];
	init(SAMPLE_PID);

	unsigned long lastTimePPM = 0;

	float PID[3] = {60, 0, 0};

	addPid(&ypr[1], &yprFromRx[2], &pid_out[2], &PID[0], &PID[1], &PID[2], -300, 300); //all pid are the same!

	uint8_t tmpRead[200], countTmpRead = 0, indTmpRead=0, nextByte;

	while (1) {

		countTmpRead = USB_read(tmpRead);
		indTmpRead = 0;
		while ( countTmpRead-indTmpRead >= 5 ){
			nextByte = tmpRead[indTmpRead++];

			union {
				uint8_t bytes[4];
			    float f;
			} byteToFloat;

			uint8_t i;
			for (i=0; i<4;i++){
				byteToFloat.bytes[i] = tmpRead[indTmpRead++];
			}
			switch( nextByte ){
			case 'P':
				PID[0] = byteToFloat.f;
				break;
			case 'I':
				PID[1] = byteToFloat.f;
				break;
			case 'D':
				PID[2] = byteToFloat.f;
				break;
			case 'E':
				poweFromRx = byteToFloat.f;
				break;
			}
			char str[20];
			int numChar = sprintf(str, "E %d PID %f %f %f", poweFromRx, PID[0], PID[1], PID[2]);
			USB_write((uint8_t*) str, numChar+1, STRING);
		}

		USB_write((uint8_t*) &CCR1, 4, PWM);

		if (Compass_ReadAcc(acc))
			USB_write((uint8_t*) acc, 6, SENSOR_ACC);
		if (Compass_ReadMag(magne)) {
			USB_write((uint8_t*) magne, 6, SENSOR_MAG);
		}

		if (Gyro_ReadAngRate(gyro)){
			USB_write((uint8_t*) gyro, 6, SENSOR_GYR);
			//-x*toRad, -y*toRad, z*toRad, -this.ay, this.ax, this.az, -this.my, this.mx, this.mz
			freeIMUUpdate(-(short)gyro[0]*gyroToRad,-(short)gyro[1]*gyroToRad, (short)gyro[2]*gyroToRad, -(short)acc[1], (short)acc[0], (short)acc[2], -(short)magne[2], (short)magne[0], (short)magne[1]);

			getQuaternion(quaternion);

			USB_write((uint8_t*) quaternion, sizeof(float)*4, DCM);



			quaternionToYawPitchRoll(ypr);

			USB_write((uint8_t*) ypr, sizeof(float)*3, ANGLE);

			computePid();//compute PID
			//don't use again those values
			//acc[0] = acc[1] = acc[2] = magne[0] = magne[1] = magne[2] = 0;
		}

		unsigned long now = micros();
		unsigned long timeChange = (now - lastTimePPM);
		//all PPM has the same update time! 100.000 Hz
		if(timeChange>=100000UL){
			lastTimePPM = micros();
			int pid2 = pid_out[2]; //esc seems to fuck up with small step

			int motRx = (poweFromRx + pid2);
			int motSx = (poweFromRx - pid2);

			setPwm(motRx,motSx,0,0);
			char str[20];
			int numChar = sprintf(str, "rx %d sx %d - %d", motRx, motSx, pid2);
			USB_write((uint8_t*) str, numChar+1, STRING);
		}
		//result = USB_read(test);
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
