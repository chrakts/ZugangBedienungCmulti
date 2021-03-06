/*
 * MyTimer.h
 *
 * Created: 11.02.2016 20:20:03
 *  Author: Christof
 */

#ifndef MYTIMER_H_
#define MYTIMER_H_

#include "timer.h"
#include "External.h"
#include "myConstants.h"

#define MYTIMER_NUM	8


enum{TIMER_BEEPER_OFF,TIMER_NEXT_REPORT,TIMER_TIMEOUT,TIMER_RECALL,TIMER_LICHT_GROSS,TIMER_CARDSTATUS,TIMER_LED_KLINGEL,TIMER_ADC};

void nextReport(uint8_t test);
void makeAdcMeasure(uint8_t test);
void Beeper_Ready(uint8_t test);
void LED_toggle(uint8_t test);
void Licht_Gross(uint8_t test);
void goto_sleep(uint8_t test);
void resetCardStatus(uint8_t test);
void Klingel_LED_PWM(uint8_t test);

extern volatile TIMER MyTimers[MYTIMER_NUM];

#endif /* MYTIMER_H_ */
