/*
 * LED.cpp
 *
 * Created: 13.02.2016 12:25:00
 *  Author: Christof
 */

#include "Bedienung.h"

static const uint8_t PROGMEM GammaTable[] = GCN_TABLE7TO8;

FARBE leds[NUMLEDS] = {F_ROT,F_LILA};

//uint16_t AutoGain[20]= {2,4,6,10,15,20,40,80,160,320,640,1280,2560,5120,10240,20480,40960,40960,40960,40960};
uint16_t AutoGain[20]= {0,0,0,10,15,20,40,80,160,320,640,1280,2560,5120,10240,20480,2560,30960,35960,40960};
uint16_t KlingelGain[6]= {0,0,200,800,10080,40960};
//                 Rot  GRÜN BLAU
FARBE iFarbe[16]={{0x00,0x00,0x00},// F_BLACK
                  {0x00,0x00,0x7f},// F_BLAU
                  {0x7f,0x7f,0x7f},// F_WEISS
                  {0x7f,0x00,0x00},// F_ROT
                  {0x00,0x7f,0x00},// F_GRUEN
                  {0x7f,0x00,0x7f},// F_LILA
                  {0x7f,0x5f,0x25},// F_ORANGE
                  {0x7f,0x35,0x35},// F_MAGENTA
                  {0x00,0x7f,0x7f},// F_TUERKIS
                  {0x7f,0x7f,0x00}, // F_MINT
                  {0x7f,0x70,0x3f}, // F_GELB
                 };

void refresh_led_new()
{ // sLEDStatus

	uint8_t i;
	static uint8_t led_loc[3*NUMLEDS];
	uint16_t color;
  uint8_t brightness = ((uint8_t)sLEDStatus[10]-65);
  FARBE f;
	for (i=0;i<NUMLEDS;i++)
	{
    f = iFarbe[(uint8_t)sLEDStatus[i]-65];
		color = (f.gruen * brightness)>>4;
		led_loc[3*i] =	(uint8_t) color;
		color = (f.rot * brightness)>>4;
		led_loc[3*i+1] = (uint8_t) color;
		color = (f.blau * brightness)>>4;
		led_loc[3*i+2] = (uint8_t) color;
	}
	while (WS_out(led_loc,NUMLEDS*3,GammaTable)!=0)
	{
	}
}

void set_led_autobright(uint16_t lumi)
{
int i=19;
	while( (lumi<AutoGain[i]) & (i!=0) )
		i--;
	fill_led_brightness(i+1);
	i=5;
	while( (lumi<KlingelGain[i]) & (i!=0) )
		i--;
	Klingel_LED_Dimmer = i;
}

void set_led_color(uint8_t r,uint8_t g,uint8_t b,uint8_t bright,uint8_t index)
{
	if (index<NUMLEDS)
	{
		leds[index].rot = r;
		leds[index].gruen = g;
		leds[index].blau = b;

		if (bright!=BR_NOCHANGE)
		{
			leds[index].brightness = bright;
		}
	}
}

void fill_led_color(uint8_t r,uint8_t g,uint8_t b,uint8_t bright)
{
	uint8_t i;

	for (i=0;i<NUMLEDS;i++)
	{
		leds[i].rot = r;
		leds[i].gruen = g;
		leds[i].blau = b;

		if ( (bright!=BR_NOCHANGE) & (bright <= BR_MAX) )
		{
			leds[i].brightness = bright;
		}
	}
}

void fill_bar_color(uint8_t r,uint8_t g,uint8_t b,uint8_t bright)
{
	uint8_t i;

	for (i=0;i<NUMBAR;i++)
	{
		leds[i].rot = r;
		leds[i].gruen = g;
		leds[i].blau = b;

		if ( (bright!=BR_NOCHANGE) & (bright <= BR_MAX) )
		{
			leds[i].brightness = bright;
		}
	}
}

void fill_led_brightness(uint8_t bright)
{
	uint8_t i;

	for (i=0;i<NUMLEDS;i++)
	{
		if ( (bright!=BR_NOCHANGE) & (bright <= BR_MAX) )
		{
			leds[i].brightness = bright;
		}
	}
}

void refresh_led()
{
	uint8_t i;
	static uint8_t led_loc[3*NUMLEDS];
	uint16_t color;

	for (i=0;i<NUMLEDS;i++)
	{
		color = (leds[i].gruen * leds[i].brightness)/BR_MAX;
		led_loc[3*i] =	(uint8_t) color;
		color = (leds[i].rot * leds[i].brightness)/BR_MAX;
		led_loc[3*i+1] = (uint8_t) color;
		color = (leds[i].blau * leds[i].brightness)/BR_MAX;
		led_loc[3*i+2] = (uint8_t) color;
	}
	while (WS_out(led_loc,NUMLEDS*3,GammaTable)!=0)
	{
		TEST_TOGGLE;
		_delay_us(300);
		TEST_TOGGLE;
		_delay_us(300);
	}
}

