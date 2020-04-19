/*
 * Klingel.cpp
 *
 * Created: 09.02.2016 20:24:18
 *  Author: Christof
 */ 

#include "Bedienung.h"

void init_klingel()
{
	PORTB_INTCTRL = PORT_INT0LVL0_bm ; // Low-Level interrupt 0 for PORTA
	PORTB_INT0MASK = KLINGEL_TASTER_PIN;
	PORTB_PIN2CTRL = PORT_ISC_FALLING_gc | PORT_OPC_PULLUP_gc | PORT_SRLEN_bm;
}

SIGNAL(PORTB_INT0_vect)
{
	
	_delay_ms(100);
	if( (PORTB_IN & KLINGEL_TASTER_PIN)==0 )
	{
		LED_KLINGEL_TOGGLE;
		PORTC_OUTSET = BEL_PIN;
		Klingel_Trigger = 1;
	}
}