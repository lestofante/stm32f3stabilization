/*
 * sensors.c
 *
 *  Created on: Nov 17, 2012
 *      Author: andrei
 */

#include "stm32f3_discovery_lsm303dlhc.h"
#include "stm32f3_discovery_l3gd20.h"



#define L3G_Sensitivity_250dps     (float)   114.285f         /*!< gyroscope sensitivity with 250 dps full scale [LSB/dps] */
#define L3G_Sensitivity_500dps     (float)    57.1429f        /*!< gyroscope sensitivity with 500 dps full scale [LSB/dps] */
#define L3G_Sensitivity_2000dps    (float)    14.285f	      /*!< gyroscope sensitivity with 2000 dps full scale [LSB/dps] */

#define LSM_Acc_Sensitivity_2g     (float)     1.0f            /*!< accelerometer sensitivity with 2 g full scale [LSB/mg] */
#define LSM_Acc_Sensitivity_4g     (float)     0.5f            /*!< accelerometer sensitivity with 4 g full scale [LSB/mg] */
#define LSM_Acc_Sensitivity_8g     (float)     0.25f           /*!< accelerometer sensitivity with 8 g full scale [LSB/mg] */
#define LSM_Acc_Sensitivity_16g    (float)     0.0834f         /*!< accelerometer sensitivity with 12 g full scale [LSB/mg] */


float magnetometerZtoXY = 1;
uint8_t buffer[6];
uint8_t i = 0;
/**
 * @brief  Configure the Mems to gyroscope application.
 * @param  None
 * @retval None
 */
void Gyro_Config(void)
{


	L3GD20_InitTypeDef L3GD20_InitStructure;
	L3GD20_FilterConfigTypeDef L3GD20_FilterStructure;

/*
 *  MY SPECIAL VALUE
 */
	L3GD20_InitStructure.Power_Mode = L3GD20_MODE_ACTIVE;
	L3GD20_InitStructure.Output_DataRate = L3GD20_OUTPUT_DATARATE_4; //ODR = 760Hz
	L3GD20_InitStructure.Axes_Enable = L3GD20_AXES_ENABLE;
	L3GD20_InitStructure.Band_Width = L3GD20_BANDWIDTH_4; //CUT off 100
	L3GD20_InitStructure.BlockData_Update = L3GD20_BlockDataUpdate_Single;
	L3GD20_InitStructure.Endianness = L3GD20_BLE_MSB;
	L3GD20_InitStructure.Full_Scale = L3GD20_FULLSCALE_500;
/*
 *  DEFAULT VALUE
 */
/*
	L3GD20_InitStructure.Power_Mode = L3GD20_MODE_ACTIVE;
	L3GD20_InitStructure.Output_DataRate = L3GD20_OUTPUT_DATARATE_1; //95Hz, cut off 25
	L3GD20_InitStructure.Axes_Enable = L3GD20_AXES_ENABLE;
	L3GD20_InitStructure.Band_Width = L3GD20_BANDWIDTH_4;
	L3GD20_InitStructure.BlockData_Update = L3GD20_BlockDataUpdate_Single;
	L3GD20_InitStructure.Endianness = L3GD20_BLE_MSB;
	L3GD20_InitStructure.Full_Scale = L3GD20_FULLSCALE_500;
*/

	L3GD20_Init(&L3GD20_InitStructure);

	L3GD20_FilterStructure.HighPassFilter_Mode_Selection =L3GD20_HPM_NORMAL_MODE_RES;
	L3GD20_FilterStructure.HighPassFilter_CutOff_Frequency = L3GD20_HPFCF_0; //51.4Hz
	L3GD20_FilterConfig(&L3GD20_FilterStructure) ;

	L3GD20_FilterCmd(L3GD20_HIGHPASSFILTER_ENABLE);

	  /* Configure Mems L3GD20 */

}

/**
 * @brief  Calculate the angular Data rate Gyroscope.
 * @param  pfData : Data out pointer
 * @retval None
 */
void Gyro_ReadAngRate (uint16_t* pfData)
{
	uint8_t tmpbuffer[6] ={0};
	//int16_t RawData[3] = {0};
	uint8_t tmpreg = 0;
	//float sensitivity = 0;
	int i =0;

	L3GD20_Read(&tmpreg,L3GD20_CTRL_REG4_ADDR,1);

	L3GD20_Read(tmpbuffer,L3GD20_OUT_X_L_ADDR,6);


	for(i=0; i<3; i++)
	{
		pfData[i]=(int16_t)(((uint16_t)tmpbuffer[2*i+1] << 8) + tmpbuffer[2*i]);
	}
}

/**
 * @brief  Configure the Mems to compass application.
 * @param  None
 * @retval None
 */
void Compass_Config(void)
{
	LSM303DLHCMag_InitTypeDef LSM303DLHC_InitStructure;
	LSM303DLHCAcc_InitTypeDef LSM303DLHCAcc_InitStructure;
	LSM303DLHCAcc_FilterConfigTypeDef LSM303DLHCFilter_InitStructure;

	/* Configure MEMS magnetometer main parameters: temp, working mode, full Scale and Data rate */

	LSM303DLHC_InitStructure.Temperature_Sensor = LSM303DLHC_TEMPSENSOR_DISABLE;
	LSM303DLHC_InitStructure.MagOutput_DataRate =LSM303DLHC_ODR_220_HZ;
	LSM303DLHC_InitStructure.MagFull_Scale = LSM303DLHC_FS_1_3_GA;
	magnetometerZtoXY = LSM303DLHC_M_SENSITIVITY_Z_1_3Ga / LSM303DLHC_M_SENSITIVITY_XY_1_3Ga;//because z has different sensitivity

	LSM303DLHC_InitStructure.Working_Mode = LSM303DLHC_SINGLE_CONVERSION;
	LSM303DLHC_MagInit(&LSM303DLHC_InitStructure);

	/* Fill the accelerometer structure */
/*
 *  THIS IS HOW I WANT USE IT
 */

	LSM303DLHCAcc_InitStructure.Power_Mode = LSM303DLHC_NORMAL_MODE;
	LSM303DLHCAcc_InitStructure.AccOutput_DataRate = LSM303DLHC_ODR_400_HZ;
	LSM303DLHCAcc_InitStructure.Axes_Enable= LSM303DLHC_AXES_ENABLE;
	LSM303DLHCAcc_InitStructure.AccFull_Scale = LSM303DLHC_FULLSCALE_4G;
	LSM303DLHCAcc_InitStructure.BlockData_Update = LSM303DLHC_BlockUpdate_Single;
	LSM303DLHCAcc_InitStructure.Endianness=LSM303DLHC_BLE_MSB;
	LSM303DLHCAcc_InitStructure.High_Resolution=LSM303DLHC_HR_ENABLE;

/*
 *  THIS WAS THE STANDARD SETUP
 */
/*
 	LSM303DLHCAcc_InitStructure.Power_Mode = LSM303DLHC_NORMAL_MODE;
	LSM303DLHCAcc_InitStructure.AccOutput_DataRate = LSM303DLHC_ODR_50_HZ;
	LSM303DLHCAcc_InitStructure.Axes_Enable= LSM303DLHC_AXES_ENABLE;
	LSM303DLHCAcc_InitStructure.AccFull_Scale = LSM303DLHC_FULLSCALE_4G;
	LSM303DLHCAcc_InitStructure.BlockData_Update = LSM303DLHC_BlockUpdate_Single;
	LSM303DLHCAcc_InitStructure.Endianness=LSM303DLHC_BLE_MSB;
	LSM303DLHCAcc_InitStructure.High_Resolution=LSM303DLHC_HR_ENABLE;
*/
	LSM303DLHC_AccInit(&LSM303DLHCAcc_InitStructure);


	LSM303DLHCFilter_InitStructure.HighPassFilter_Mode_Selection =LSM303DLHC_HPM_NORMAL_MODE;
	LSM303DLHCFilter_InitStructure.HighPassFilter_CutOff_Frequency = LSM303DLHC_HPFCF_16;
	LSM303DLHCFilter_InitStructure.HighPassFilter_AOI1 = LSM303DLHC_HPF_AOI1_DISABLE;
	LSM303DLHCFilter_InitStructure.HighPassFilter_AOI2 = LSM303DLHC_HPF_AOI2_DISABLE;


	LSM303DLHC_AccFilterConfig(&LSM303DLHCFilter_InitStructure);
}

/**
 * @brief Read LSM303DLHC output register, and calculate the acceleration ACC=(1/SENSITIVITY)* (out_h*256+out_l)/16 (12 bit rappresentation)
 * @param pnData: pointer to float buffer where to store data
 * @retval None
 */
void Compass_ReadAcc(uint16_t* pfData)
{

	/* Read the register content */
	LSM303DLHC_Read(ACC_I2C_ADDRESS, LSM303DLHC_OUT_X_L_A, buffer, 6);

	for(i=0; i<3; i++)
	{
		pfData[i]=((int16_t)((uint16_t)buffer[2*i+1] << 8) + buffer[2*i]);
	}

}

/**
 * @brief  calculate the magnetic field Magn.
 * @param  pfData: pointer to the data out
 * @retval None
 */
void Compass_ReadMag (uint16_t* pfData)
{


	/*
	 * maybe i'm wrong, but this is BULLSHIT!
	LSM303DLHC_Read(MAG_I2C_ADDRESS, LSM303DLHC_OUT_X_H_M, buffer, 1);
	LSM303DLHC_Read(MAG_I2C_ADDRESS, LSM303DLHC_OUT_X_L_M, buffer+1, 1);
	LSM303DLHC_Read(MAG_I2C_ADDRESS, LSM303DLHC_OUT_Y_H_M, buffer+2, 1);
	LSM303DLHC_Read(MAG_I2C_ADDRESS, LSM303DLHC_OUT_Y_L_M, buffer+3, 1);
	LSM303DLHC_Read(MAG_I2C_ADDRESS, LSM303DLHC_OUT_Z_H_M, buffer+4, 1);
	LSM303DLHC_Read(MAG_I2C_ADDRESS, LSM303DLHC_OUT_Z_L_M, buffer+5, 1);
	*/
	LSM303DLHC_Read(MAG_I2C_ADDRESS, LSM303DLHC_OUT_X_H_M, buffer, 6);
	/* Switch the sensitivity set in the CRTLB*/

	for(i=0; i<3; i++)
	{
		pfData[i]=((int16_t)((uint16_t)buffer[2*i] << 8) + buffer[2*i+1]);
	}
	pfData[1]*= magnetometerZtoXY; //because Z has different sensitivity
}



/**
 * @brief  Basic management of the timeout situation.
 * @param  None.
 * @retval None.
 */
uint32_t LSM303DLHC_TIMEOUT_UserCallback(void)
{
	//printf("ERROR!!! LSM303DLHC TIMEOUT !!!!!!!!");
	return 0;
}

/**
 * @brief  Basic management of the timeout situation.
 * @param  None.
 * @retval None.
 */
uint32_t L3GD20_TIMEOUT_UserCallback(void)
{
	//printf("ERROR!!! L3GD20 TIMEOUT !!!!!!!!");
	return 0;
}
