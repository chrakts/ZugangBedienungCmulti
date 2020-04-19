/*
 * Bed_Functions.cpp
 *
 * Created: 21.02.2016 16:43:48
 *  Author: Christof
 */

#include "Bedienung.h"

//const char *fehler_text[]={"Speicherfehler","Parameterfehler","keine SMS","kein Handy","Antwort falsch","Job Fehler","keine Uebertragung"};


uint8_t rec_state_KNET=RCST_WAIT;
uint8_t function_KNET=0;
uint8_t job_KNET=0;

char *parameter_text_KNET=NULL;
uint8_t parameter_text_length_KNET;
uint8_t parameter_text_pointer_KNET;

uint8_t bootloader_attention;		// nur wenn true, dann darf Bootloader gestartet werden.
uint8_t reset_attention;			// nur wenn true, dann darf Reset ausgeloest werden.

void (*bootloader)( void ) = (void (*)(void)) (BOOT_SECTION_START/2);       // Set up function pointer
void (*reset)( void ) = (void (*)(void)) 0x0000;       // Set up function pointer



#define NUM_COMMANDS 15

COMMAND cnetCommands[NUM_COMMANDS] =
{
  {'S','K',CUSTOMER,STRING,16,jobSetSecurityKey},
  {'S','k',CUSTOMER,NOPARAMETER,0,jobGetSecurityKey},
  {'S','C',DEVELOPMENT,NOPARAMETER,0,jobGetCompilationDate},
  {'S','T',DEVELOPMENT,NOPARAMETER,0,jobGetCompilationTime},
  {'S','m',PRODUCTION,NOPARAMETER,0,jobGetFreeMemory},
  {'M','A',CUSTOMER,STRING,16,setBootloaderAttention},    //JOB_BL_ATTENTION
  {'M','B',CUSTOMER,NOPARAMETER,0,startBootloader},    // JOB_BL_START		'B'
  {'R','s',CUSTOMER,NOPARAMETER,0,jobReceiveRandom},    // JOB_RESET			'R'
  {'C','b',CUSTOMER,UINT_8,1,jobGetBlockingStatus},    // JOB_DC_ATTENTION	'D'			// Direkten Channel zum rueckwaerigen Geraet
  {'D','o',CUSTOMER,NOPARAMETER,0,jobDoorIsOpen},    // JOB_DC_ATTENTION	'D'			// Direkten Channel zum rueckwaerigen Geraet
  {'M','C',CUSTOMER,NOPARAMETER,0,NULL},    // JOB_DC_START		'C'
  {'M','M',CUSTOMER,NOPARAMETER,0,NULL},    // JOB_BE_MASTER		'M'
  {'M','m',CUSTOMER,NOPARAMETER,0,NULL},    // JOB_RELEASE_MASTER	'm'
  {'M','T',CUSTOMER,NOPARAMETER,0,NULL},    // JOB_BE_TUNNEL		'T'
  {'M','t',CUSTOMER,NOPARAMETER,0,NULL},    // JOB_RELEASE_TUNNEL	't'

};


#define NUM_INFORMATION 5

INFORMATION cnetInformation[NUM_INFORMATION]=
{
  {klingelNode,'L','1','s',UINT_8,1,(void*)&iLichtKleinStatus,setStatusLichtKlein},
  {klingelNode,'C','1','t',FLOAT,1,(void*)&fHelligkeit,NULL},
  {klingelNode,'C','g','g',UINT_16,1,(void*)&iLichtgrenzwert,NULL},
  {klingelNode,'C','h','g',UINT_16,1,(void*)&iLichtwertHysterese,NULL},
  {klingelNode,'D','0','S',UINT_8,1,(void*)&Zustand_AutoDoor,NULL}
};

ComReceiver cnetCom(&cmulti,Node, cnetCommands,NUM_COMMANDS, cnetInformation,NUM_INFORMATION);

void setStatusLichtKlein()
{
  switch (iLichtKleinStatus)
  {
    case STATUS_LICHT_KLEIN_AUS:
      set_led_color(F_GRUEN,20,LED_LICHT_KLEIN);
      LICHT_KLEIN_OFF;
    break;
    case STATUS_LICHT_KLEIN_AN:
      set_led_color(F_ROT,20,LED_LICHT_KLEIN);
      LICHT_KLEIN_ON;
    break;
    case STATUS_LICHT_KLEIN_AUTO:
      set_led_color(F_BLAU,20,LED_LICHT_KLEIN);
///////////////////////////////////////////////////////
      //if(illu.Get_Data()<SCHALTSCHWELLE_LICHT_KLEIN)
      //  LICHT_KLEIN_ON;
    break;
  }
}

void jobDoorIsOpen(ComReceiver *comRec, char function,char address,char job, void * pMem)
{
  Zustand_Eingabe = INPUT_IO;
  MyTimers[TIMER_BLOCKADE].value= TIME_OFF_OPEN_DOOR;
  MyTimers[TIMER_BLOCKADE].state = TM_START;
  Zustand_AutoDoor = true;
}

void setBootloaderAttention(ComReceiver *comRec, char function,char address,char job, void * pMem)
{
	if (strcmp((char *)pMem,BOOTLOADER_ATTENTION_KEY)==0)
    bootloader_attention=true;
  else
    bootloader_attention=false;
}

void jobGetBlockingStatus(ComReceiver *comRec, char function,char address,char job, void * pMem)
{
  BlockingStatus = ( (uint8_t *)pMem )[0];
  if(BlockingStatus>0)
  {
    Zustand_Eingabe = INPUT_BLOCKADE;
    MyTimers[TIMER_BLOCKADE].value = BlockadeZeiten[BlockingStatus];
    MyTimers[TIMER_BLOCKADE].state = TM_RESET;
  }
  else
  {
    Zustand_Eingabe = INPUT_IO;
    MyTimers[TIMER_BLOCKADE].value= TIME_OFF_OPEN_DOOR;
    MyTimers[TIMER_BLOCKADE].state = TM_START;
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

void startBootloader(ComReceiver *comRec, char function,char address,char job, void * pMem)
{
  comRec->sendAnswer("GotoBootloader",function,address,job,true);
  _delay_ms(600);
  bootloader();
}

void jobSetSecurityKey(ComReceiver *comRec, char function,char address,char job, void * pMem)
{
uint8_t ret = true;
	if (strcmp((char *)pMem,SECURITY_LEVEL_PRODUCTION_KEY)==0)
	{
    comRec->SetSecurityLevel(PRODUCTION);
	}
	else if(strcmp((char *)pMem,SECURITY_LEVEL_DEVELOPMENT_KEY)==0)
	{
    comRec->SetSecurityLevel(DEVELOPMENT);
	}
	else
	{
    comRec->SetSecurityLevel(CUSTOMER);
		ret = false;
	}
	comRec->sendAnswerInt(function,address,job,comRec->GetSecurityLevel(),ret);
}

void jobGetSecurityKey(ComReceiver *comRec, char function,char address,char job, void * pMem)
{
	comRec->sendAnswerInt(function,address,job,comRec->GetSecurityLevel(),true);
}


void jobGetCompilationDate(ComReceiver *comRec, char function,char address,char job, void * pMem)
{
char temp[20];
	strcpy(temp,Compilation_Date);
	comRec->sendAnswer(temp,function,address,job,true);
}

void jobGetCompilationTime(ComReceiver *comRec, char function,char address,char job, void * pMem)
{
char temp[20];
	strcpy(temp,Compilation_Time);
	comRec->sendAnswer(temp,function,address,job,true);
}

void jobGetFreeMemory(ComReceiver *comRec, char function,char address,char job, void * pMem)
{
extern int __heap_start, *__brkval;
int v;

	uint16_t mem = (uint16_t) &v - (__brkval == 0 ? (uint16_t) &__heap_start : (uint16_t) __brkval);
	comRec->sendAnswerInt(function,address,job,mem,true);
}

uint8_t try_scode(char *code)
{
//	uint8_t random[16];
//	char	inp[51];
	CRC_Calc cc;
//	uint8_t i;
	uint8_t ret;
	uint8_t cardnum;

	WDT_EnableAndSetTimeout(WDT_LONG);
	WDT_Reset();
	ret = false;
	if ( strncmp(SCode[0],Code,4 )==0 )
	{
		blockade_status = 0;
		cardnum = atoi( (char *)(Code+4) );

		ret = make_new_card(cardnum) ;
	} // strncmp
	if ( strncmp(SCode[1],Code,6 )==0 )				/* Zweiten SCode testen -> Dient zum Zuruecksetzen einer Karte */
	{
		blockade_status = 0;
		fill_bar_color(F_BLAU,BR_NOCHANGE);
		refresh_led();
		ret = clear_card();
	}
	if ( strncmp(SCode[2],Code,6 )==0 )				/* Dritten SCode testen -> Dient zum Erstellen einer 0-Karte */
	{
		blockade_status = 0;
		fill_bar_color(F_BLAU,BR_NOCHANGE);
		refresh_led();
		ret = make_new_zero_card();
	}
	if (ret==false)
	{
		WDT_Reset();
		Zustand_Eingabe = INPUT_BLOCKADE;
		MyTimers[TIMER_BLOCKADE].value= 3000;
		MyTimers[TIMER_BLOCKADE].state = TM_START;
	}
	else
	{
		blockade_status = 0;
		fill_bar_color(F_LILA,BR_NOCHANGE);
		refresh_led();
		WDT_Reset();
		_delay_ms(2000);
		WDT_Reset();
	}
	WDT_Reset();
	WDT_EnableAndSetTimeout(WDT_SHORT);
	return(ret);
}

void make_blocking()
{
	Zustand_Eingabe = INPUT_BLOCKADE;
	MyTimers[TIMER_BLOCKADE].value = Blockade_Zeiten[blockade_status];
	MyTimers[TIMER_BLOCKADE].state = TM_RESET;
	blockade_status+=1;
	if(blockade_status == BLOCKADE_NUM)
		blockade_status--;
}

uint8_t make_new_card(uint8_t cardnum)
{
uint8_t ret = false, temp;
	// output_print(KNET,"\nKartennummer durch Taste: %d",cardnum);
	WDT_Reset();
	if (cardnum<KEY_NUM)
	{
		fill_bar_color(F_ORANGE,BR_NOCHANGE);
		set_led_color(F_BLAU,BR_NOCHANGE,0);
		refresh_led();
		MyTimers[TIMER_TIMEOUT].value = 1000;
		MyTimers[TIMER_TIMEOUT].state = TM_START;
		do
		{
			_delay_ms(1);
			temp = selectCard(false);
		} while (  (MyTimers[TIMER_TIMEOUT].state!=TM_STOP) && (temp==false) );
		if (temp==false)
		{
			fill_bar_color(F_GRUEN,BR_MAX);
			refresh_led();
		}
		WDT_Reset();
		cmulti.sendCommand(klingelNode,'R','1','N'); //send_command(KNET, "ZRN");
		if (temp)
		{
			//				MFRC522_Init();
			cmulti.sendInfo("Pruefe Karte.",klingelNode); //output_str(KNET,"Prüfe Karte.\n\r");
			if (try_card(CARD_SPECIAL))
			{
				cmulti.sendInfo("Karte erfolgreich geprueft.",klingelNode); //output_str(KNET,"Karte erfolgreich geprüft.\n\r");
				Zustand_Eingabe = INPUT_IO;
				MyTimers[TIMER_TIMEOUT].value = 2000;
				MyTimers[TIMER_TIMEOUT].state = TM_START;
				set_led_color(F_BLAU,BR_NOCHANGE,1);
				refresh_led();
				WDT_Reset();
				do
				{
					_delay_ms(1);
					MFRC522_Init();
				} while ( selectCard(false) );
				WDT_Reset();
				cmulti.sendInfo("Karte entfernt.",klingelNode); //output_str(KNET,"Karte entfernt.\n\r");
				cmulti.sendCommand(klingelNode,'R','1','N');  //send_command(KNET, "ZRN");
				set_led_color(F_BLAU,BR_NOCHANGE,2);
				refresh_led();
				MyTimers[TIMER_TIMEOUT].value = 2000;
				MyTimers[TIMER_TIMEOUT].state = TM_START;
				MFRC522_Init();
				WDT_Reset();
				do
				{
					_delay_ms(1);
					temp = selectCard(false);
				} while (  (MyTimers[TIMER_TIMEOUT].state!=TM_STOP) && (temp==false) );
				WDT_Reset();
				cmulti.sendCommand(klingelNode,'R','1','N'); //send_command(KNET, "ZRN");
				WDT_Reset();
				if (temp)
				{
					//						MFRC522_Init();
					ret = prog_card(cardnum);
					set_led_color(F_BLAU,BR_NOCHANGE,3);
					refresh_led();
				}
				else
					ret = false;
				WDT_Reset();
			}
			else
			{
				ret= false;
			}

		}
	} // cardnum < KEYNUM
	else
	{
		ret= false;
	}
	return(ret);
}

uint8_t make_new_zero_card()
{
uint8_t ret = false, temp;

	// output_print(KNET,"\nErstelle neue Zero-Card");
	fill_bar_color(F_ORANGE,BR_NOCHANGE);
	set_led_color(F_BLAU,BR_NOCHANGE,0);
	refresh_led();
	cmulti.sendCommand(klingelNode,'R','1','N'); //send_command(KNET, "ZRN");
	set_led_color(F_BLAU,BR_NOCHANGE,1);
	refresh_led();
	MyTimers[TIMER_TIMEOUT].value = 2000;
	MyTimers[TIMER_TIMEOUT].state = TM_START;
	MFRC522_Init();
	WDT_Reset();
	do
	{
		_delay_ms(1);
		temp = selectCard(false);
	} while (  (MyTimers[TIMER_TIMEOUT].state!=TM_STOP) && (temp==false) );
	WDT_Reset();
	cmulti.sendCommand(klingelNode,'R','1','N'); //send_command(KNET, "ZRN");
	WDT_Reset();
	if (temp)
	{
		//						MFRC522_Init();
		ret = prog_card(0);
		set_led_color(F_BLAU,BR_NOCHANGE,3);
		refresh_led();
	}
	else
		ret = false;

	return(ret);
}

uint8_t clear_card()
{
uint8_t cardnum,ret = false,temp;
uint8_t info[INFO_LENGTH+1];		// plus eins wegen Kartennummer
uint8_t ckey[KEY_LENGTH];
uint8_t random[16];
char	inp[51];
char	text[10];
CRC_Calc cc;
uint8_t i;


	MyTimers[TIMER_TIMEOUT].value = 2000;
	MyTimers[TIMER_TIMEOUT].state = TM_START;
	do
	{
		_delay_ms(1);
		temp = selectCard(false);
	} while (  (MyTimers[TIMER_TIMEOUT].state!=TM_STOP) && (temp==false) );
	if (temp == true)
	{
		cardnum = get_card_number();
		if (cardnum>=0 && cardnum < KEY_NUM)
		{
			// Hohle mir die aktuelle Random
			cmulti.sendCommand(klingelNode,'R','1','G'); //send_command(KNET, "ZRG");									// Get Random

			/* €€€€€€€€€€€€
			input_flush(KNET);
			if(input_line(KNET,inp,50,1000))
			{
				if (recode_data(inp[2], inp[3], inp[4], inp+5))
				{
					memcpy(random,inp+5,16);
					// Hohle mir den Key
					// sprintf(text,"ZZK%02d",cardnum);					// Get Key
					cmulti.sendStandardInt(klingelNode,'Z','1','K',cardnum); //send_command( KNET, text );
					input_flush(KNET);
					if(input_line(KNET,inp,50,2000))
					{
						if (recode_data(inp[2], inp[3], inp[4], inp+5))
						{
							for (i=0;i<KEY_LENGTH;i++)
							{
								ckey[i] = (uint8_t)(inp[5+i]) ^ random[i];
							}
							info[0] = cardnum;

							//sprintf(text,"ZZG%02d",cardnum);			// Get Info
							cmulti.sendStandardInt(klingelNode,'Z','1','G',cardnum); //send_command( KNET, text );
							input_flush(KNET);
							if(input_line(KNET,inp,50,2000))
							{
								if (recode_data(inp[2], inp[3], inp[4], inp+5))
								{
									for (i=0;i<INFO_LENGTH;i++)
									{
										info[i] = 0xff; // alle Daten loeschen (uint8_t)(inp[5+i]) ^ random[i];
									}
									if( rc522_write_key(SECTOR_INFO, ckey , defaultKeyA) )
									{
										cmulti.sendInfo("Schlüssel geschrieben.",klingelNode); //output_str(KNET,"\nSchlüssel geschrieben\n");
										if ( rc522_write_block(SECTOR_INFO,SUB_INFO,defaultKeyA,(char*)info,INFO_LENGTH) ) // Info in Sub 0
										{
											cmulti.sendInfo("Info geschrieben.",klingelNode); //output_str(KNET,"Info geschrieben\n");
											info[0] = 0xff; // cardnum loeschen
											if ( rc522_write_block(SECTOR_CARDNUM,SUB_CARDNUM,defaultKeyA,(char*)info,1) )	// Kartennummer in Sub 1
											{
												cmulti.sendInfo("Nummer geschrieben.",klingelNode); //output_str(KNET,"Nummer geschrieben\n");
												MFRC522_Init();
												ret=true;
											}
										}
									}
								}
							}
						}
					}
				}
			}
		  */
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
		if (cardnum>=KEY_NUM)
			cardnum = 0;
		return( cardnum );
	}
	else
		return( 255 );
#warning "************************** Achtung Rueckgabe noch nicht getestet"
}

uint8_t try_card(uint8_t cardnum)
{
uint8_t info[INFO_LENGTH+1];		// plus eins wegen Kartennummer
uint8_t ckey[KEY_LENGTH];
uint8_t random[16];
uint8_t ret = false;
char	inp[51];
char	text[10];

CRC_Calc cc;
uint8_t i;
//uint8_t temp;

	if (cardnum==CARD_STANDARD)
	{
		cardnum = 0;
		if(rc522_read_block(SECTOR_CARDNUM,SUB_CARDNUM,defaultKeyA,(char*)info,1))
		{
			cardnum = (uint8_t)info[0];
			if (cardnum>=KEY_NUM)
				cardnum = 0;
		}
	}
	else
		cardnum = 0;
	//output_print(KNET,"Kartennummer: %d\n\r",cardnum);
	cmulti.sendCommand(klingelNode,'R','1','G'); //send_command(KNET, "ZRG");  // Hole mir die aktuelle Random


	/* €€€€€€€€€€€eee
	input_flush(KNET);
	if(input_line(KNET,inp,50,1000))
	{
		if (recode_data(inp[2], inp[3], inp[4], inp+5))
		{
			memcpy(random,inp+5,16);
		// Hohle mir den Key
			//sprintf(text,"ZZK%02d",cardnum);
			cmulti.sendStandardInt(klingelNode,'Z','1','K',cardnum); //send_command( KNET, text );
			input_flush(KNET);
			if(input_line(KNET,inp,50,2000))
			{
				if (recode_data(inp[2], inp[3], inp[4], inp+5))
				{
					for (i=0;i<KEY_LENGTH;i++)
					{
						ckey[i] = (uint8_t)(inp[5+i]) ^ random[i];
					}
					info[0] = cardnum;
					if(rc522_read_block(SECTOR_INFO,SUB_INFO,ckey,(char*)info+1,INFO_LENGTH))
					{
						MFRC522_Init();
						encrypt.Output_Encrypt_Code(KNET,'Z','Z','I',random,info,INFO_LENGTH+1);
						if(test_answer(KNET))
							ret = true;
					}
				}
			}
		}
	}

	*/
	return(ret);
}

uint8_t prog_card(uint8_t cardnum)
{
	uint8_t info[INFO_LENGTH+1];		// plus eins wegen Kartennummer
	uint8_t ckey[KEY_LENGTH];
	uint8_t random[16];
	uint8_t ret = false;
	char	inp[51];
	char	text[10];

	CRC_Calc cc;
	uint8_t i;
//	uint8_t temp;

	// Hohle mir die aktuelle Random
	cmulti.sendCommand(klingelNode,'R','1','G'); //send_command(KNET, "ZRG");									// Get Random

	/* €€€€€€€€€€€€e
	input_flush(KNET);
	if(input_line(KNET,inp,50,1000))
	{
		if (recode_data(inp[2], inp[3], inp[4], inp+5))
		{
			memcpy(random,inp+5,16);
			// Hohle mir den Key
			//sprintf(text,"ZZK%02d",cardnum);					// Get Key
			cmulti.sendStandardInt(klingelNode,'Z','1','K',cardnum); //send_command( KNET, text );



			input_flush(KNET);
			if(input_line(KNET,inp,50,2000))
			{
				if (recode_data(inp[2], inp[3], inp[4], inp+5))
				{
					for (i=0;i<KEY_LENGTH;i++)
					{
						ckey[i] = (uint8_t)(inp[5+i]) ^ random[i];
					}
					info[0] = cardnum;

					// sprintf(text,"ZZG%02d",cardnum);			// Get Info
					cmulti.sendStandardInt(klingelNode,'Z','1','G',cardnum); //send_command( KNET, text );
					input_flush(KNET);
					if(input_line(KNET,inp,50,2000))
					{
						if (recode_data(inp[2], inp[3], inp[4], inp+5))
						{
							for (i=0;i<INFO_LENGTH;i++)
							{
								info[i] = (uint8_t)(inp[5+i]) ^ random[i];
							}
							if( rc522_write_key(SECTOR_INFO, defaultKeyA, ckey) )
							{
								cmulti.sendInfo("Schlüssel geschrieben.",klingelNode); //output_str(KNET,"Schlüssel geschrieben\n");
								if ( rc522_write_block(SECTOR_INFO,SUB_INFO,ckey,(char*)info,INFO_LENGTH) ) // Info in Sub 0
								{
									cmulti.sendInfo("Info geschrieben.",klingelNode); //output_str(KNET,"Info geschrieben\n");
									info[0] = cardnum;
									if ( rc522_write_block(SECTOR_CARDNUM,SUB_CARDNUM,defaultKeyA,(char*)info,1) )	// Kartennummer in Sub 1
									{
										cmulti.sendInfo("Nummer geschrieben.",klingelNode); //output_str(KNET,"Nummer geschrieben\n");
										MFRC522_Init();
										ret=true;
									}
								}
							}
						}
					}
				}
			}

		}
	}*/
	return(ret);
}


uint8_t getAddress(char adr)
{
  if(adr== '1')
    return(1);
  else
    return(0);
}
