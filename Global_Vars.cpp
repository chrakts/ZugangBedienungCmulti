/*
 * Global_Vars.cpp
 *
 * Created: 21.02.2016 16:45:52
 *  Author: Christof
 */

#define EXTERNALS_H_

#include "Bedienung.h"

char const *Node = "ZB";
char const *klingelNode = "Kg";

char Compilation_Date[] = __DATE__;
char Compilation_Time[] = __TIME__;

volatile TIMER MyTimers[MYTIMER_NUM]= {	{TM_STOP,RESTART_NO,100,0,Beeper_Ready},
										{TM_STOP,RESTART_NO,100,0,NULL},			// Eingabe abgelaufen
										{TM_START,RESTART_NO,250,0,LED_toggle},
										{TM_STOP,RESTART_NO,500,0,Licht_Gross},
										{TM_START,RESTART_NO,12000,0,goto_sleep},
										{TM_STOP,RESTART_NO,250,0,Eingabe_bereit},	// Blockade nach falscher Eingabe
										{TM_START,RESTART_YES,0,0,Klingel_LED_PWM}	// Blockade nach falscher Eingabe

};

RandomTimer my_random_timer(&TCC1);

uint8_t key[16] = { AES256_KEY };

uint8_t Actual_Random[2][16]={{ RANDOM_0 },{RANDOM_1 }};

uint8_t Valid_Random[2] = {false,false};

uint8_t Klingel_LED_Dimmer = 5;

uint8_t iLichtKleinStatus=0;

volatile uint8_t do_sleep = 0;

volatile float fHelligkeit=0.0;
volatile uint16_t iLichtgrenzwert=200,iLichtwertHysterese=50;

uint8_t	Zustand_Eingabe=0;
uint8_t Zustand_AutoDoor = false;

volatile uint8_t blockade_status = 0;
uint16_t Blockade_Zeiten[BLOCKADE_NUM] = {250,1000,1000,3000,5000,15000,60000};

volatile uint8_t BlockingStatus = UNBLOCKED;
uint16_t BlockadeZeiten[BLOCKED_LAST] = {250,1000,1000,3000,5000,15000,60000};


volatile char actuelle_taste=0;
volatile uint8_t PIR_Trigger=0;
volatile uint8_t Klingel_Trigger=0;

char Code[7]="------";

const char SCode[SCODE_NUM][SCODE_LENGTH+1] = {SCODES};
uchar defaultKeyA[16] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }; // muss alles 0xff sein !!!
uchar NDEFKeyA[16] =    { 0xD3, 0xF7, 0xD3, 0xF7, 0xD3, 0xF7 };
uchar myinfo[16] = "ABABABABABABABA";
uchar newkey[16] = { NEWKEY };
uchar conditionBits_KeyB[10] = { 0xFF , 0x07 , 0x80 , 0x69 , 0x88 , 0x88 , 0x8F , 0x88 , 0x88 , 0x88 };
uchar diffkey[6] = {0xCE, 0xE0, 0xC0, 0xD2, 0x74, 0xBA};

Communication cmulti(KNET,Node,5,true);
