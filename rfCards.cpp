#include "Bedienung.h"

// versucht die Karte zu selektieren, wenn erfolgreich, wird geschrieben, wenn nicht wird die Funktion sofort verlassen
uint8_t writeNewCard(uint8_t cardnum,uint8_t* keyInfo)
{
  /*
  if(selectCard(false))
  {

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
  */
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
