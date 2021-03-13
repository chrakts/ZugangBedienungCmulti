#include "Licht.h"

uint8_t lichtKleinStatus=0;


void nextStatusLichtKlein()
{
  cmulti.sendCommand(klingelNode,'L','1','n');
}

void aktiviereStromStoss()
{
  STROM_STOSS_ON;
  MyTimers[TIMER_LICHT_GROSS].state = TM_START;
  LED_ROT_ON;
}
