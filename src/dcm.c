/*
 * dcm.c
 *
 *  Created on: 06/set/2013
 *      Author: mauro
 */

#include "dcm.h"

float sampleFreq = 160;
float q0 = 1, q1 = 0, q2 = 0, q3 = 0;
float twoKp = (2.0f * 0.1f);
float twoKi = 2.0f * 0.0f;

//long lastUp = System.nanoTime();
uint32_t countImuUpdate = -1, lastFreqUp;

float qPred1;
float qPred2;
float qPred3;
float qPred4;

float integralFBx;
float integralFBy;
float integralFBz;

float invSqrt(float n){
	return 1/sqrt(n);
}

void getQuaternion(float* q){
	q[0] = q0;
	q[1] = q1;
	q[2] = q2;
	q[3] = q3;
}

void freeIMUUpdate(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz) {

	/* DYANMIC FREQUENCY! */
	if (countImuUpdate == -1) { //just the first time!
		lastFreqUp = micros();
	}
	countImuUpdate++;
	if (micros() - lastFreqUp >= 1000) {
		//System.out.println("Frequenza: " + count);
		sampleFreq = countImuUpdate;
		countImuUpdate = 0;
		lastFreqUp = micros();
	}
	/* END DYANMIC FREQUENCY! */

	float recipNorm;
	float q0q0, q0q1, q0q2, q0q3, q1q1, q1q2, q1q3, q2q2, q2q3, q3q3;
	float halfex = 0.0f, halfey = 0.0f, halfez = 0.0f;
	float qa, qb, qc;

	// Auxiliary variables to avoid repeated arithmetic
	q0q0 = q0 * q0;
	q0q1 = q0 * q1;
	q0q2 = q0 * q2;
	q0q3 = q0 * q3;
	q1q1 = q1 * q1;
	q1q2 = q1 * q2;
	q1q3 = q1 * q3;
	q2q2 = q2 * q2;
	q2q3 = q2 * q3;
	q3q3 = q3 * q3;

	// Use magnetometer measurement only when valid (avoids NaN in magnetometer normalisation)
	if (mx != 0.0f || my != 0.0f || mz != 0.0f) {
		float hx, hy, bx, bz;
		float halfwx, halfwy, halfwz;

		// Normalise magnetometer measurement
		recipNorm = invSqrt(mx * mx + my * my + mz * mz);
		mx *= recipNorm;
		my *= recipNorm;
		mz *= recipNorm;

		// Reference direction of Earth's magnetic field
		hx = 2.0f
				* (mx * (0.5f - q2q2 - q3q3) + my * (q1q2 - q0q3)
						+ mz * (q1q3 + q0q2));
		hy = 2.0f
				* (mx * (q1q2 + q0q3) + my * (0.5f - q1q1 - q3q3)
						+ mz * (q2q3 - q0q1));
		bx = sqrt(hx * hx + hy * hy);
		bz = 2.0f
				* (mx * (q1q3 - q0q2) + my * (q2q3 + q0q1)
						+ mz * (0.5f - q1q1 - q2q2));

		// Estimated direction of magnetic field
		halfwx = bx * (0.5f - q2q2 - q3q3) + bz * (q1q3 - q0q2);
		halfwy = bx * (q1q2 - q0q3) + bz * (q0q1 + q2q3);
		halfwz = bx * (q0q2 + q1q3) + bz * (0.5f - q1q1 - q2q2);

		float norm = invSqrt(
				halfwx * halfwx + halfwy * halfwy + halfwz * halfwz);
		halfwx *= norm;
		halfwy *= norm;
		halfwz *= norm;
		/*
		 System.out.println("Ref:\t"+bx+" "+bz);
		 System.out.println("Est:\t"+halfwx+" "+halfwy+" "+halfwz+" "+Math.sqrt(halfwx*halfwx+halfwy*halfwy+halfwz*halfwz) );
		 System.out.println("Real:\t"+mx+" "+my+" "+mz+" "+Math.sqrt(mx*mx+my*my+mz*mz));
		 System.out.println();
		 */
		// Error is sum of cross product between estimated direction and measured direction of field vectors
		halfex += (my * halfwz - mz * halfwy);
		halfey += (mz * halfwx - mx * halfwz);
		halfez += (mx * halfwy - my * halfwx);
		/*
		 System.out.println("Ref:\t"+bx+" "+bz);
		 System.out.println("Est:\t"+halfwx+" "+halfwy+" "+halfwz+" "+Math.sqrt(halfwx*halfwx + halfwy*halfwy + halfwz*halfwz) );
		 System.out.println("Real:\t"+mx+" "+my+" "+mz+" "+Math.sqrt(mx*mx + my*my + mz*mz));
		 System.out.println("Err:\t"+halfex+" "+halfey+" "+halfez+" "+Math.sqrt(halfex*halfex + halfey*halfey + halfez*halfez));
		 System.out.println();
		 */
	}

	// Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
	if (ax != 0.0f || ay != 0.0f || az != 0.0f) {
		float halfvx, halfvy, halfvz;

		// Normalise accelerometer measurement
		recipNorm = invSqrt(ax * ax + ay * ay + az * az);
		ax *= recipNorm;
		ay *= recipNorm;
		az *= recipNorm;

		// Estimated direction of gravity
		halfvx = q1q3 - q0q2;
		halfvy = q0q1 + q2q3;
		halfvz = q0q0 - 0.5f + q3q3;

		// Error is sum of cross product between estimated direction and measured direction of field vectors
		halfex += (ay * halfvz - az * halfvy);
		halfey += (az * halfvx - ax * halfvz);
		halfez += (ax * halfvy - ay * halfvx);

		//System.out.println("HalfV|"+"halfvx: "+halfvx+" halfvy: "+halfvy+" halfvz: "+halfvz);
		//System.out.println("Halferr|"+"halfex: "+halfex+" halfey: "+halfey+" halfez: "+halfez);
	}

	// Apply feedback only when valid data has been gathered from the accelerometer or magnetometer
	if (halfex != 0.0f || halfey != 0.0f || halfez != 0.0f) {
		// Compute and apply integral feedback if enabled
		if (twoKi > 0.0f) {
			integralFBx += twoKi * halfex * (1.0f / sampleFreq); // integral error scaled by Ki
			integralFBy += twoKi * halfey * (1.0f / sampleFreq);
			integralFBz += twoKi * halfez * (1.0f / sampleFreq);
			gx += integralFBx;  // apply integral feedback
			gy += integralFBy;
			gz += integralFBz;
			//System.out.println("Integrating!");
		} else {
			integralFBx = 0.0f; // prevent integral windup
			integralFBy = 0.0f;
			integralFBz = 0.0f;
		}

		// Apply proportional feedback
		gx += twoKp * halfex;
		gy += twoKp * halfey;
		gz += twoKp * halfez;
	}

	// Integrate rate of change of quaternion
	gx *= (0.5f * (1.0f / sampleFreq));   // pre-multiply common factors
	gy *= (0.5f * (1.0f / sampleFreq));
	gz *= (0.5f * (1.0f / sampleFreq));
	qa = q0;
	qb = q1;
	qc = q2;
	q0 += (-qb * gx - qc * gy - q3 * gz);
	q1 += (qa * gx + qc * gz - q3 * gy);
	q2 += (qa * gy - qb * gz + q3 * gx);
	q3 += (qa * gz + qb * gy - qc * gx);

	// Normalise quaternion
	recipNorm = invSqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
	q0 *= recipNorm;
	q1 *= recipNorm;
	q2 *= recipNorm;
	q3 *= recipNorm;

	/*
	 System.out.print(q0+" ");
	 System.out.print(q1+" ");
	 System.out.print(q2+" ");
	 System.out.print(q3+" ");
	 System.out.println();
	 */
}
