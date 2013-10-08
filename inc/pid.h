/*
 * pid.h
 *
 *  Created on: 10/set/2013
 *      Author: mauro
 */

#ifndef PID_H_
#define PID_H_

#include "common.h"
#include "stddef.h"
#include "stdlib.h"

#define MAX_PID_NUMBER 4

struct PidStruct {
	//put data here
	float *myInput;
	float *mySetpoint;
	//get result here
	float *myOutput;

	//one -time inizialization
	float *kp;
	float *ki;
	float *kd;

	float outMax;
	float outMin;

	//internal data
	float ITerm;
	float lastInput;
};

//struct PidStruct *head;
extern struct PidStruct arrayOfPid[MAX_PID_NUMBER];

extern uint8_t numberOfPid;

uint32_t lastTimePID;

uint32_t SampleTimePID; //10ms

extern void init(uint32_t sample_time);
extern struct PidStruct* addPid(float *in, float *set, float *out, float *kp, float *ki, float *kd, float outMin, float outMax);
extern uint8_t computePid();

#endif /* PID_H_ */
