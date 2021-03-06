/*
 * Name RC522DumpMifare.ino
 *
 * 2013-07-xx started
 * 2013-08-03 first commit
 * 2013-08-08 anticol/select 7byte UID, RATS
 *
 * Based on code by www.electrodragon.com
 * and modified/fixed by http://twitter.com/regnerischerTag
 *
 * TODO:
 *  - ISO/IEC 14443-4
 *  - Auth 7byte
 *
 */

#include "Bedienung.h"


// the sensor communicates using SPI, so include the library:
// #include <SPI.h>


//data array maxium length
#define MAX_LEN 16

/////////////////////////////////////////////////////////////////////
//set the pin
/////////////////////////////////////////////////////////////////////

PORT_t *rc522_port;
#define chipSelectPin PIN4_bm
#define NRSTPD PIN1_bm
#define SPI_SCK PIN7_bm
#define SPI_MOSI PIN5_bm

SPI_t *rc522_spi;
/* ####
const int chipSelectPin = 10;
const int NRSTPD = 5;####*/


//4 bytes Serial number of card, the 5th byte is crc
uchar serNum[5];
//7 bytes Serial number of card, the 8th byte is crc
uchar serNum7[8];
//buffer
//uchar str[MAX_LEN];


void rc522_setup()
{
  rc522_port = &PORTD;
  rc522_port->DIRSET = chipSelectPin | NRSTPD | SPI_SCK | SPI_MOSI;
  rc522_port->OUTCLR = chipSelectPin;
  rc522_port->OUTSET = NRSTPD;

  /*####Serial.begin(9600);              */         // RFID reader SOUT pin connected to Serial RX pin at 2400bps
  // start the SPI library:
  // #### SPI.begin();
  rc522_spi = &SPID;
  rc522_spi->CTRL = SPI_ENABLE_bm | SPI_MODE_0_gc | SPI_MASTER_bm | SPI_PRESCALER_DIV4_gc;


/*####  pinMode(chipSelectPin,OUTPUT);         // Set digital pin 10 as OUTPUT to connect it to the RFID /ENABLE pin
  digitalWrite(chipSelectPin, LOW);      // Activate the RFID reader
  pinMode(NRSTPD,OUTPUT);                // Set digital pin 10 , Not Reset and Power-down
  digitalWrite(NRSTPD, HIGH);*/


  MFRC522_Init();

  //display version info
  //9.3.4.8 VersionReg register : 0x91 / 0x92
  char temp[20];
  uchar version = Read_MFRC522(VersionReg);
  sprintf(temp,"MFRC522 V.: %x",version);
  cmulti.sendInfo(temp,klingelNode);

  //output_str(KNET,"\nMFRC522 Version: 0x"); //####  Serial.print("MFRC522 Version: 0x");
  //output_hex(KNET,version);				 //#### Serial.println(version, HEX);
  //output_str(KNET,"\n");

}

uint8_t rc522_read_block(uint8_t sector, uint8_t sub, uchar *key, char *info, uint8_t length)
{
	uchar status;
	uchar buffer[16];
//	uint8_t i;

	status = MFRC522_Auth(PICC_AUTHENT1A, sector*4+sub, key, serNum);
	if (status == MI_OK)
	{
		status = MFRC522_Read(sector*4+sub,buffer);
	}

	memcpy(info,buffer,length);
	if (status == MI_OK)
		return(true);
	else
		return(false);
}

uint8_t rc522_write_block(uint8_t sector, uint8_t sub, uchar *key, char *info, uint8_t length)
{
uchar status;
uchar buffer[16];
uint8_t i;

	for (i=0;i<15;i++)
	{
		if(i<length)
			buffer[i] = info[i];
		else
			buffer[i] = 0;
	}
	status = MFRC522_Auth(PICC_AUTHENT1A, sector*4+sub, key, serNum);
	if (status == MI_OK)
	{
		status = MFRC522_Write(sector*4+sub,buffer);
	}
	if (status == MI_OK)
		return(true);
	else
		return(false);
}

uint8_t rc522_write_key(uint8_t sector, uchar *oldkey, uchar *newkey)
{
	uchar status;
	uchar buffer[16];
	uint8_t i;

	for (i=0;i<6;i++)
		buffer[i] = newkey[i];
	for (i=0;i<10;i++)
		buffer[i+6] = conditionBits_KeyB[i];
/*	status = MFRC522_Auth(PICC_AUTHENT1A, (sector-1)*4+2, oldkey, serNum);
	if (status == MI_OK)
	{
		status = MFRC522_Write((sector-1)*4+2,buffer);
	}*/
	status = MFRC522_Auth(PICC_AUTHENT1A, sector*4+3, oldkey, serNum);
	if (status == MI_OK)
	{
		status = MFRC522_Write(sector*4+3,buffer);
	}
	if (status == MI_OK)
		return(true);
	else
		return(false);
}

void rc522_loop()
{
    uchar status;
    uchar buffer[MAX_LEN];
    if (selectCard(true))
    {
         for(int block=0; block < 64; block++)
         {
              status = MFRC522_Auth(PICC_AUTHENT1A, block, defaultKeyA, serNum); //auth with default key
              if (status != MI_OK)
              {
                   selectCard(false);
                   status = MFRC522_Auth(PICC_AUTHENT1A, block, diffkey, serNum); //auth with my own key
              }
              else
              {
                cmulti.sendInfo("Identifiziert mit defaultKeyA.",klingelNode); //output_str(KNET,"Identifiziert mit defaultKeyA"); //####
              }
              if (status != MI_OK)
              {
                   selectCard(false);
                   status = MFRC522_Auth(PICC_AUTHENT1A, block, newkey, serNum); //auth NDEF data key
              }
              if (status == MI_OK)
              {
				   if (block==70)
				   {
					   status = MFRC522_Write(4,myinfo);
					   status = MFRC522_Write(5,myinfo);
					   if (status== MI_OK)
					   {
						   cmulti.sendInfo("Erfolgreich geschrieben.",klingelNode); //output_str(KNET,"Erfolgreich geschrieben \n\r"); //####
					   }
					   else
					   {
						   cmulti.sendInfo("Da klappt noch was nicht.",klingelNode); //output_str(KNET,"Da klappt noch was nicht \n\r"); //####
					   }
				   }
                   status = MFRC522_Read(block, buffer);
                   if (status == MI_OK)
                   {
                        if (block % 4 == 0)
                        {
                          ;
                            // !!! // output_str(KNET,"Sector "); //####  Serial.print("Sector ");
                            // !!! // output_dec(KNET,block/4,6,' '); //####  Serial.print(block / 4, DEC);
                            // !!! // output_str(KNET,": \n\r"); //####Serial.println(": ");

                        }
                        dumpHex((char*)buffer, MAX_LEN);
                    }
                    else
                    {
                        cmulti.sendInfo("Read failed.",klingelNode); //output_str(KNET,"Read failed"); //####  Serial.println("Read failed");
                        break;
                    }
              }
              else
              {
                  cmulti.sendInfo("Auth failed.",klingelNode); //output_str(KNET,"Auth failed"); //####  Serial.println("Auth failed");
                  //TODO Mifare Ultra-Light
                  //MIFARE Ultralight C http://www.nxp.com/documents/short_data_sheet/MF0ICU2_SDS.pdf
                  break;
              }
         }//for
//         _delay_ms(1000); // original 1000
    }
    else
    {
        //output_str(KNET,"-"); //####  Serial.println("no card select");
    }
    //reset/init for next loop
    MFRC522_Init();
    _delay_ms(50); // original 1500
}

int selectCard(int dumpInfo)
{
  uchar status;
  uchar buffer[MAX_LEN];
  //Search card, return card types
  status = MFRC522_Request(PICC_REQIDL, buffer);//ShortFrame: 0x26 REQA (Request Type A)
  //status = MFRC522_Request(PICC_REQALL, buffer);//0x52 WUPA (Wake-Up) // diese war auskommentiert
  if (dumpInfo)
    cmulti.sendInfo("Status",klingelNode); //  output_print(KNET,"Status: %d\n\r",status);
  if (status == MI_OK)
  {
     if (dumpInfo)
     {
         cmulti.sendInfo("Card detected. ATQA:",klingelNode); //output_str(KNET,"Card detected.\r\n ATQA:"); //####  Serial.print("Card detected.\r\n ATQA:");
         dumpHex((char*)buffer, 2);
         // output_str(KNET,"\n\r"); //####  Serial.println(" ");
     }
     //Prevent conflict, return the 4 bytes Serial number of the card
     status = MFRC522_Anticoll(buffer);
     if (status == MI_OK)
     {
         memcpy(serNum, buffer, 5);
         uchar sak = 0;
//		 output_str(KNET,"Irgendwas gefunden\n\r"); // ***********************
         status = MFRC522_SelectTag(serNum, &sak);
         if (status == MI_OK && ((sak & 0x04) == 0x00))
         {

             if (dumpInfo)
             {
                 // output_str(KNET," UID: "); //####  Serial.print(" UID: ");
                 // dumpHex((char*)serNum, 4);
                 // output_str(KNET,"\n\r"); //####  Serial.println("");
             }
             if ((sak & 0x20) == 0x20)
             {
                 //ISO/IEC FCD 14443-3: Table 9 — Coding of SAK
                 //if (dumpInfo)
                 //    Serial.println(" UID complete, PICC compliant with ISO/IEC 14443-4");
                 //send RATS (Request for Answer To Select)
                 uchar ats[MAX_LEN];
                 uint unLen = 0;
                 status = MFRC522_RATS(ats, &unLen);
                 if (status == MI_OK && dumpInfo)
                 {
                      // output_str(KNET," ATS: "); //####  Serial.println(" ATS: ");
                      // dumpHex((char*)ats, ats[0]);
                      // output_str(KNET,"\n\r"); //####  Serial.println("");
                 }
             }
             if (dumpInfo)
             {
                  ;
                 // !!! // output_str(KNET," SAK: "); //####  Serial.print(" SAK: ");
                 // !!! // output_hex(KNET,sak);      //##### Serial.print(sak, HEX);
                 // !!! // output_str(KNET,"\n\r");     //####  Serial.println("");
             }
             return(true);
         }
         else
         {
             //cascading level 2
             memcpy(serNum7, &serNum[1], 3);//cascading L1
             status = MFRC522_Anticoll2(buffer);
             if (status == MI_OK)
             {
                 memcpy(&serNum7[3], buffer, 4);
                 status = MFRC522_SelectTag2(&serNum7[3], &sak);
                 if (dumpInfo)
                 {
                  ;
                    // !!! // output_str(KNET," UID7: "); //####  Serial.print(" UID: ");
                    // !!! // dumpHex((char*)serNum7, 7);
                    // !!! // output_str(KNET,"\n\r");     //####  Serial.println("");
                    // !!! // output_str(KNET," SAK: "); //####  Serial.print(" SAK: ");
                    // !!! // output_hex(KNET,sak);      //##### Serial.print(sak, HEX);
                    // !!! // output_str(KNET,"\n\r");	  //####  Serial.println("");
                 }
                 return(true);
             }
             else
             {
                cmulti.sendInfo("ANTICOLL error cascading level 2",klingelNode); //output_str(KNET,"ANTICOLL error cascading level 2");
				// ####  Serial.println("ANTICOLL error: cascading level 2");
             }
         }
   }//Anticoll
   else
   {
       cmulti.sendInfo("ANTICOLL failed",klingelNode); //output_str(KNET,"ANTICOLL failed"); //####  Serial.print("ANTICOLL failed");
   }
 }
 else
 {
	 //output_str(KNET,"-");
     //Serial.print("-");
 }
 return(false);
}//selectCard

/*
 * Function:Write_MFRC5200
 * Description:write a byte data into one register of MR RC522
 * Input parameter:addr--register address;val--the value that need to write in
 * Return:Null
 */
void Write_MFRC522(uchar addr, uchar val)
{
	rc522_port->OUTCLR = chipSelectPin; //#### digitalWrite(chipSelectPin, LOW);
	_delay_us(10);

	//address format:0XXXXXX0
	SPI_Transfer(rc522_spi,(addr<<1)&0x7E);		//#### SPI.transfer((addr<<1)&0x7E);
	SPI_Transfer(rc522_spi,val);		//#### SPI.transfer(val);

	rc522_port->OUTSET = chipSelectPin; //#### digitalWrite(chipSelectPin, HIGH);
	_delay_us(10);
}

uchar SPI_Transfer(SPI_t *spi, uchar val)
{
	spi->DATA = val;
	do
	{
	} while ( !(spi->STATUS & SPI_IF_bm) );
	_delay_us(5);
	return(spi->DATA);
}


/*
 * Function:Read_MFRC522
 * Description:read a byte data into one register of MR RC522
 * Input parameter:addr--register address
 * Return:return the read value
 */
uchar Read_MFRC522(uchar addr)
{
	uchar val;

	rc522_port->OUTCLR = chipSelectPin; //#### digitalWrite(chipSelectPin, LOW);
	_delay_us(10);
	//address format:1XXXXXX0
	SPI_Transfer( rc522_spi,((addr<<1)&0x7E) | 0x80 ); // #### SPI.transfer(((addr<<1)&0x7E) | 0x80);
	val = SPI_Transfer( rc522_spi,0x00 );// #### val =SPI.transfer(0x00);

	rc522_port->OUTSET = chipSelectPin; //#### digitalWrite(chipSelectPin, HIGH);
	_delay_us(10);

	return val;
}

/*
 * Function:SetBitMask
 * Description:set RC522 register bit
 * Input parameter:reg--register address;mask--value
 * Return:null
 */
void SetBitMask(uchar reg, uchar mask)
{
    uchar tmp;
    tmp = Read_MFRC522(reg);
    Write_MFRC522(reg, tmp | mask);  // set bit mask
}


/*
 * Function:ClearBitMask
 * Description:clear RC522 register bit
 * Input parameter:reg--register address;mask--value
 * Return:null
 */
void ClearBitMask(uchar reg, uchar mask)
{
    uchar tmp;
    tmp = Read_MFRC522(reg);
    Write_MFRC522(reg, tmp & (~mask));  // clear bit mask
}


/*
 * Function:AntennaOn
 * Description:Turn on antenna, every time turn on or shut down antenna need at least 1ms delay
 * Input parameter:null
 * Return:null
 */
void AntennaOn(void)
{
	uchar temp;

	temp = Read_MFRC522(TxControlReg);
	if (!(temp & 0x03))
	{
		SetBitMask(TxControlReg, 0x03);
	}
}


/*
 * Function:AntennaOff
 * Description:Turn off antenna, every time turn on or shut down antenna need at least 1ms delay
 * Input parameter:null
 * Return:null
 */
void AntennaOff(void)
{
	ClearBitMask(TxControlReg, 0x03);
}


/*
 * Function:ResetMFRC522
 * Description: reset RC522
 * Input parameter:null
 * Return:null
 */
void MFRC522_Reset(void)
{
    Write_MFRC522(CommandReg, PCD_RESETPHASE);
}


/*
 * Function:InitMFRC522
 * Description:initilize RC522
 * Input parameter:null
 * Return:null
 */
void MFRC522_Init(void)
{
   rc522_port->OUTSET =  NRSTPD; // #### digitalWrite(NRSTPD,HIGH);

   MFRC522_Reset();

    //Timer: TPrescaler*TreloadVal/6.78MHz = 24ms
    Write_MFRC522(TModeReg, 0x8D);		//Tauto=1; f(Timer) = 6.78MHz/TPreScaler
    Write_MFRC522(TPrescalerReg, 0x3E);	//TModeReg[3..0] + TPrescalerReg
    Write_MFRC522(TReloadRegL, 30);
    Write_MFRC522(TReloadRegH, 0);

    Write_MFRC522(TxAutoReg, 0x40);		//100%ASK
    Write_MFRC522(ModeReg, 0x3D);		//CRC initilizate value 0x6363	???

    //ClearBitMask(Status2Reg, 0x08);		//MFCrypto1On=0
    //Write_MFRC522(RxSelReg, 0x86);		//RxWait = RxSelReg[5..0]
    //Write_MFRC522(RFCfgReg, 0x7F);   		//RxGain = 48dB

    AntennaOn();		//turn on antenna
}


/*
 * Function:MFRC522_Request
 * Description:Searching card, read card type
 * Input parameter:reqMode--search methods,
 *			 TagType--return card types
 *			 	0x4400 = Mifare_UltraLight
 *				0x0400 = Mifare_One(S50)
 *				0x0200 = Mifare_One(S70)
 *				0x0800 = Mifare_Pro(X)
 *				0x4403 = Mifare_DESFire
 * return:return MI_OK if successed
 */
uchar MFRC522_Request(uchar reqMode, uchar *TagType)
{
	uchar status;
	uint backBits;			//the data bits that received

	Write_MFRC522(BitFramingReg, 0x07);		//TxLastBists = BitFramingReg[2..0]	???

	TagType[0] = reqMode;
	status = MFRC522_ToCard(PCD_TRANSCEIVE, TagType, 1, TagType, &backBits);

	if ((status != MI_OK) || (backBits != 0x10))
	{
	     status = MI_ERR;
//		 output_print(KNET,"status: %d, backbits: %d\n\r",status,backBits);
/*
             Serial.print("status: ");
             Serial.print(status, HEX);
             Serial.print(" backBits: ");
             Serial.print(backBits, HEX);
             Serial.println("");
*/
	}
	return status;
}


/*
 * Function:MFRC522_ToCard
 * Description:communicate between RC522 and ISO14443
 * Input parameter:command--MF522 command bits
 *			 sendData--send data to card via rc522
 *			 sendLen--send data length
 *			 backData--the return data from card
 *			 backLen--the length of return data
 * return:return MI_OK if successed
 */
uchar MFRC522_ToCard(uchar command, uchar *sendData, uchar sendLen, uchar *backData, uint *backLen)
{
    uchar status = MI_ERR;
    uchar irqEn = 0x00;
    uchar waitIRq = 0x00;
    uchar lastBits;
    uchar n;
    uint i;

    switch (command)
    {
        case PCD_AUTHENT:		//verify card password
		{
			irqEn = 0x12;
			waitIRq = 0x10;
			break;
		}
	case PCD_TRANSCEIVE:	//send data in the FIFO
		{
			irqEn = 0x77;
			waitIRq = 0x30;
			break;
		}
	default:
			break;
    }

    Write_MFRC522(CommIEnReg, irqEn|0x80);	//Allow interruption
    ClearBitMask(CommIrqReg, 0x80);			//Clear all the interrupt bits
    SetBitMask(FIFOLevelReg, 0x80);			//FlushBuffer=1, FIFO initilizate

    Write_MFRC522(CommandReg, PCD_IDLE);	//NO action;cancel current command	???

    //write data into FIFO
    for (i=0; i<sendLen; i++)
    {
        Write_MFRC522(FIFODataReg, sendData[i]);
    }

    //procceed it
    Write_MFRC522(CommandReg, command);
    if (command == PCD_TRANSCEIVE)
    {
        SetBitMask(BitFramingReg, 0x80);		//StartSend=1,transmission of data starts
    }

    //waite receive data is finished
    i = 2000;	//i should adjust according the clock, the maxium the waiting time should be 25 ms???
    do
    {
	//CommIrqReg[7..0]
	//Set1 TxIRq RxIRq IdleIRq HiAlerIRq LoAlertIRq ErrIRq TimerIRq
        n = Read_MFRC522(CommIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x01) && !(n&waitIRq));

    ClearBitMask(BitFramingReg, 0x80);			//StartSend=0

    if (i != 0)
    {
        if(!(Read_MFRC522(ErrorReg) & 0x1B))	//BufferOvfl Collerr CRCErr ProtecolErr
        {
			status = MI_OK;
            if (n & irqEn & 0x01)
            {
	           // output_str(KNET,"."); //NoTagError\n\r
               status = MI_NOTAGERR;			//??
            }

            if (command == PCD_TRANSCEIVE)
            {
               	n = Read_MFRC522(FIFOLevelReg);
              	lastBits = Read_MFRC522(ControlReg) & 0x07;
                if (lastBits)
                {
					*backLen = (n-1)*8 + lastBits;
				}
                else
                {
					*backLen = n*8;
				}

                if (n == 0)
                {
					n = 1;
				}
                if (n > MAX_LEN)
                {
					n = MAX_LEN;
				}

		//read the data from FIFO
                for (i=0; i<n; i++)
                {
					backData[i] = Read_MFRC522(FIFODataReg);
//					output_hex(KNET,backData[i]);
//					output_str(KNET,";");
				}
//				output_str(KNET,"\n\r");
            }
        }
        else
        {
			status = MI_ERR;
		}
    }
    else
    {
//         output_str(KNET,"i=0\n\r");
    }

    //SetBitMask(ControlReg,0x80);           //timer stops
    //Write_MFRC522(CommandReg, PCD_IDLE);

    return status;
}


/*
 * Function:MFRC522_Anticoll
 * Description:Prevent conflict, read the card serial number
 * Input parameter:serNum--return the 4 bytes card serial number, the 5th byte is recheck byte
 * return:return MI_OK if successed
 */
uchar MFRC522_Anticoll(uchar *serNum)
{
    uchar status;
    uchar i;
    uchar serNumCheck=0;
    uint unLen;


    //ClearBitMask(Status2Reg, 0x08);		//TempSensclear
    //ClearBitMask(CollReg,0x80);		//ValuesAfterColl
    Write_MFRC522(BitFramingReg, 0x00);		//TxLastBists = BitFramingReg[2..0]

    serNum[0] = PICC_ANTICOLL;
    serNum[1] = 0x20;
    status = MFRC522_ToCard(PCD_TRANSCEIVE, serNum, 2, serNum, &unLen);

    if (status == MI_OK)
    {
	 //Verify card serial number
         for (i=0; i<4; i++)
	 {
	     serNumCheck ^= serNum[i];
	 }
	 if (serNumCheck != serNum[i])
	 {
	     status = MI_ERR;
	 }
    }
    //SetBitMask(CollReg, 0x80);		//ValuesAfterColl=1
    return status;
}

//ANTICOLL cascading level 2
uchar MFRC522_Anticoll2(uchar *serNum)
{
    uchar status;
    uchar i;
    uchar serNumCheck=0;
    uint unLen;


    //ClearBitMask(Status2Reg, 0x08);		//TempSensclear
    //ClearBitMask(CollReg,0x80);		//ValuesAfterColl
    Write_MFRC522(BitFramingReg, 0x00);		//TxLastBists = BitFramingReg[2..0]

    serNum[0] = PICC_ANTICOLL2;
    serNum[1] = 0x20;
    status = MFRC522_ToCard(PCD_TRANSCEIVE, serNum, 2, serNum, &unLen);

    if (status == MI_OK)
    {
	 //Verify card serial number
         for (i=0; i<4; i++)
	 {
	     serNumCheck ^= serNum[i];
	 }
	 if (serNumCheck != serNum[i])
	 {
	     status = MI_ERR;
	 }
    }
    //SetBitMask(CollReg, 0x80);		//ValuesAfterColl=1
    return status;
}

//send RATS and returns ATS
uchar MFRC522_RATS(uchar *recvData, uint *pLen)
{
    uchar status;
    uint unLen = 0;

    recvData[0] = 0xE0; // RATS
    recvData[1] = 0x50; // FSD=128, CID=0
    CalulateCRC(recvData,2, &recvData[2]);
    status = MFRC522_ToCard(PCD_TRANSCEIVE, recvData, 4, recvData, &unLen);
    /*
    Serial.print("unLen: ");
    Serial.print(unLen, HEX);
    //Serial.print(" status: ");
    Serial.print(status, HEX);
    Serial.println("");
    */
    //TODO
    //if ((status != MI_OK) || (unLen != 0x90))
    //{
    //    status = MI_ERR;
    //}
    return status;
}


/*
 * Function:CalulateCRC
 * Description:Use MF522 to caculate CRC
 * Input parameter:pIndata--the CRC data need to be read,len--data length,pOutData-- the caculated result of CRC
 * return:Null
 */
void CalulateCRC(uchar *pIndata, uchar len, uchar *pOutData)
{
    uchar i, n;

    ClearBitMask(DivIrqReg, 0x04);			//CRCIrq = 0
    SetBitMask(FIFOLevelReg, 0x80);			//Clear FIFO pointer
    //Write_MFRC522(CommandReg, PCD_IDLE);

	//Write data into FIFO
    for (i=0; i<len; i++)
    {
        Write_MFRC522(FIFODataReg, *(pIndata+i));
    }
    Write_MFRC522(CommandReg, PCD_CALCCRC);

	//waite CRC caculation to finish
    i = 0xFF;
    do
    {
        n = Read_MFRC522(DivIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x04));			//CRCIrq = 1

	//read CRC caculation result
    pOutData[0] = Read_MFRC522(CRCResultRegL);
    pOutData[1] = Read_MFRC522(CRCResultRegM);
}


/*
 * Function:MFRC522_SelectTag
 * Description:Select card, read card storage volume
 * Input parameter:serNum--Send card serial number
 * sak see ISO14443-3 Table 9 — Coding of SAK
 * return return MI_OK if successed
 */
uchar MFRC522_SelectTag(uchar *serNum, uchar *sak)
{
    uchar i;
    uchar status;
    //uchar size;
    uint recvBits;
    uchar buffer[9];
    //uchar buffCheck=0;

    //ClearBitMask(Status2Reg, 0x08);			//MFCrypto1On=0

    buffer[0] = PICC_SElECTTAG;
    buffer[1] = 0x70;
    for (i=0; i<5; i++)
    {
    	buffer[i+2] = *(serNum+i);
    }
    CalulateCRC(buffer, 7, &buffer[7]);		//??
    status = MFRC522_ToCard(PCD_TRANSCEIVE, buffer, 9, buffer, &recvBits);
    //TODO: the above call returns 2 instead of MI_OK -> why?
    status = MI_OK;
    //Serial.print("recvBits: ");
    //Serial.print(recvBits, DEC);
    /*
    for (i=0; i<recBits / 8; i++)
    {
    	buff[i] = *(buffer+i);
    }*/
    //dumpHex((char*)buffer, recvBits / 8);
    *sak = buffer[0];
    //Verify received buffer
    /* TODO
    for (i=0; i< recvBits/8; i++)
    {
       buffCheck ^= buffer[i];
    }
    if (buffCheck != buffer[i])
    {
       status = MI_ERR;
    }*/
    return status;
}

uchar MFRC522_SelectTag2(uchar *serNum, uchar *sak)
{
    uchar i;
    uchar status;
    //uchar size;
    uint recvBits;
    uchar buffer[9];
    //uchar buffCheck=0;

    //ClearBitMask(Status2Reg, 0x08);			//MFCrypto1On=0

    buffer[0] = PICC_ANTICOLL2;
    buffer[1] = 0x70;
    for (i=0; i<5; i++)
    {
    	buffer[i+2] = *(serNum+i);
    }
    CalulateCRC(buffer, 7, &buffer[7]);
    status = MFRC522_ToCard(PCD_TRANSCEIVE, buffer, 9, buffer, &recvBits);
    //TODO: the above call returns 2 instead of MI_OK -> why?
    status = MI_OK;
    //Serial.print("recvBits: ");
    //Serial.print(recvBits, DEC);
    /*
    for (i=0; i<recBits / 8; i++)
    {
    	buff[i] = *(buffer+i);
    }*/
    //dumpHex((char*)buffer, recvBits / 8);
    *sak = buffer[0];
    //Verify received buffer
    /* TODO
    for (i=0; i< recvBits/8; i++)
    {
       buffCheck ^= buffer[i];
    }
    if (buffCheck != buffer[i])
    {
       status = MI_ERR;
    }*/
    return status;
}


/*
 * Function:MFRC522_Auth
 * Description:verify card password
 * Input parameters:authMode--password verify mode
                 0x60 = verify A passowrd key
                 0x61 = verify B passowrd key
             BlockAddr--Block address
             Sectorkey--Block password
             serNum--Card serial number ,4 bytes
 * return:return MI_OK if successed
 */
uchar MFRC522_Auth(uchar authMode, uchar BlockAddr, uchar *Sectorkey, uchar *serNum)
{
    uchar status;
    uint recvBits;
    uchar i;
    uchar buff[12];

    //Verify command + block address + buffer password + card SN
    buff[0] = authMode;
    buff[1] = BlockAddr;
    for (i=0; i<6; i++)
    {
        buff[i+2] = *(Sectorkey+i);
    }
    for (i=0; i<4; i++)
    {
        buff[i+8] = *(serNum+i);
    }
    status = MFRC522_ToCard(PCD_AUTHENT, buff, 12, buff, &recvBits);

    if ((status != MI_OK) || (!(Read_MFRC522(Status2Reg) & 0x08)))
    {
        status = MI_ERR;
    }

    return status;
}


/*
 * Function:MFRC522_Read
 * Description:Read data
 * Input parameters:blockAddr--block address;recvData--the block data which are read
 * return:return MI_OK if successed
 */
uchar MFRC522_Read(uchar blockAddr, uchar *recvData)
{
    uchar status;
    uint unLen;

    recvData[0] = PICC_READ;
    recvData[1] = blockAddr;
    CalulateCRC(recvData,2, &recvData[2]);
    status = MFRC522_ToCard(PCD_TRANSCEIVE, recvData, 4, recvData, &unLen);

    if ((status != MI_OK) || (unLen != 0x90))
    {
        status = MI_ERR;
    }

    return status;
}


/*
 * Function:MFRC522_Write
 * Description:write block data
 * Input parameters:blockAddr--block address;writeData--Write 16 bytes data into block
 * return:return MI_OK if successed
 */
uchar MFRC522_Write(uchar blockAddr, uchar *writeData)
{
    uchar status;
    uint recvBits;
    uchar i;
    uchar buff[18];

    buff[0] = PICC_WRITE;
    buff[1] = blockAddr;
    CalulateCRC(buff, 2, &buff[2]);
    status = MFRC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff, &recvBits);

    if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A))
    {
      status = MI_ERR;
      cmulti.sendInfo("Fehler 1",klingelNode); //output_str(KNET,"Fehler 1\n\r");
    }

    if (status == MI_OK)
    {
        for (i=0; i<16; i++)		//Write 16 bytes data into FIFO
        {
        	buff[i] = *(writeData+i);
        }
        CalulateCRC(buff, 16, &buff[16]);
        status = MFRC522_ToCard(PCD_TRANSCEIVE, buff, 18, buff, &recvBits);

		if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A))
        {
			status = MI_ERR;
			cmulti.sendInfo("Fehler 2",klingelNode); //output_str(KNET,"Fehler 2\n\r");
		}
    }

    return status;
}

/*
 * Function:MFRC522_Halt
 * Description:Command the cards into sleep mode
 * Input parameters:null
 * return:null
 */
void MFRC522_Halt(void)
{
    uint unLen;
    uchar buff[4];

    //ISO14443-3: 6.4.3 HLTA command
    buff[0] = PICC_HALT;
    buff[1] = 0;
    CalulateCRC(buff, 2, &buff[2]);

    MFRC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff, &unLen);
}

void dumpHex(char* buffer, int len)
{
  for(byte i=0; i < len; i++) {
//     char text[4];
     if (i % 16 == 0) {
        ;
        // !!! // output_str(KNET," "); //####  Serial.print(" ");
     }
	 // !!! // output_print(KNET,"%x ",(byte)(*(buffer + i)) );
     //####  sprintf(text, "%02X \x00", (byte)(*(buffer + i)));
     //####  Serial.print(text);

     if (i % 16 == 15) {
        ;
        // !!! // output_str(KNET,"\n\r"); //####  Serial.println("");
     }
  }
  //Serial.println(" ");
}
