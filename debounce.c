#include "debounce.h"
#include "led.h"
#include "globals.h"

void InitializeSwitch(SwitchDefine *Switch,char *SwitchPort,unsigned char SwitchBit)
{
	Switch->CurrentState = DbExpectHigh;
	Switch->SwitchPort = SwitchPort;
	Switch->SwitchPortBit = SwitchBit;
}

void InitializePushButtonSwitchPins(){
	//enable push button (p1.3) as output
	P1DIR &= ~BIT3;
	P1REN |= BIT3;
	P1OUT |= BIT3;
}

SwitchStatus GetSwitch(SwitchDefine *Switch)
{
	//switch is active low, so high means button is pressed
	char *port = Switch->SwitchPort;
	char portBit = Switch->SwitchPortBit;
	if(*port & portBit )
		return Low;
	return High;
}

int startTime,stopTime, elapsedTime;

bool Debouncer(SwitchDefine *Switch)
{
	// First, determine the current inputs.
	SwitchStatus CurrentSwitchReading = GetSwitch(Switch);
	DbState NextState = DbExpectHigh;	

	bool switchTriggered = false;

	// Next, based on the input values and the current state, determine the next state.
	switch (Switch->CurrentState) {

		case DbExpectHigh:
			if(CurrentSwitchReading == High){
				NextState = DbValidateHigh;
				//record time so that it can be used in validation
				startTime = g1msTimer;
			}
			else
				NextState = DbExpectHigh;
		break;

		case DbValidateHigh:
			if(CurrentSwitchReading == Low){
				NextState = DbExpectHigh;
			}

			//we now know that the switch state is high
			//clock overflowed (g1msTimer resets at 500 ms)
			stopTime = g1msTimer;

			if(stopTime-startTime <0)
				//will be adding a negative number
				elapsedTime = 500 + (stopTime-startTime);
			else
				elapsedTime = stopTime-startTime;

			//we want to delay until the threshold
			if(elapsedTime < HIGH_THRESHOLD)
				NextState = DbValidateHigh;
			else{
				// state is definitely high
				switchTriggered = true;
				NextState = DbExpectLow;
			}
		break;

		case DbExpectLow:
			if(CurrentSwitchReading == Low){
				NextState = DbValidateLow;
			}
			else
				NextState = DbExpectLow;
			//record time so that it can be used in validation
			startTime = g1msTimer;
		break;

		case DbValidateLow:
			if(CurrentSwitchReading == High){
				NextState = DbExpectLow;
			}
			//we now know that the switch state is high
			//clock overflowed (g1msTimer resets at 500 ms)
			stopTime = g1msTimer;
			if(stopTime-startTime <0)
				elapsedTime = 500 + (stopTime-startTime);
			else
				elapsedTime = stopTime-startTime;

			//we want to delay until the threshold
			if(elapsedTime < LOW_THRESHOLD)
				NextState = DbValidateLow;
			else {
				// state is definitely Low
				switchTriggered = false;
				NextState = DbExpectHigh;
			}
		break;
		default: NextState = DbExpectHigh;
	}

	// Perform the output function(s) based on the inputs and current state.
	// if(Switch->CurrentState ==  DbValidateHigh && (NextState == DbExpectLow) )
	// 	TOGGLE_LED1;
	// else if(Switch->CurrentState ==  DbValidateLow && (NextState == DbExpectHigh) )
	// 	TOGGLE_LED2;

	// Finally, update the current state with the next state.
	Switch->CurrentState = NextState;
	
	return switchTriggered;
}

