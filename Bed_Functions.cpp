/*
 * Bed_Functions.cpp
 *
 * Created: 21.02.2016 16:43:48
 *  Author: Christof
 */

#include "Bedienung.h"

#define NUM_COMMANDS 4 + CMULTI_STANDARD_NUM

COMMAND cnetCommands[NUM_COMMANDS] =
{
  cmultiStandardCommands,
  {'R','s',CUSTOMER,NOPARAMETER,0,jobReceiveRandom},
  {'C','k',CUSTOMER,BYTEARRAY,KEY_LENGTH,jobgotCardKey},
  {'C','r',CUSTOMER,NOPARAMETER,0,jobReleaseCard},
  {'C','w',CUSTOMER,BYTEARRAY,KEY_LENGTH+INFO_LENGTH,jobWriteCard},
};

#define NUM_INFORMATION 1

INFORMATION cnetInformation[NUM_INFORMATION]=
{
  {klingelNode,'S','1','s',STRING,15,(void*)&sLEDStatus,gotNewLedStatus}
};

ComReceiver cnetCom(&cmulti,Node, cnetCommands,NUM_COMMANDS, cnetInformation,NUM_INFORMATION,NULL,NULL);

void gotNewLedStatus()
{
  refresh_led_new();
  Klingel_LED_Dimmer = (uint8_t)sLEDStatus[11]-65;
}

void jobWriteCard(ComReceiver *comRec, char function,char address,char job, void * pMem)
{

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

