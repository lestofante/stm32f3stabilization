/*
 * sensors.h
 *
 *  Created on: Nov 17, 2012
 *      Author: andrei
 */

#ifndef SENSORS_H_
#define SENSORS_H_


extern void Gyro_Config(void);
extern uint8_t Gyro_ReadAngRate (uint16_t* pfData);
extern void Compass_Config(void);
extern uint8_t Compass_ReadAcc(uint16_t* pfData);
extern uint8_t Compass_ReadMag (uint16_t* pfData);

#endif /* SENSORS_H_ */
