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
int main(void)
{
	/* SysTick end of count event each 1us */
	RCC_GetClocksFreq(&RCC_Clocks);
	SysTick_Config( (RCC_Clocks.HCLK_Frequency / 1000000UL) );
	//SysTick_Config(250); //should set interrupt to go off 32000 times a second

	/* Configure the USB */
	USB_Config();

	/* Infinite loop */

	Gyro_Config();

	Compass_Config();

	uint16_t temp_sensor_read[] = {0, 0, 0};

	uint32_t lastGyro=0, lastacc=0, lastMagne=0, lastMicrosec=micros();
	uint16_t read = 0, cicle = 0;
	uint32_t ora = micros();

	uint16_t toWrite[3];

	while (1)
	{
		if(Gyro_ReadAngRate(toWrite))
			USB_write((uint8_t*)toWrite, 6, SENSOR_GYR);
		if(Compass_ReadAcc(toWrite))
			USB_write((uint8_t*)toWrite, 6, SENSOR_ACC);
		if(Compass_ReadMag(toWrite))
			USB_write((uint8_t*)toWrite, 6, SENSOR_MAG);
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
