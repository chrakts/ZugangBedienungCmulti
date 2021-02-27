/*
 * External.h
 *
 * Created: 21.02.2016 16:49:31
 *  Author: Christof
 */


#ifndef EXTERNAL_H_
#define EXTERNAL_H_

#include "myConstants.h"
#include "Communication.h"
#include "Bedienung.h"
#include "ComReceiver.h"


extern const char *klingelNode;
extern const char *Node;

extern char Compilation_Date[];
extern char Compilation_Time[];

extern char sLEDStatus[13];
extern bool cardStatus;


extern RandomTimer my_random_timer;

extern uint8_t key[16];

//extern Encrypt encrypt;

extern uint8_t Actual_Random[2][16];
extern uint8_t Valid_Random[2];

extern volatile char actuelle_taste;

extern uint8_t Klingel_LED_Dimmer;

extern uint8_t iLichtKleinStatus;

extern volatile uint16_t iLichtgrenzwert,iLichtwertHysterese;
extern volatile float fHelligkeit;

extern volatile uint8_t do_sleep;

extern volatile uint8_t BlockingStatus;
extern uint16_t BlockadeZeiten[];

extern uint8_t	Zustand_Eingabe;
extern uint8_t  Zustand_AutoDoor;
extern volatile uint8_t blockade_status;
extern uint16_t Blockade_Zeiten[BLOCKADE_NUM];

extern volatile char actuelle_taste;
extern volatile uint8_t PIR_Trigger;
extern volatile uint8_t Klingel_Trigger;

extern char Code[7];
extern const char SCode[SCODE_NUM][SCODE_LENGTH+1];

extern uchar defaultKeyA[16]; // muss alles 0xff sein !!!

extern uchar NDEFKeyA[16];
extern uchar myinfo[16];
extern uchar newkey[16];
extern uchar conditionBits_KeyB[10];
extern uchar diffkey[6];

extern Communication cmulti;
extern ComReceiver cnetCom;
#endif /* EXTERNAL_H_ */
