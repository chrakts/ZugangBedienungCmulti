/*
 * MyTimer.cpp
 *
 * Created: 11.02.2016 20:19:03
 *  Author: Christof
 */

#include "myTimers.h"

extern volatile uint8_t do_sleep;

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

void goto_sleep(uint8_t test)
{
	if( (PORTC_IN & PIR_PIN) > 0) // falls PIR aktiv soll kein Powerdown eingeleitet werden
	{
		MyTimers[TIMER_SLEEP].state = TM_RESET;
	}
	else
	{
		do_sleep = 1;
	}

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
