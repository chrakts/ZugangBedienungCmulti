/*
 * PIR.cpp
 *
 * Created: 09.02.2016 19:53:11
 *  Author: Christof
 */ 

#include "Bedienung.h"

extern volatile TIMER MyTimers[MYTIMER_NUM];

void init_PIR()
{
	PORTC_INTCTRL = PORT_INT0LVL_LO_gc; // Low-Level interrupt 0 for PORTA
	PORTC_INT0MASK = PIR_PIN;
	PORTC_PIN2CTRL = PORT_ISC_RISING_gc;
}

SIGNAL(PORTC_INT0_vect)
{
	MyTimers[TIMER_SLEEP].state = TM_START;
	PIR_Trigger = 1;
}