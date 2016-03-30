#include "timerA.h"
#include "LED.h"
#include "globals.h"

unsigned int g1msTimeout;

void ConfigureTimerA(void)
{
	//Tell the counter to count up, divide by 8, and clear it
	TACTL |= TASSEL_2 | ID_3 | MC_1 | TACLR;
	//MCLK @ 16 MHz - > divide by 8 -> 2 MHz clk
	//if count up to 500 would be 1 ms
	//count up to 500 = 1 ms
	TACCR0 |= 500;
	//Enable interupts
	TACCTL0 |= CCIE;
}



#pragma vector = TIMER0_A0_VECTOR
// Timer a interrupt service routine, add code here for IN LAB
	__interrupt void TimerA0_routine(void)
{
	g1msTimeout++;
}
