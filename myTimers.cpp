/*
 * MyTimer.cpp
 *
 * Created: 11.02.2016 20:19:03
 *  Author: Christof
 */

#include "myTimers.h"

extern volatile uint8_t do_sleep;

volatile TIMER MyTimers[MYTIMER_NUM]= {	{TM_STOP,RESTART_NO,100,0,Beeper_Ready},
  									{TM_START,RESTART_YES,1000,0,nextReport},
										{TM_STOP,RESTART_NO,100,0,NULL},			// Eingabe abgelaufen
										{TM_START,RESTART_NO,250,0,LED_toggle},
										{TM_STOP,RESTART_NO,500,0,Licht_Gross},
										{TM_STOP,RESTART_NO,250,0,resetCardStatus},	// Blockade nach falscher Eingabe
										{TM_START,RESTART_YES,1,0,Klingel_LED_PWM},	// Blockade nach falscher Eingabe
                    {TM_START,RESTART_YES,1,0,makeAdcMeasure}
};

void makeAdcMeasure(uint8_t test)
{
    if(sLEDStatus[SLEEPSTATUS]=='S')
    {
      if(sleepOldStatus != 'S')
      {
        initTemperatureMessung();
        sleepOldStatus = sLEDStatus[SLEEPSTATUS];
      }
      startAdcMeasure();
    }
		refresh_led_new();
}

void nextReport(uint8_t test)
{
  doNextReport=true;
}

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


void resetCardStatus(uint8_t test)
{
  MyTimers[TIMER_CARDSTATUS].state = TM_STOP;
	cardStatus = false;
  MFRC522_Init();
  _delay_ms(2);
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
