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

volatile bool doNextReport=true;
volatile uint8_t toReport=REPORT_FIRST;

volatile char sleepOldStatus='S';

#define SLEEPSTATUS 12
char sLEDStatus[14]={C_BLAU,C_WEISS,C_ROT,C_GRUEN,C_LILA,C_ORANGE,C_MAGENTA,C_MINT,C_TUERKIS,C_GELB,'I','Q','R',0};
bool cardStatus=false;
RandomTimer my_random_timer(&TCC1);

uint8_t key[16] = { AES256_KEY };

uint8_t Actual_Random[2][16]={{ RANDOM_0 },{RANDOM_1 }};

uint8_t Valid_Random[2] = {false,false};

uint8_t Klingel_LED_Dimmer = 5;

uint8_t	Zustand_Eingabe=0;


volatile char actuelle_taste=0;
volatile uint8_t PIR_Trigger=0;
volatile uint8_t Klingel_Trigger=0;

char Code[7]="------";

const char SCode[SPECIAL_NUM][SCODE_LENGTH+1] = {SCODES};
uchar defaultKeyA[16] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }; // muss alles 0xff sein !!!
uchar NDEFKeyA[16] =    { 0xD3, 0xF7, 0xD3, 0xF7, 0xD3, 0xF7 };
uchar myinfo[16] = "ABABABABABABABA";
uchar newkey[16] = { NEWKEY };
uchar conditionBits_KeyB[10] = { 0xFF , 0x07 , 0x80 , 0x69 , 0x88 , 0x88 , 0x8F , 0x88 , 0x88 , 0x88 };
uchar diffkey[6] = {0xCE, 0xE0, 0xC0, 0xD2, 0x74, 0xBA};

Communication cmulti(KNET,Node,5,true);
//ComReceiver cmultiRec(&cmulti,Node,cnetCommands,NUM_CNET_COMMANDS,cnetInformation,NUM_INFORMATION,NULL,NULL);
