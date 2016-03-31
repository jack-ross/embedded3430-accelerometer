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
	ADC10DTC1 = 0x20;                         // number of conversions

}

/*
 * Returns the ADC Value of the specified input
 * 0 - X
 * 1 - Y
 * 2 - Z
 */
volatile unsigned int sample[3][8];

unsigned int getADCConversion(int analogInput){

	int ADCReading = 12345;

	ADC10CTL0 &= ~ENC;
	while (ADC10CTL1 & BUSY);               // Wait if ADC10 core is active
    ADC10SA = (int) &(samples);        // Data buffer start
	ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion ready
    __bis_SR_register(CPUOFF + GIE);        // LPM0, ADC10_ISR will force exit

    _NOP();                                 // space for debugger
    _NOP();                                 // Set Breakpoint here to read ADC

	return ADCReading;
}


// ADC10 interrupt service routine
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR (void)
{
  __bic_SR_register_on_exit(CPUOFF);        // Clear CPUOFF bit from 0(SR)
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



