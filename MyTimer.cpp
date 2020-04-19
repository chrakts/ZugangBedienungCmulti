/*
 * MyTimer.cpp 
 *
 * Created: 11.02.2016 20:19:03
 *  Author: Christof
 */ 

#include "Bedienung.h"

extern volatile uint8_t do_sleep;

void init_mytimer(void)
{
	CLK.RTCCTRL = CLK_RTCSRC_RCOSC_gc | CLK_RTCEN_bm;
	
	do {
		;/* Wait until RTC is not busy. */
	} while ( RTC.STATUS & RTC_SYNCBUSY_bm );
	
	RTC.PER = 1;
	RTC.CNT = 0;
	RTC.COMP = 0; 
	RTC.CTRL = 1; // Teiler 1
	RTC.INTCTRL	= RTC_OVFINTLVL_MED_gc;
}

void no_function( void )
{
	;
}

ISR ( RTC_OVF_vect )
{
uint8_t i;
//	LED_KLINGEL_TOGGLE;
	for (i=0;i<MYTIMER_NUM;i++)
	{
		switch(MyTimers[i].state )
		{
			case TM_STOP:
			break;
			case TM_RUN:
				if (MyTimers[i].actual==0)
				{			
					if (MyTimers[i].restart == RESTART_YES)
					{
						MyTimers[i].actual = MyTimers[i].value;
						MyTimers[i].state = TM_RUN;
					}
					else
						MyTimers[i].state = TM_STOP;
					if (MyTimers[i].OnReady!=NULL)
						MyTimers[i].OnReady(3);
				}
				else
					MyTimers[i].actual--;
			break;			
			case TM_START:
				MyTimers[i].actual = MyTimers[i].value;
				MyTimers[i].state = TM_RUN;
			break;

			case TM_RESET:
				MyTimers[i].state = TM_START;
			break;
		}
	}
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