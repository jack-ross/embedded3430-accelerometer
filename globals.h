/*
 * globals.h
 *
 *  Created on: Feb 8, 2016
 *      Author: jross_000
 */

#ifndef ACCELEROMETER1_GLOBALS_H_
#define ACCELEROMETER1_GLOBALS_H_


extern unsigned int g1msTimeout;// This variable is incremented by the interrupt handler and
// decremented by a software call in the main loop.
extern unsigned int g1msTimer;
extern unsigned int pwmHex;

//ADC
#define ADC10_A0_BIT	BIT0	// Port pin bit location for X
#define ADC10_A1_BIT	BIT1	// Port pin bit location for Y
#define ADC10_A2_BIT	BIT2	// Port pin bit location for Z
unsigned int samples[3][8];

//Accel
extern int CoordinateX, CoordinateXMax, CoordinateXMin, CoordinateX0;
extern int CoordinateY, CoordinateYMax, CoordinateYMin, CoordinateY0;
extern int CoordinateZ, CoordinateZMax, CoordinateZMin, CoordinateZ0;


#endif /* ACCELEROMETER1_GLOBALS_H_ */
