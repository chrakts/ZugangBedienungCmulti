/*
 * tastatur.cpp
 *
 * Created: 09.02.2016 19:42:36
 *  Author: Christof
 */ 

#include "Bedienung.h"

static const char tasten[16] = {	'D','B','C','A',
									'0','5','8','2',
									'#','6','9','3',
									'*','4','7','1',};

void init_tastatur()
{
	PORTA_INTCTRL = PORT_INT0LVL_LO_gc; // Low-Level interrupt 0 for PORTA
	PORTA_INT0MASK = TASTATUR_DA_PIN;
	PORTA_PIN2CTRL = PORT_ISC_RISING_gc;
}

SIGNAL(PORTA_INT0_vect)
{

//	LED_KLINGEL_TOGGLE;
	actuelle_taste = tasten[PORTA_IN >> 4];
//	output_print(KNET,"taste:%c\n\r",actuelle_taste);
}