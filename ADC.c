#include "ADC.h"
#include "globals.h"
#include "LED.h"

unsigned int curAvg = 0;
int curSampleIndex =0;
//unsigned int ADCReading;

void ConfigureADC(void)
{
	/* ADC10 Control register 1
	 * INCH_0 = choose analog input A0 (X)
	 *
	 * ADC10DIV_3 = divide ADC clock by 4 (for temperature sensor)
	 */
	ADC10CTL1 = (INCH_0 | ADC10DIV_7);
	ADC10AE0 |= BIT0; 	// Enable ADC10 A7

	/* ADC10 Control register 0
	 * SREF_0: Choose Vcc reference for analog inputs
	 * SREF_1: Choose 1.5V reference for temperature sensor
	 *
	 * ADC10SHT_3 = 64 × ADC10CLKs (for temperature sensor)
	 */

	ADC10CTL0 = (SREF_0 | ADC10SHT_3 | ADC10ON);


	_delay_cycles(1000);			// Wait for the ADC to adjust and stabilize
	ADC10CTL0 |=  (ENC | ADC10SC);	// Enable and begin sampling and conversion


	P1DIR &= ~ BIT0;
	P1SEL |= BIT0;

}

/*
 * Returns the ADC Value of the specified input
 * 0 - X
 * 1 - Y
 * 2 - Z
 */
unsigned int getADCConversion(int analogInput){

	int ADCReading = 12345;
	//ADC10CTL0 &=  ~ENC;

	/*switch(analogInput){
			case 0:
				ADC10AE0 |= ADC10_A0_BIT;
				break;
			case 1:
				ADC10AE0 |= ADC10_A1_BIT;
				break;
			case 2:
				ADC10AE0 |= ADC10_A2_BIT;
				break;
			default:
				_nop();//ADC10AE0 |= ADC10_A0_BIT;
	}
*/
	//ADC10CTL0 |= ENC;
	//wait until the conversion is done
	while(ADC10CTL1 & BUSY);
	//read the register
	ADCReading=ADC10MEM;

	//Turn off conversion
	ADC10CTL0&=~ENC;
	return ADCReading;
}

void filter(int analogInput, unsigned int ADCVal){
	//TURN_ON_LED1;

	//equivalent to i = (i+1) % 8
	curSampleIndex = (curSampleIndex+1) & 0x7;
	samples[analogInput][curSampleIndex] = ADCVal;

	//get cur temp
	int i, curSum =0;
	for (i=0; i <8 ; ++i){
		curSum+= samples[analogInput][i];
	}
	//divide by 8
	curAvg = curSum >> 3;

	//TURN_OFF_LED1;
	return;
}



