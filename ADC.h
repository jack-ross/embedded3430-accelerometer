#ifndef ADC_H
#define ADC_H
#include <msp430.h>

// Use the following to select either analog channel 7 or the temperature sensor
#define ADC10_A7 1
#define ADC10_TEMP_SENSOR 0
#define ADC10_MODE ADC10_A7
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *                 ADC10 A7
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   GPIO      :  P1.7
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
#define ADC10_A7_BIT	BIT7	// Port pin bit location for ADC10 A7


// Prototypes
void ConfigureADC(void);
int filter(int analogInput);
void getADCValues();

#endif
