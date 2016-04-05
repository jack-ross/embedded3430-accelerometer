#ifndef ADC_H
#define ADC_H
#include <msp430.h>
#include "globals.h"

// Prototypes
void ConfigureADC(void);
int filter(Axis axis);
void getADCValues();

#endif
