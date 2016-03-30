#include "debounce.h"
#include "led.h"

void InitializeSwitch(SwitchDefine *Switch,char *SwitchPort,unsigned char SwitchBit)
{
	Switch->CurrentState = DbExpectHigh;
	Switch->SwitchPort = SwitchPort;
	Switch->SwitchPortBit = SwitchBit;
}

void InitializeSwitchPins(){
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

extern int g1msTimer;
int startTime,stopTime, elapsedTime;
/*LED codes
 * 	LED 1 = BIT0 | LED 2 = BIT1
 * 00 Exepect High
 * 01 Validate High
 * 10 Expect Low
 * 11 Validate Low
 *
 */
SwitchStatus Debouncer(SwitchDefine *Switch)
{
	SwitchStatus CurrentSwitchReading = GetSwitch(Switch);
	DbState NextState = DbExpectHigh;
	
	// First, determine the current inputs.

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
			//TURN_OFF_LED1;
			//TURN_OFF_LED2;
		break;

		case DbValidateHigh:
//			TURN_OFF_LED1;
//			TURN_ON_LED2;
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
			else
				NextState = DbExpectLow;
		break;

		case DbExpectLow:
			if(CurrentSwitchReading == Low){
				NextState = DbValidateLow;
			}
			else
				NextState = DbExpectLow;
//			TURN_ON_LED1;
//			TURN_OFF_LED2;
			//record time so that it can be used in validation
			startTime = g1msTimer;
		break;

		case DbValidateLow:
//			TURN_ON_LED1;
//			TURN_ON_LED2;
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
			else
				NextState = DbExpectHigh;
		break;
		default: NextState = DbExpectHigh;
	}

	// Perform the output function(s) based on the inputs and current state.
	if(Switch->CurrentState ==  DbValidateHigh && (NextState == DbExpectLow) )
		TOGGLE_LED1;
	else if(Switch->CurrentState ==  DbValidateLow && (NextState == DbExpectHigh) )
		TOGGLE_LED2;

	// Finally, update the current state with the next state.
	Switch->CurrentState = NextState;
	
	return CurrentSwitchReading;
}

