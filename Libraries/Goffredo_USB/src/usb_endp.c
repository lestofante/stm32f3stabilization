/**
  ******************************************************************************
  * @file    USB_Example/usb_endp.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    20-September-2012
  * @brief   Endpoint routines
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
#include "usb_lib.h"
#include "usb_istr.h"
#include "usb_main.h"


#define BUFFER 180


uint8_t vuoto[BUFFER] = {0};
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
extern __IO bool NEXT_BUFFER_R;
extern __IO bool* DATA_PRESENT;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  EP1 IN Callback Routine.
  * @param  None
  * @retval None
  */
void EP1_IN_Callback(void)
{
	if (GetENDPOINT(ENDP1) & EP_DTOG_RX)
	{
		DATA_PRESENT[1] = 0;
		NEXT_BUFFER_R = 0;
	}
	else
	{
		DATA_PRESENT[0] = 0;
		NEXT_BUFFER_R = 1;
	}

	if(!DATA_PRESENT[NEXT_BUFFER_R]){
		SetEPTxCount(ENDP1,0);
		SetEPTxValid(ENDP1);
	}
}
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/


