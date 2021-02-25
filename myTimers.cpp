/*
 * MyTimer.cpp
 *
 * Created: 11.02.2016 20:19:03
 *  Author: Christof
 */

#include "myTimers.h"

extern volatile uint8_t do_sleep;

volatile TIMER MyTimers[MYTIMER_NUM]= {	{TM_STOP,RESTART_NO,100,0,Beeper_Ready},
										{TM_STOP,RESTART_NO,100,0,NULL},			// Eingabe abgelaufen
										{TM_START,RESTART_NO,250,0,LED_toggle},
										{TM_STOP,RESTART_NO,500,0,Licht_Gross},
										{TM_STOP,RESTART_NO,250,0,Eingabe_bereit},	// Blockade nach falscher Eingabe
										{TM_START,RESTART_YES,0,0,Klingel_LED_PWM}	// Blockade nach falscher Eingabe

};

void no_function( void )
{
	;
}

void Beeper_Ready(uint8_t test)
{
	BEEPER_OFF;
}

void LED_toggle(uint8_t test)
{
//	LED_KLINGEL_TOGGLE;
}

void Licht_Gross(uint8_t test)
{
	STROM_STOSS_OFF;
	set_led_color(F_GRUEN,BR_NOCHANGE,LED_LICHT_GROSS);
}


void Eingabe_bereit(uint8_t test)
{
	Zustand_Eingabe = INPUT_WAIT;
}

void Klingel_LED_PWM(uint8_t test)
{
//	LED_KLINGEL_TOGGLE;
	static uint8_t  count ;
	count++;
	if (count<=Klingel_LED_Dimmer)
		LED_KLINGEL_ON;
	else
		LED_KLINGEL_OFF;
	if(count==5)
		count = 0;
}
