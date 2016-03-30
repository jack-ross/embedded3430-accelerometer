#ifndef FSM_DEBOUNCE_H_
#define FSM_DEBOUNCE_H_

#include <msp430.h>

// Definitions for debounce Times for the P1.3 button (ms)
#define HIGH_THRESHOLD 10
#define LOW_THRESHOLD 20

// Type Definitions
typedef enum {
	DbExpectHigh, DbValidateHigh, DbExpectLow, DbValidateLow
} DbState;

typedef enum {
	Low, High
} SwitchStatus;

// This structure should encapsulate all of the information needed for
// a switch.  As such, You will NEED TO insert more definitions in this struct.
// For example, you should include values for your High/Low threshold delay times.
typedef struct {
	DbState CurrentState;			// Current state of the FSM
	char * SwitchPort;				// Input port associated with switch
	unsigned char SwitchPortBit;	// Port pin associated with switch
} SwitchDefine;

// Function Prototypes
void InitializeSwitch(SwitchDefine *Switch,char *SwitchPort,unsigned char SwitchBit);

//This function returns the instantaneous value of the selected switch
SwitchStatus GetSwitch(SwitchDefine *Switch);

//This function debounces a switch input
SwitchStatus Debouncer(SwitchDefine *Switch);

#define ENABLE_PUSH_INTERRUPT	P1IE |= BIT3;
#define SET_RISING_EDGE			P1IES |= BIT3;
#define RESET_PUSH_FLAG			P1IFG &= ~BIT3;
#define ENABLE_PULLUP			P1REN |= BIT3;

void InitializeSwitchPins(void);

#endif
