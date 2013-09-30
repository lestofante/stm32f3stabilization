/**
 ******************************************************************************
 * @file    USB_Example/stm32f30x_it.c
 * @author  MCD Application Team
 * @version V1.1.0
 * @date    20-September-2012
 * @brief   Main Interrupt Service Routines.
 *          This file provides template for all exceptions handler and
 *          peripherals interrupt service routine.
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

/** @addtogroup STM32F3_Discovery_Peripheral_Examples
 * @{
 */

/** @addtogroup USB_Example
 * @{
 */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t IC2Value = 0;
__IO uint16_t DutyCycle = 0;
__IO uint32_t Frequency = 0;
__IO uint32_t CCR1 = 0;
RCC_ClocksTypeDef RCC_Clocks;
extern __IO uint32_t UserButtonPressed;
__IO uint32_t i2 =0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
 * @brief  This function handles NMI exception.
 * @param  None
 * @retval None
 */
void NMI_Handler(void)
{
}

/**
 * @brief  This function handles Hard Fault exception.
 * @param  None
 * @retval None
 */
void HardFault_Handler(void)
{
	/* Go to infinite loop when Hard Fault exception occurs */
	while (1)
	{
	}
}

/**
 * @brief  This function handles Memory Manage exception.
 * @param  None
 * @retval None
 */
void MemManage_Handler(void)
{
	/* Go to infinite loop when Memory Manage exception occurs */
	while (1)
	{
	}
}

/**
 * @brief  This function handles Bus Fault exception.
 * @param  None
 * @retval None
 */
void BusFault_Handler(void)
{
	/* Go to infinite loop when Bus Fault exception occurs */
	while (1)
	{
	}
}

/**
 * @brief  This function handles Usage Fault exception.
 * @param  None
 * @retval None
 */
void UsageFault_Handler(void)
{
	/* Go to infinite loop when Usage Fault exception occurs */
	while (1)
	{
	}
}

/**
 * @brief  This function handles SVCall exception.
 * @param  None
 * @retval None
 */
void SVC_Handler(void)
{
}

/**
 * @brief  This function handles Debug Monitor exception.
 * @param  None
 * @retval None
 */
void DebugMon_Handler(void)
{
}

/**
 * @brief  This function handles PendSVC exception.
 * @param  None
 * @retval None
 */
void PendSV_Handler(void)
{
}

/**
 * @brief  This function handles SysTick Handler.
 * @param  None
 * @retval None
 */
/* DEFINED IN COMMON.c
void SysTick_Handler(void)
{
  DataReady ++;
}
 */

/******************************************************************************/
/*                 STM32F30x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f30x.s).                                            */
/******************************************************************************/
/**
 * @brief  This function handles TIM2 global interrupt request.
 * @param  None
 * @retval None
 */
void TIM2_IRQHandler(void)
{
	RCC_GetClocksFreq(&RCC_Clocks);


	/* Get the Input Capture value */
	IC2Value = TIM_GetCapture2(TIM2);

	STM_EVAL_LEDToggle(LED7);

	if (IC2Value != 0)
	{
		CCR1 = TIM_GetCapture1(TIM2);
		/* Duty cycle computation */
		DutyCycle = (CCR1 * 1000) / IC2Value;

		/* Frequency computation
       TIM2 counter clock = (RCC_Clocks.HCLK_Frequency)/2 */

		Frequency = RCC_Clocks.HCLK_Frequency / IC2Value;
	}
	else
	{
		DutyCycle = 0;
		Frequency = 0;
	}

	/* Clear TIM2 Capture compare interrupt pending bit */
	TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);
}

/**
 * @brief  This function handles EXTI0_IRQ Handler.
 * @param  None
 * @retval None
 */
void EXTI0_IRQHandler(void)
{
	if ((EXTI_GetITStatus(USER_BUTTON_EXTI_LINE) == SET)&&(STM_EVAL_PBGetState(BUTTON_USER) != RESET))
	{

		/* Delay */
		for(i2=0; i2<0x7FFFF; i2++);

		/* Wait for SEL button to be pressed  */
		while(STM_EVAL_PBGetState(BUTTON_USER) != RESET);
		/* Delay */
		for(i2=0; i2<0x7FFFF; i2++);
		UserButtonPressed++;

		STM_EVAL_LEDToggle(LED6);
		if(UserButtonPressed==2)
			STM_EVAL_LEDToggle(LED7);


		if (UserButtonPressed > 0x2)
		{
			UserButtonPressed = 0x0;
		}


		/* Clear the EXTI line pending bit */
		EXTI_ClearITPendingBit(USER_BUTTON_EXTI_LINE);
	}
}

#if defined (USB_INT_DEFAULT)
void USB_LP_CAN1_RX0_IRQHandler(void)
#elif defined (USB_INT_REMAP)
void USB_LP_IRQHandler(void)
#endif
{
	USB_Istr();
}

#if defined (USB_INT_DEFAULT)
void USBWakeUp_IRQHandler(void)
#elif defined (USB_INT_REMAP)
void USBWakeUp_RMP_IRQHandler(void)
#endif
{
	/* Initiate external resume sequence (1 step) */
	Resume(RESUME_EXTERNAL);
	EXTI_ClearITPendingBit(EXTI_Line18);
}
/**
 * @brief  This function handles PPP interrupt request.
 * @param  None
 * @retval None
 */
/*void PPP_IRQHandler(void)
{
}*/

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

