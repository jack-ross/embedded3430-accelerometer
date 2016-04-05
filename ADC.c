#include "ADC.h"
#include "globals.h"
#include "LED.h"

unsigned int curAvg = 0;
int curSampleIndex =0;
//unsigned int ADCReading;

void ConfigureADC(void)
{
	ADC10CTL1 = INCH_2 + CONSEQ_3;            // A2/A1/A0, repeat multi channel
	ADC10CTL0 = ADC10SHT_2 + MSC + ADC10ON + ADC10IE;
	ADC10AE0 = (BIT0 + BIT1 + BIT2);          // P1.0,1, 2 Analog enable
	ADC10DTC1 = 0x03;                         // number of conversions
}

/*
 * Sets the samples array with the values read and converted
 * from the accelerometer
 *
 */
void getADCValues(){

	ADC10CTL0 &= ~ENC;
	while (ADC10CTL1 & BUSY);               // Wait if ADC10 core is active
    ADC10SA = (int) &(samples);        // Data buffer start
	ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion ready
    __bis_SR_register(CPUOFF + GIE);        // LPM0, ADC10_ISR will force exit

    _NOP();                                 // space for debugger
    _NOP();                                 // Set Breakpoint here to read ADC

	return ;
}


// ADC10 interrupt service routine
// Needed to read the accellerometer
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR (void)
{
  __bic_SR_register_on_exit(CPUOFF);        // Clear CPUOFF bit from 0(SR)
}


/*
 * Returns an average of one of the columns in the samples array
 * Each axis corresponds to an index
 *
 * A return value of -1 means that something went wrong
 *
 * @param  analogInput : 0 -> Z, 1 -> Y, 2 -> X
 */
int filter(int analogInput){
	int i;
	int curSum =0, curAvg = -1;

	for (i=0; i <8 ; ++i){
		curSum+= samples[i][analogInput];
	}

	//divide by 8
	curAvg = curSum >> 3;

	return curAvg;
}



