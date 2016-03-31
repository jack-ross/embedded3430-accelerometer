#include <msp430.h>
#include "globals.h"
#include "LEDDriver.h"
#include "LED.h"
#include "debounce.h"
#include "ADC.h"
#include "timerA.h"
// Function Prototypes
void ConfigureClockModule();


SwitchDefine CalibrateButton;
//unsigned int g100nsTimeout;		// This variable is incremented by the interrupt handler and
								// decremented by a software call in the main loop.

//changed to int to ease overflow calculations
unsigned int g1msTimer;

// Function Prototypes
void ConfigureClockModule();
void InitializeGlobalVariables();
void ManageSoftwareTimers();	// This function manages software timers.


void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;		// Stop watchdog timer
    ConfigureClockModule();
    InitializeGlobalVariables();

    ConfigureTimerA();
    ConfigureADC();

    // Initialization of hardware.
    SetLEDState(LED1,OFF);
    SetLEDState(LED2,OFF);
    InitializeLEDPortPins();

    startLEDDisplay();

    _BIS_SR(GIE);		// interrupts enabled

	// Loop forever
	while(1) {
		ManageSoftwareTimers();

		if (Debouncer(&CalibrateButton) == true) {
			// button was pushed
		}
	}
}

void ConfigureClockModule()
{
	// Configure Digitally Controlled Oscillator (DCO) using factory calibrations
	DCOCTL  = CALDCO_16MHZ;
	BCSCTL1 = CALBC1_16MHZ;
}

int counter;

void InitializeGlobalVariables(void)
{
	g1msTimeout = 0;
	g1msTimer = 0;
	InitializeSwitch(&CalibrateButton,(char *) &P1IN,(unsigned char) BIT3);

	counter =0;

	int i,j;
	for(j = 0; j<3; ++j)
		for(i = 0; i<8;++i)
			samples[j][i] = 0;
}

void accelerometerCheck() {
	if(counter==8)
		_nop();
	else
		++counter;

	int temp;
	//take sample
	temp = getADCConversion(1);

	//filter (also stores data)
	filter(1, temp);

	//enable conversion and start next conversion
	ADC10CTL0 |= ENC | ADC10SC;
}

//unsigned int samples[3][8];
unsigned int g2msTimer;
void ManageSoftwareTimers(void)
{
	int temp;

	if(g1msTimeout != 0){
		g1msTimeout--;
		g1msTimer++;
		ledPWM();		
	}
	
	TOGGLE_LED2;
	temp = getADCConversion(1);

	//0.5 s interrupt | wrap around at 500
	if(g1msTimer == 500){
		g1msTimer = 0;
		//TOGGLE_LED2;
	}

}

