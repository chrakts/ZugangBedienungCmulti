/* 
* RandomTimer.cpp
*
* Created: 17.11.2015 21:02:44
* Author: Christof
*/


#include "Bedienung.h"

// default constructor
RandomTimer::RandomTimer()
{
} //RandomTimer

RandomTimer::RandomTimer(TC1_t *tm)
{
	timer = tm;
	timer->CTRLA = TC_CLKSEL_DIV1_gc;
	timer->CTRLB = TC_WGMODE_NORMAL_gc;
	timer->CTRLC = 0;
	timer->CTRLD = 0;
	timer->CTRLE = 0;
	position = 0;
} 

uint16_t RandomTimer::GetValue()
{
	return(timer->CNT);
}

void RandomTimer::GetRandom(uint8_t *ran)
{
uint8_t i;
	for (i=0;i<16;i++)
		ran[i] = this->randoms[i];
}

void RandomTimer::Clear()
{
	timer->CNT = 0;
}

void RandomTimer::Make_New()
{
	randoms[position]= timer->CNTL;
	position ++;
	randoms[position]= timer->CNTH;
	position ++;
	if (position>14)
		position=0;
}
//RandomTimer
// default destructor
RandomTimer::~RandomTimer()
{
} //~RandomTimer
