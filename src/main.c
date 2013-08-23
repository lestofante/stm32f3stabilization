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
/* Private macro -------------------------------------------------------------*/
#define PI                         (float)     3.14159265f

#define LSM_Acc_Sensitivity_2g     (float)     1.0f            /*!< accelerometer sensitivity with 2 g full scale [LSB/mg] */
#define LSM_Acc_Sensitivity_4g     (float)     0.5f            /*!< accelerometer sensitivity with 4 g full scale [LSB/mg] */
#define LSM_Acc_Sensitivity_8g     (float)     0.25f           /*!< accelerometer sensitivity with 8 g full scale [LSB/mg] */
#define LSM_Acc_Sensitivity_16g    (float)     0.0834f         /*!< accelerometer sensitivity with 12 g full scale [LSB/mg] */

/* Private variables ---------------------------------------------------------*/
RCC_ClocksTypeDef RCC_Clocks;
__IO uint32_t TimingDelay = 0;

float  AccBuffer[3] = {0.0f};

__IO uint8_t DataReady = 0;

uint16_t bytes = 0;
uint8_t *Mouse_Buffer;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*
static uint8_t *getBuffer2 (void)
{
	static int temp[64] = {0};
	int j = 0;

	float asd = asin(3);

	for(j=0; j<64; j++){

		if(j%8==0)
			temp[j]= -32768;

		if(j%8==1)
			temp[j]= -16384;

		if(j%8==2)
			temp[j]= 0;

		if(j%8==3)
			temp[j]= 16384;

		if(j%8==4)
			temp[j]= 32767;

		if(j%8==5)
			temp[j]= 16384;

		if(j%8==6)
			temp[j]= 0;

		if(j%8==7)
			temp[j]= -16384;
	}

	static uint8_t HID_Buffer[128] = {0};
	int i = 0;
	for(i = 0; i<128; i+=2){
		HID_Buffer[i] = (uint8_t)(temp[i/2]>>8);
		HID_Buffer[i+1] = (uint8_t)temp[i/2];
	}

	return HID_Buffer;
}
 */
/**
 * @brief  USBD_HID_GetPos
 * @param  None
 * @retval Pointer to report
 */
/*
static uint8_t *USBD_HID_GetPos (void)
{
	static uint8_t HID_Buffer[16] = {0};

	HID_Buffer[0] = 65;
	HID_Buffer[1] = 66;
	HID_Buffer[2] = 67;
	HID_Buffer[3] = 68;

	HID_Buffer[4] = 69;
	HID_Buffer[5] = 70;
	HID_Buffer[6] = 71;
	HID_Buffer[7] = 72;

	HID_Buffer[8] = 73;
	HID_Buffer[9] = 74;
	HID_Buffer[10] = 75;
	HID_Buffer[11] = 76;

	HID_Buffer[12] = 77;
	HID_Buffer[13] = 78;
	HID_Buffer[14] = 79;
	HID_Buffer[15] = 80;


	return HID_Buffer;
}
 */
/**
 * @brief  Configure the Mems to Accelerometer MEMS.
 * @param  None
 * @retval None
 */
void Acc_Config(void)
{
	LSM303DLHCAcc_InitTypeDef LSM303DLHCAcc_InitStructure;
	LSM303DLHCAcc_FilterConfigTypeDef LSM303DLHCFilter_InitStructure;

	/* Fill the accelerometer structure */
	LSM303DLHCAcc_InitStructure.Power_Mode = LSM303DLHC_NORMAL_MODE;
	LSM303DLHCAcc_InitStructure.AccOutput_DataRate = LSM303DLHC_ODR_50_HZ;
	LSM303DLHCAcc_InitStructure.Axes_Enable= LSM303DLHC_AXES_ENABLE;
	LSM303DLHCAcc_InitStructure.AccFull_Scale = LSM303DLHC_FULLSCALE_2G;
	LSM303DLHCAcc_InitStructure.BlockData_Update = LSM303DLHC_BlockUpdate_Continous;
	LSM303DLHCAcc_InitStructure.Endianness=LSM303DLHC_BLE_LSB;
	LSM303DLHCAcc_InitStructure.High_Resolution=LSM303DLHC_HR_ENABLE;
	/* Configure the accelerometer main parameters */
	LSM303DLHC_AccInit(&LSM303DLHCAcc_InitStructure);

	/* Fill the accelerometer LPF structure */
	LSM303DLHCFilter_InitStructure.HighPassFilter_Mode_Selection =LSM303DLHC_HPM_NORMAL_MODE;
	LSM303DLHCFilter_InitStructure.HighPassFilter_CutOff_Frequency = LSM303DLHC_HPFCF_16;
	LSM303DLHCFilter_InitStructure.HighPassFilter_AOI1 = LSM303DLHC_HPF_AOI1_DISABLE;
	LSM303DLHCFilter_InitStructure.HighPassFilter_AOI2 = LSM303DLHC_HPF_AOI2_DISABLE;

	/* Configure the accelerometer LPF main parameters */
	LSM303DLHC_AccFilterConfig(&LSM303DLHCFilter_InitStructure);
}

/**
 * @brief Read LSM303DLHC output register, and calculate the acceleration ACC=(1/SENSITIVITY)* (out_h*256+out_l)/16 (12 bit rappresentation)
 * @param pnData: pointer to float buffer where to store data
 * @retval None
 */
void Acc_ReadData(float* pfData)
{
	int16_t pnRawData[3];
	uint8_t ctrlx[2];
	float LSM_Acc_Sensitivity = LSM_Acc_Sensitivity_2g;
	uint8_t buffer[6], cDivider;
	uint8_t i = 0;

	/* Read the register content */
	LSM303DLHC_Read(ACC_I2C_ADDRESS, LSM303DLHC_CTRL_REG4_A, ctrlx,2);
	LSM303DLHC_Read(ACC_I2C_ADDRESS, LSM303DLHC_OUT_X_L_A, buffer, 6);


	if(ctrlx[1]&0x40)
		cDivider=64;
	else
		cDivider=16;

	/* check in the control register4 the data alignment*/
	if(!(ctrlx[0] & 0x40) || (ctrlx[1] & 0x40)) /* Little Endian Mode or FIFO mode */
	{
		for(i=0; i<3; i++)
		{
			pnRawData[i]=((int16_t)((uint16_t)buffer[2*i+1] << 8) + buffer[2*i])/cDivider;
		}
	}
	else /* Big Endian Mode */
	{
		for(i=0; i<3; i++)
			pnRawData[i]=((int16_t)((uint16_t)buffer[2*i] << 8) + buffer[2*i+1])/cDivider;
	}
	/* Read the register content */
	LSM303DLHC_Read(ACC_I2C_ADDRESS, LSM303DLHC_CTRL_REG4_A, ctrlx,2);


	if(ctrlx[1]&0x40)
	{
		/* FIFO mode */
		LSM_Acc_Sensitivity = 0.25;
	}
	else
	{
		/* normal mode */
		/* switch the sensitivity value set in the CRTL4*/
		switch(ctrlx[0] & 0x30)
		{
		case LSM303DLHC_FULLSCALE_2G:
			LSM_Acc_Sensitivity = LSM_Acc_Sensitivity_2g;
			break;
		case LSM303DLHC_FULLSCALE_4G:
			LSM_Acc_Sensitivity = LSM_Acc_Sensitivity_4g;
			break;
		case LSM303DLHC_FULLSCALE_8G:
			LSM_Acc_Sensitivity = LSM_Acc_Sensitivity_8g;
			break;
		case LSM303DLHC_FULLSCALE_16G:
			LSM_Acc_Sensitivity = LSM_Acc_Sensitivity_16g;
			break;
		}
	}

	/* Obtain the mg value for the three axis */
	for(i=0; i<3; i++)
	{
		pfData[i]=(float)pnRawData[i]/LSM_Acc_Sensitivity;
	}
}

void writeSensor(uint8_t lettera, uint16_t read[3]){
	uint8_t data[15];
	uint8_t index =0;

	//2 times
	data[index++] = lettera;
	data[index++] = lettera;

	//now data
	uint8_t i;
	for (i=0;i<3;i++){
		data[index++] = (read[i]>>8);
		data[index++] = read[i];
	}

	USB_write(data,index);
}

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
/*
	uint16_t toWrite[3];
	toWrite[0] = 0;
	toWrite[1] = 0;
	toWrite[2] = 0;
	toWrite[0] = GetENDPOINT(ENDP1);
	uint8_t *ptr;
	ptr = (uint8_t*)&toWrite;

	UserToPMABufferCopy(ptr, ENDP1_BUF0, 6);
	SetEPDblBuffCount(ENDP1, EP_DBUF_IN, 6);
*/
	SetEPTxValid(ENDP1);
	DATA_PRESENT[0] = 1;

	while (1)
	{
		cicle++;
		if(cicle%1000==0){
			USB_write((uint8_t*)&cicle, 2);
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
