/*
 * LEDDriver.c
 *
 *  Created on: Mar 29, 2016
 *      Author: jross_000
 */

#include <msp430.h>
#include "globals.h"
#include "LEDDriver.h"
#include "LED.h"

void startLEDDisplay(void) {
	ConfigureLEDDisplay();
	setDisplay(0x80);
}


void ConfigureLEDDisplay(void) {

	P1DIR |= BLANK;		// Enable Output on BLANK PORT P1.4
	P1DIR |= SCLK;		// Enable Output on SCLK PORT P1.5
	P1DIR |= MOSI;		// Enable Output on MOSI PORT P1.7
	P2DIR |= XLAT;		// Enable Output on Latch port P2.0

	// set blank low to enable LEDs
	P1OUT &= ~BLANK;
}

void setDisplay(unsigned char byte_value) {

	// set latch low
	P2OUT &= ~XLAT;
	P1OUT &= ~MOSI; // default output to zero

	int i;

	for (i = 0; i < 8; i++) {
		P1OUT &= ~SCLK;	// set clock low

		if (byte_value & 0x80){
			P1OUT |= MOSI;
		} else {
			P1OUT &= ~MOSI;
		}
		byte_value <<= 1;

		P1OUT |= SCLK;	// set clock high
	}

	P2OUT |= XLAT;
}
