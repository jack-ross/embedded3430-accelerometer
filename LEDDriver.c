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

unsigned char LEDIntensities[8];
// division of pwm (0 to 20) each is 1 ms
unsigned char pwmCurrentDivision;


void startLEDDisplay(void) {
	ConfigureLEDDisplay();
	setDisplay(0x40);
}


void ConfigureLEDDisplay(void) {

	P1DIR |= BLANK;		// Enable Output on BLANK PORT P1.4
	P1DIR |= SCLK;		// Enable Output on SCLK PORT P1.5
	P1DIR |= MOSI;		// Enable Output on MOSI PORT P1.7
	P2DIR |= XLAT;		// Enable Output on Latch port P2.0

	// set blank low to enable LEDs
	P1OUT &= ~BLANK;

	// set all LEDs to full brightness
	unsigned char i;
	for (i = 0; i < 8; ++i)
	{
		setIntensity(i, 2*i);
	}
}

void turnOffLEDs() {
	int i;
	for ( i = 0; i < 8; ++i)
	{
		LEDIntensities[i] = 0;
	}
}

void turnOnLED(unsigned char LEDNumber) {
	turnOffLEDs();
	LEDIntensities[LEDNumber] = 20;
}

/** 
 *	Set LED Intensity
 *	
 *	Set 1 of the 8 LEDs to an intensity between
 *	0% and 100% in divisions of 5%
 *	@param LEDNumber the (0 to 7) of the LED to set where
 *					 0 is Northern most LED, 1 is NE, 2 is E...
 *	@param intensity is 0 to 20 where 0 = 0% (off), 15 = 75%, 20 = 100%
 *
 */

void setIntensity(unsigned char LEDNumber, unsigned char intensity) {
	if (intensity < 0) {
		LEDIntensities[LEDNumber] = 0;
		return;
	}
	if (intensity > 20) {
		LEDIntensities[LEDNumber] = 20;
		return;
	}

	LEDIntensities[LEDNumber] = intensity;
}

/**
 *	Control LED brightness
 *
 *	Controll each LED's brightness my sending proper byte array
 * 	LED PWM period is 20 ms 
 * 	LEDs are turned of at LEDIntensity[i]*2 (ms)
 * 	i.e if LED3 is 40% and all others are off, 0010 0000 would be sent 
 * 	from 0 <= g100nsTimer < 8 and 0000 0000 from 8 <= g100nsTimer <= 20
 */

void ledPWM() {

	// data to send indicating LED status for all 8 LEDs
	// i.e 0xF0 would turn on the North LED
	unsigned char data = 0x00;

	// if LED intensity 
	unsigned char i;
	for (i = 0; i < 8; ++i)
	{
		if (pwmCurrentDivision < LEDIntensities[i])
		{
			data |= 0x1 << (7-i);
		}
	}

	pwmCurrentDivision++;
	if (pwmCurrentDivision == 20) pwmCurrentDivision = 0;

	setDisplay(data);
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
