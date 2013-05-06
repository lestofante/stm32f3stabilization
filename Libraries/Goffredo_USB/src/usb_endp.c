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
#include "usb_endp.h"

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


__IO uint8_t contatoreBufferUSB=0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  EP1 OUT Callback Routine.
  * @param  None
  * @retval None
  */
void EP1_IN_Callback(void)
{
	uint8_t addr = 0;

	  if (GetENDPOINT(ENDP1) & EP_DTOG_RX)
	  {
		  addr = 0x14c;
	  }
	  else
	  {
		  addr = 0x098;
	  }

	  FreeUserBuffer(ENDP1, EP_DBUF_IN);

	  //__IO uint8_t *tmpBuffer = scrivimi;


	  UserToPMABufferCopy(scrivimi, addr, BUFFER_SIZE);

	  uint8_t i;
	  for (i=0; i < BUFFER_SIZE;i++){
	  	  scrivimi[i] = 0;
	  }

	  contatoreBufferUSB = 0;

	  SetEPTxValid(ENDP1);
}


/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/ 

