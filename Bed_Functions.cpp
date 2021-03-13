/*
 * Bed_Functions.cpp
 *
 * Created: 21.02.2016 16:43:48
 *  Author: Christof
 */

#include "Bedienung.h"

#define NUM_COMMANDS 7 + CMULTI_STANDARD_NUM

COMMAND cnetCommands[NUM_COMMANDS] =
{
  cmultiStandardCommands,
  {'R','s',CUSTOMER,NOPARAMETER,0,jobReceiveRandom},
  {'L','t',CUSTOMER,NOPARAMETER,0,jobToggleLight},
  {'C','k',CUSTOMER,BYTEARRAY,KEY_LENGTH,jobgotCardKey},
  {'C','r',CUSTOMER,NOPARAMETER,0,jobReleaseCard},
  {'C','w',CUSTOMER,BYTEARRAY,KEY_LENGTH+INFO_LENGTH,jobWriteCard},
  {'C','d',CUSTOMER,BYTEARRAY,KEY_LENGTH,jobClearCard},
  {'C','p',CUSTOMER,NOPARAMETER,0,jobWaitForClear},
};

#define NUM_INFORMATION 2

INFORMATION cnetInformation[NUM_INFORMATION]=
{
  {klingelNode,'L','1','a',UINT_8,1,&lichtKleinStatus,NULL},
  {klingelNode,'S','1','s',STRING,15,(void*)&sLEDStatus,gotNewLedStatus}
};

ComReceiver cnetCom(&cmulti,Node, cnetCommands,NUM_COMMANDS, cnetInformation,NUM_INFORMATION,NULL,NULL);

void gotNewLedStatus()
{
  refresh_led_new();
  Klingel_LED_Dimmer = (uint8_t)sLEDStatus[11]-65;
}

void jobToggleLight(ComReceiver *comRec, char function,char address,char job, void * pMem)
{
  if(address=='2')
  {
    aktiviereStromStoss();
  }
}

void jobWriteCard(ComReceiver *comRec, char function,char address,char job, void * pMem)
{
  uint8_t i;
  for(i=0;i<KEY_LENGTH;i++)
    cardKey[i] = ((uint8_t*) pMem)[i];
  for(i=0;i<INFO_LENGTH;i++)
    cardInfo[i] = ((uint8_t*) pMem)[i+KEY_LENGTH];
  cardNumber = address-'0';
  if(cardNumber<KEY_NUM)
    cardStatus = CARD_WAITING_FOR_WRITE;
  else
    cardStatus = CARD_SELECTED;
}

void jobWaitForClear(ComReceiver *comRec, char function,char address,char job, void * pMem)
{
  cardStatus = CARD_WAITING_FOR_CLEAR;
}


void jobClearCard(ComReceiver *comRec, char function,char address,char job, void * pMem)
{
  uint8_t i;
  for(i=0;i<KEY_LENGTH;i++)
    cardKey[i] = ((uint8_t*) pMem)[i];
  cardNumber = address-'0';
  if(cardNumber<KEY_NUM)
  {
    clearCard(cardNumber,cardKey);
  }

  cardStatus = CARD_SELECTED;
}

void jobReleaseCard(ComReceiver *comRec, char function,char address,char job, void * pMem)
{
}

void jobgotCardKey(ComReceiver *comRec, char function,char address,char job, void * pMem)
{
  uint8_t *key = (uint8_t*) pMem;
  uint8_t info[INFO_LENGTH];

  if(rc522_read_block(SECTOR_INFO,SUB_INFO,key,(char*)info,INFO_LENGTH))
  {
    comRec->Getoutput()->sendStandardByteArray(info,INFO_LENGTH,klingelNode,'C',address,'i','T');
  }
}

void jobReceiveRandom(ComReceiver *comRec, char function,char address,char job, void * pMem)
{
uint8_t i;
  uint8_t adr = getAddress(address);
  if(adr<2)
  {
    for(i=0;i<16;i++)
    {
      Actual_Random[adr][i] = ( (uint8_t *)pMem )[i];
    }
    Valid_Random[adr] = true;
  }
}

