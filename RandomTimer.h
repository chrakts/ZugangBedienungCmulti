/* 
* RandomTimer.h
*
* Created: 17.11.2015 21:02:44
* Author: Christof
*/


#ifndef __RANDOMTIMER_H__
#define __RANDOMTIMER_H__

#include "avr/io.h"

class RandomTimer
{
//variables
public:
protected:
private:
TC1_t *timer;
uint8_t randoms[16] ={0x34,0x12,0x01,0x99,0xab,0x32,0x3e,0x1c,0xa2,0x56,0xef,0x22,0x09,0xba,0x20,0x77};
uint8_t position;

//functions 
public:
	RandomTimer();
	~RandomTimer();
	RandomTimer(TC1_t *tm);
	uint16_t GetValue();
	void GetRandom(uint8_t *ran);
	void Clear();
	void Make_New();

protected:

private:
	
	RandomTimer( const RandomTimer &c );
	RandomTimer& operator=( const RandomTimer &c );

}; //RandomTimer

#endif //__RANDOMTIMER_H__
