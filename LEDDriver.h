/*
 * LEDDriver.h
 *
 *  Created on: Mar 29, 2016
 *      Author: jross_000
 */

#ifndef LEDDRIVER_H_
#define LEDDRIVER_H_


#define SCLK BIT5 	// P1.5
#define MOSI BIT7 	// P1.7
#define XLAT BIT0 	// P2.0
#define BLANK BIT4	// P1.4

void startLEDDisplay(void);

#endif /* LEDDRIVER_H_ */
