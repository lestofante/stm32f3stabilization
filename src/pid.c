/*
 * pid.c
 *
 *  Created on: 10/set/2013
 *      Author: mauro
 */

#include "pid.h"

uint8_t numberOfPid;
struct PidStruct arrayOfPid[];

void init(uint32_t sample_time){
	lastTimePID = micros();
	SampleTimePID = sample_time;
	//head = NULL;
	numberOfPid = 0;
}


struct PidStruct* addPid(float *in, float *set, float *out, float *kp, float *ki, float *kd, float outMin, float outMax){

	struct PidStruct *pidStruct;
/*
	pidStruct = head;

	while (pidStruct != NULL){
		pidStruct = pidStruct->next;
	}

	pidStruct = malloc(sizeof(struct PidStruc));
*/
	if (numberOfPid < MAX_PID_NUMBER){
		pidStruct = &(arrayOfPid[numberOfPid]);
		numberOfPid++;

		pidStruct->myInput = in;
		pidStruct->myOutput = out;
		pidStruct->mySetpoint = set;

		pidStruct->kp = kp;
		pidStruct->ki = ki;
		pidStruct->kd = kd;

		pidStruct->outMax = outMax;
		pidStruct->outMin = outMin;

		pidStruct->ITerm = 0;
		pidStruct->lastInput = 0;

		return pidStruct;
	}

	return NULL;
}

/* Compute() **********************************************************************
 *     This, as they say, is where the magic happens.  this function should be called
 *   fast as you can.  the function will decide for itself whether a new
 *   pid Output needs to be computed.  returns true when the output is computed,
 *   false when nothing has been done.
 **********************************************************************************/
uint8_t computePid(){

	unsigned long now = micros();
	unsigned long timeChange = (now - lastTimePID);
	//all PID has the same update time!
	if(timeChange>=SampleTimePID){

		struct PidStruct *pidStruct;
		//pidStruct = head;

		//while (pidStruct != NULL){
		uint8_t i;
		for (i=0; i< numberOfPid; i++){
			//well fuck you array, i'd like so much my list, that i won't change my code for you
			pidStruct = &(arrayOfPid[i]);

			//Compute all the working error variables
			float input = *(pidStruct->myInput);
			float error = *(pidStruct->mySetpoint) - input;
			pidStruct->ITerm+= ( *(pidStruct->ki) * error);

			//prevent integral windup
			if(pidStruct->ITerm > pidStruct->outMax)
				pidStruct->ITerm= pidStruct->outMax;
			else if(pidStruct->ITerm < pidStruct->outMin)
				pidStruct->ITerm= pidStruct->outMin;

			float dInput = (input - pidStruct->lastInput);

			//Compute PID Output
			float output = *(pidStruct->kp) * error + pidStruct->ITerm- *(pidStruct->kd) * dInput;

			//constrains value, prevent pid overshoot
			if(output > pidStruct->outMax)
				output = pidStruct->outMax;
			else if(output < pidStruct->outMin)
				output = pidStruct->outMin;
			*(pidStruct->myOutput) = output;

			//Remember some variables for next time
			pidStruct->lastInput = input;

			//pidStruct = pidStruct->next;
		}
		lastTimePID = now;
	}else
		return 0;

	return 1;
}
