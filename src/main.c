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
#include "stm32f3_discovery.h"

#define gyroToRad (2293.76/32768)*0.0174532925

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

	while (1) {

		if (Compass_ReadAcc(acc))
			USB_write((uint8_t*) acc, 6, SENSOR_ACC);
		if (Compass_ReadMag(magne)) {
			USB_write((uint8_t*) magne, 6, SENSOR_MAG);
		}

		if (Gyro_ReadAngRate(gyro)){
			USB_write((uint8_t*) gyro, 6, SENSOR_GYR);
			//-x*toRad, -y*toRad, z*toRad, -this.ay, this.ax, this.az, -this.my, this.mx, this.mz
			freeIMUUpdate(-gyro[0]*gyroToRad,-gyro[1]*gyroToRad, gyro[2]*gyroToRad, -acc[1], acc[0], acc[2], -magne[1], magne[0], magne[2]);

			getQuaternion(quaternion);

			USB_write((uint8_t*) quaternion, sizeof(float)*4, DCM);

			//don't use again those values
			acc[0] = acc[1] = acc[2] = magne[0] = magne[1] = magne[2] = 0;
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
