#include "common.h"
#include "sensors.h"
#include "usart.h"


RCC_ClocksTypeDef RCC_Clocks;

__IO uint32_t UserButtonPressed = 0;
__IO float HeadingValue = 0.0f;

uint16_t MagBuffer[3] = {0}, AccBuffer[3] = {0}, GyroBuffer[3] = {0};

int main(void)
{
	/* SysTick end of count event each 1us */
	RCC_GetClocksFreq(&RCC_Clocks);
	SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000000UL);

	/* Initialize LEDs */
	STM_EVAL_LEDInit(LED3);
	STM_EVAL_LEDInit(LED4);
	STM_EVAL_LEDInit(LED5);
	STM_EVAL_LEDInit(LED6);
	STM_EVAL_LEDInit(LED7);
	STM_EVAL_LEDInit(LED8);
	STM_EVAL_LEDInit(LED9);
	STM_EVAL_LEDInit(LED10);

	/* Initialize pushbutton */
	STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_EXTI);



	USART2_Init(460800);
	printf("\fUART2 initialized\n\r");

	/*
	while(1){
		printf("ciao!\n");
	}
	 */

	int8_t readGiroscopio = 1;
	int8_t readAccelerometro = 1;
	int8_t readMagnetometro = 1;


	/* Reset UserButton_Pressed variable */
	UserButtonPressed = 0x00;

	/* Initialize gyro */
	Gyro_Config();

	/* Initialize compass */
	Compass_Config(); //

	uint32_t lastTime, lastTimeUpGyro, lastTimeUpAcc, lastTimeUpMagne;
	uint32_t almostNow=micros();



	lastTime = lastTimeUpGyro = lastTimeUpAcc = lastTimeUpMagne = micros();
	uint8_t upG, upA, upM;

	upG = upA = upM = 0;

	int i;

	uint8_t *a;

	while (1)
	{

		almostNow = micros();
		if (readGiroscopio && micros() - lastTimeUpGyro > 1316){ //should be at 760Hz, so wait for 1316 uSec
			/* Read Gyro data */
			Gyro_ReadAngRate(GyroBuffer);

			printf("G");
			a = GyroBuffer;
			for(i = 0; i < sizeof(uint16_t)*3; i++)
			    printf("%c", a[i]);

			lastTimeUpGyro = micros();
			upG++;
		}

		if (readAccelerometro && micros() - lastTimeUpAcc > 745){ //should be at 1344Hz, so wait for 745 uSec
			/* Read Accelerometer data */
			Compass_ReadAcc(AccBuffer);

			printf("A");
			a = AccBuffer;
			for(i = 0; i < sizeof(uint16_t)*3; i++)
			    printf("%c", a[i]);

			lastTimeUpAcc = micros();

			upA++;
		}

		if (readMagnetometro && micros() - lastTimeUpMagne > 4546){ //should be at 220Hz, so wait for 4546 uSec
			/* Read Magnetometer data */
			Compass_ReadMag(MagBuffer);
			//printf("M: %f %f %f\n",MagBuffer[0], MagBuffer[1], MagBuffer[2]);

			printf("M");
			a = MagBuffer;
			for(i = 0; i < sizeof(uint16_t)*3; i++)
			    printf("%c", a[i]);

			lastTimeUpMagne = micros();

			upM++;
		}


		/* Toggle LD3 */
		//STM_EVAL_LEDToggle(LED3);
		//_delay_ms(5);

		if (micros()-lastTime > 1000000UL){ //1 seconds
			almostNow = micros()-almostNow;
			printf("loop duration: %lu\n",almostNow);

			printf("up G %d, A %d, M %d\n",upG, upA, upM);
			/* Toggle LD10 */
			STM_EVAL_LEDToggle(LED10);

			upG = upA = upM = 0;

			lastTime = micros();
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



/******************************************************************************/
/*                 STM32F30x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f30x.s).                                            */
/******************************************************************************/
/**
 * @brief  This function handles EXTI0_IRQ Handler.
 * @param  None
 * @retval None
 */
void EXTI0_IRQHandler(void)
{
	uint32_t i;
	if ((EXTI_GetITStatus(USER_BUTTON_EXTI_LINE) == SET)&&(STM_EVAL_PBGetState(BUTTON_USER) != RESET))
	{
		/* Delay */
		for(i=0; i<0x7FFFF; i++);

		/* Wait for SEL button to be pressed  */
		while(STM_EVAL_PBGetState(BUTTON_USER) != RESET);
		/* Delay */
		for(i=0; i<0x7FFFF; i++);
		UserButtonPressed++;

		if (UserButtonPressed > 0x2)
		{
			UserButtonPressed = 0x0;
		}

		/* Clear the EXTI line pending bit */
		EXTI_ClearITPendingBit(USER_BUTTON_EXTI_LINE);
	}
}

