/*
 * dcm.h
 *
 *  Created on: 06/set/2013
 *      Author: mauro
 */

#ifndef DCM_H_
#define DCM_H_

#include "common.h"
#include "math.h"

extern void freeIMUUpdate(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz);
extern void getQuaternion(float* q);
extern void quaternionToYawPitchRoll(float *ypr);
#endif /* DCM_H_ */
