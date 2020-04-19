/*
 * MyTimer.h
 *
 * Created: 11.02.2016 20:20:03
 *  Author: Christof
 */ 


#ifndef MYTIMER_H_
#define MYTIMER_H_

#define MYTIMER_NUM	7

enum{TM_START,TM_RESET,TM_STOP,TM_RUN};
enum{RESTART_YES,RESTART_NO};

enum{TIMER_BEEPER_OFF,TIMER_TIMEOUT,TIMER_RECALL,TIMER_LICHT_GROSS,TIMER_SLEEP,TIMER_BLOCKADE,TIMER_LED_KLINGEL};

struct Timer
{
	uint8_t		state;
	uint8_t		restart;
	uint16_t	value;
	uint16_t	actual;
	void  (*OnReady)  (uint8_t test);
};

typedef struct Timer TIMER;

void Beeper_Ready(uint8_t test);
void LED_toggle(uint8_t test);
void init_mytimer(void);
void Licht_Gross(uint8_t test);
void goto_sleep(uint8_t test);
void Eingabe_bereit(uint8_t test);
void Klingel_LED_PWM(uint8_t test);

#endif /* MYTIMER_H_ */