#include <msp430.h>
#include "globals.h"
#include "LEDDriver.h"
#include "LED.h"
#include "debounce.h"
#include "ADC.h"
#include "timerA.h"
#include "cordic.h"


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
		} else {
			updateDisplayWithTilt();
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
			CoordinateXMax = filter(Xaxis);
			break;

		//-X
		case 1:
			//Turn on South LED
			turnOnLED(4);
			CoordinateXMin = filter(Xaxis);
			break;

		//+Y
		case 2:
			//Turn on West LED
			turnOnLED(6);
			CoordinateYMax = filter(Yaxis);
			break;

		//-Y
		case 3:
			//Turn on East LED
			turnOnLED(2);
			CoordinateYMin = filter(Yaxis);
			break;

		//+Z
		case 4:
			//Turn on N,S,E,W LEDS
			setIntensity(0,20);
			setIntensity(2,20);
			setIntensity(4,20);
			setIntensity(6,20);
			CoordinateZMax = filter(Zaxis);
			break;

		//-Z
		case 5:
			//Turn on NE,SE,NW,SW LEDS
			setIntensity(1,20);
			setIntensity(3,20);
			setIntensity(5,20);
			setIntensity(7,20);
			CoordinateZMin = filter(Zaxis);
			break;
	}

	//We have calibrated all 3 directions, set the offsets
	if(calibrationStep == 6){
		isCalibrated = true;
		
		// These are offsets aka "zero" that will normalize the data for the cordic
		// Average is level
		CoordinateX0 = (CoordinateXMax + CoordinateXMin) >> 1;	
		CoordinateY0 = (CoordinateYMax + CoordinateYMin) >> 1;
		CoordinateZ0 = (CoordinateZMax + CoordinateZMin) >> 1;
	}
}

/*
 *	Interface between the accellerometer and the LEDs
 *  LED closest to ground is brightest
 *	1st and 2nd neighbors glow less
 *	3rd and outward are off
 *
 */
int xAxisAvg, yAxisAvg, zAxisAvg, mainLED, mainLEDIntensity,
	left1LED, left2LED, right1LED, right2LED;
void updateDisplayWithTilt(){
	//Get current  moving avages of the tilt
	xAxisAvg = filter(Xaxis);
	yAxisAvg = filter(Yaxis);
	zAxisAvg = filter(Zaxis);

	// normalize coordinates
	xAxisAvg -= CoordinateX0;
	yAxisAvg -= CoordinateY0;
	zAxisAvg -= CoordinateZ0;

	// create coordiantes struct
	coordinates *currentCoords;
	currentCoords->x = xAxisAvg;
	currentCoords->y = yAxisAvg;
	currentCoords->z = zAxisAvg;

	measurements currentMeasurement;

	currentMeasurement = getDisplaySetting(&currentCoords);

	mainLED = getMainLED(currentMeasurement);
	mainLEDIntensity = getLEDIntensity(currentMeasurement);

	turnOffLEDs();
	
	if (mainLEDIntensity == -1)
	{
		return;
	}

	left1LED = (mainLED - 1) & 0x7;
	left2LED = (mainLED - 2) & 0x7;
	right1LED = (mainLED + 1) & 0x7;
	right2LED = (mainLED + 2) & 0x7;

	int firstIntensity, secondIntensity;
	firstIntensity = 2;
	secondIntensity = 1;

	if (mainLEDIntensity > 15)
	{
		firstIntensity = 10;
		secondIntensity = 5;
	} else if (mainLEDIntensity > 10) {
		firstIntensity = 7;
		secondIntensity = 4;
	} else if (mainLEDIntensity >= 5) {
		firstIntensity = 4;
		secondIntensity = 2;
	} 

	setIntensity(mainLED, mainLEDIntensity);
	setIntensity(left1LED, firstIntensity);
	setIntensity(right1LED, firstIntensity);
	setIntensity(left2LED, secondIntensity);
	setIntensity(right2LED, secondIntensity);

}


/**
 *	Get Main LED
 * 	
 */
int xyAngle, ledNumber, ledIntensity;
int getMainLED(measurements *measurements) {

	xyAngle = measurements->angle;

	ledNumber = 0;
	int i;
	for (i = 0; i < 8; ++i)
	{
		if (xyAngle < 2250)
		{
			return ledNumber;
		}
		ledNumber++;
		xyAngle -= 4500;
	}
}

//Intensity of the main LED
int getLEDIntensity(measurements *measurements) {
	int tilt;
	tilt = measurements->tilt;

	// if board is nearly level (9 degrees), all LEDs are off
	if (tilt <= 900)
	{
		return -1;
	}

	// there are 20 levels of intensity so each degree
	// is worth 0.22 of a (20ms) period of intensity or
	// every 4.5 degrees increases intensity by one
	ledIntensity = 3;
	int i;
	for (i = 3; i <= 20; ++i)
	{
		if (tilt < 450)
		{
			return ledIntensity;
		}
		ledIntensity++;
		tilt -= 450;
	}

	return -1;
}

void ConfigureClockModule()
{
	// Configure Digitally Controlled Oscillator (DCO) using factory calibrations
	DCOCTL  = CALDCO_16MHZ;
	BCSCTL1 = CALBC1_16MHZ;
}

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

void ManageSoftwareTimers(void)
{
	int temp;

	if(g1msTimeout != 0){
		g1msTimeout--;
		g1msTimer++;
		ledPWM();	
		getADCValues();

	}
	
	//TOGGLE_LED2;

	//0.5 s interrupt | wrap around at 500
	if(g1msTimer == 500){
		g1msTimer = 0;
	}

}





