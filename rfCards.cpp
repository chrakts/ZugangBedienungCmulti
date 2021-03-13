#include "Bedienung.h"

volatile uint8_t cardStatus=CARD_UNSELECTED,cardNumber;
volatile uint8_t cardKey[KEY_LENGTH],cardInfo[INFO_LENGTH];


uint8_t writeNewCard(volatile uint8_t cardnum,volatile uint8_t* key,volatile uint8_t* info)
{
  uint8_t ret = false;
  if( rc522_write_key(SECTOR_INFO, defaultKeyA, key) )
  {
    if ( rc522_write_block(SECTOR_INFO,SUB_INFO,key,(char*)info,INFO_LENGTH) ) // Info in Sub 0
    {
      info[0] = cardnum;
      if ( rc522_write_block(SECTOR_CARDNUM,SUB_CARDNUM,defaultKeyA,(char*)info,1) )	// Kartennummer in Sub 1
      {
        cmulti.sendCommand(klingelNode,'C','1','c');
        MFRC522_Init();
        ret=true;
      }
    }
  }
  return(ret);
}

uint8_t clearCard(volatile uint8_t cardnum,volatile uint8_t* oldkey)
{
  uint8_t ret = false,i;
  char info[INFO_LENGTH];
  for(i=0;i<INFO_LENGTH;i++)
    info[i] = 0xff;
  if( rc522_write_key(SECTOR_INFO, oldkey, defaultKeyA) )
  {
    if ( rc522_write_block(SECTOR_INFO,SUB_INFO,defaultKeyA,(char*)info,INFO_LENGTH) ) // Info in Sub 0
    {
      if ( rc522_write_block(SECTOR_CARDNUM,SUB_CARDNUM,defaultKeyA,info,1) )	// Kartennummer in Sub 1
      {
        cmulti.sendCommand(klingelNode,'C','1','c');
        MFRC522_Init();
        ret=true;
      }
    }
  }
  return(ret);
}

uint8_t get_card_number()
{
uint8_t cardnum;
uint8_t info[INFO_LENGTH+1];		// plus eins wegen Kartennummer

	if(rc522_read_block(SECTOR_CARDNUM,SUB_CARDNUM,defaultKeyA,(char*)info,1))
	{
		cardnum = (uint8_t)info[0];
		return( cardnum );
	}
	else
		return( 255 );
}


uint8_t getAddress(char adr)
{
  if(adr== '1')
    return(1);
  else
    return(0);
}
