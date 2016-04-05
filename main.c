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
bool isCalibrated;

//changed to int to ease overflow calculations
unsigned int g1msTimer;
unsigned int samplesArray[8][3];
int CoordinateX, CoordinateXMax, CoordinateXMin, CoordinateX0;
int CoordinateY, CoordinateYMax, CoordinateYMin, CoordinateY0;
int CoordinateZ, CoordinateZMax, CoordinateZMin, CoordinateZ0;

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
    InitializePushButtonSwitchPins();

    startLEDDisplay();

    _BIS_SR(GIE);		// interrupts enabled

	// Loop forever
	while(1) {
		ManageSoftwareTimers();

		if (isCalibrated == false)
		{
			Calibrate();
		}
	}
}

int calibrationStep = 0;
void Calibrate() {

	if (Debouncer(&CalibrateButton) == true) {
		calibrationStep++;
	}

	turnOffLEDs();

	switch(calibrationStep){
		//+X
		case 0:
			//Turn on North LED
			turnOnLED(0);
			CoordinateXMax = filter(2);
			break;

		//-X
		case 1:
			//Turn on South LED
			turnOnLED(4);
			CoordinateXMin = filter(2);
			break;

		//+Y
		case 2:
			//Turn on West LED
			turnOnLED(6);
			CoordinateYMax = filter(1);
			break;

		//-Y
		case 3:
			//Turn on East LED
			turnOnLED(2);
			CoordinateYMin = filter(1);
			break;

		//+Z
		case 4:
			//Turn on N,S,E,W LEDS
			setIntensity(0,20);
			setIntensity(2,20);
			setIntensity(4,20);
			setIntensity(6,20);
			CoordinateZMax = filter(0);
			break;

		//-Z
		case 5:
			//Turn on NE,SE,NW,SW LEDS
			setIntensity(1,20);
			setIntensity(3,20);
			setIntensity(5,20);
			setIntensity(7,20);
			CoordinateZMin = filter(0);
			break;
	}

	//We have calibrated all 3 directions, set the offsets
	if(calibrationStep == 6){
		isCalibrated = true;
		/*
		//These are offsets aka "zero" that will normalize the data for the cordic
		//Do you add or subtract XMin
		CoordinateX0 = CoordinateXMax + CoordinateXMin;

		*/
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

	//curSampleIndex =0;

	int i,j;
	for(i = 0; i<8;++i){
		for(j = 0; j<3; ++j)
			samplesArray[i][j] = 0;	
	}
}

unsigned int g2msTimer;
void ManageSoftwareTimers(void)
{
	int temp;

	if(g1msTimeout != 0){
		g1msTimeout--;
		g1msTimer++;
		ledPWM();	
		getADCValues();
	}
	
	TOGGLE_LED2;

	//0.5 s interrupt | wrap around at 500
	if(g1msTimer == 500){
		g1msTimer = 0;
	}

}



