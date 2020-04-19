/*
 * test01.cpp
 *
 * Created: 15.11.2015 17:39:41
 *  Author: Christof
 */

#include "Bedienung.h"

int main(void)
{
//static uint8_t MyData[NUMLEDS*3];
PORT_t *myp;
uint8_t i;
	init_clock(SYSCLK,PLL,true,CLOCK_CALIBRATION);
	WDT_Disable();
	init_io();

  cmulti.open(Serial::BAUD_19200,F_CPU);

	init_tastatur();
	init_PIR();
	init_klingel();
	init_mytimer();

	PMIC_CTRL = PMIC_LOLVLEX_bm | PMIC_HILVLEN_bm | PMIC_MEDLVLEN_bm;
	sei();

	_delay_ms(11);
	BEEPER_OFF;

	WS_init();
	myp = &PORTC;
	myp->REMAP = 1;

	fill_led_color(F_WEISS,0);
	refresh_led();
	for (i=0;i<=BR_MAX;i++)
	{
		fill_led_brightness(i);
		refresh_led();
		_delay_ms(20);
	}
	fill_led_color(F_ROT,BR_MAX);
	for (i=BR_MAX;i>0;i--)
	{
		fill_led_brightness(i);
		refresh_led();
		_delay_ms(20);
	}
	MyTimers[TIMER_SLEEP].state = TM_RESET;

//	test_aes();
	fill_led_color(F_BLACK,BR_MAX);
	rc522_setup();
	AntennaOn();
	WDT_EnableAndSetTimeout(WDT_SHORT);
	WDT_Reset();
	//output(KNET,'E');		// Beendet den Bootloader von "Bedienung"
	do
	{
		WDT_Reset();
		if (PIR_Trigger!=0)
		{
			//output_str(KNET,"\\>ZRN<\\");
			cmulti.sendCommand(klingelNode,'P','1','t');
			PIR_Trigger = 0;
		}
		if (Klingel_Trigger!=0)
		{
			//output_str(KNET,"\\>ZZB<\\");
			cmulti.sendCommand(klingelNode,'K','1','r');
			Klingel_Trigger = 0;
			PIR_Trigger = 1;
			_delay_ms(500);
			PORTC_OUTCLR = BEL_PIN;
		}
/*		rc522_loop();*/
		if ( (Zustand_Eingabe!=INPUT_BLOCKADE) && (Zustand_Eingabe!=INPUT_IO) )
		{
			if (selectCard(false))
			{
				if (try_card(CARD_STANDARD))
				{
					blockade_status = 0;
					Zustand_Eingabe = INPUT_IO;
					MyTimers[TIMER_BLOCKADE].value= TIME_OFF_OPEN_DOOR;
					MyTimers[TIMER_BLOCKADE].state = TM_START;
				}
				else
				{
					make_blocking();
				}

			}
		}
		if (actuelle_taste!=0)
		{
			//output_str(KNET,"\\>ZRN<\\");
			cmulti.sendCommand(klingelNode,'R','1','N');
			switch(actuelle_taste)
			{
				case 0:
				break;
				case 'A':
					MyTimers[TIMER_SLEEP].state = TM_RESET;
					set_led_color(F_ROT,20,LED_LICHT_GROSS);
					STROM_STOSS_ON;
					MyTimers[TIMER_LICHT_GROSS].state = TM_START;
				break;
				case 'B':
					MyTimers[TIMER_SLEEP].state = TM_RESET;
					nextStatusLichtKlein();

				break;
				case '#':
					MyTimers[TIMER_SLEEP].state = TM_RESET;
					Zustand_Eingabe = INPUT_WAIT;
					cmulti.sendCommand(klingelNode,'Z','1','a'); //send_command(KNET, "ZZa");
					Zustand_AutoDoor = false;
				break;
				case '*':
					cmulti.sendCommand(klingelNode,'Z','1','a'); //send_command(KNET, "ZZa");
					Zustand_AutoDoor = false;
					MyTimers[TIMER_SLEEP].state = TM_RESET;
					Zustand_Eingabe = INPUT_S1;
					MyTimers[TIMER_TIMEOUT].value = 1000;
					MyTimers[TIMER_TIMEOUT].state = TM_START;
				break;
				case 'C':
					MyTimers[TIMER_SLEEP].state = TM_RESET;
				break;
				case 'D':
					MyTimers[TIMER_SLEEP].state = TM_RESET;
					cmulti.sendCommand(klingelNode,'D','1','A');
				break;
				default:
					MyTimers[TIMER_SLEEP].state = TM_RESET;
					switch(Zustand_Eingabe)
					{
						case INPUT_WAIT:
							Zustand_AutoDoor = false;
							cmulti.sendCommand(klingelNode,'Z','1','a'); //send_command(KNET, "ZZa");
							Code[0]=actuelle_taste;
							Zustand_Eingabe = INPUT_C2;
							MyTimers[TIMER_TIMEOUT].value = 1000;
							MyTimers[TIMER_TIMEOUT].state = TM_START;
						break;
						case INPUT_C2:
							MyTimers[TIMER_TIMEOUT].state = TM_RESET;
							Code[1]=actuelle_taste;
							Zustand_Eingabe = INPUT_C3;
						break;
						case INPUT_C3:
							MyTimers[TIMER_TIMEOUT].state = TM_RESET;
							Code[2]=actuelle_taste;
							Zustand_Eingabe = INPUT_C4;
						break;
						case INPUT_C4:
							MyTimers[TIMER_TIMEOUT].state = TM_RESET;
							Code[3]=actuelle_taste;
							Code[4]= 0;
							Zustand_Eingabe = INPUT_CODE_READY;
						break;
						case INPUT_S1:
							MyTimers[TIMER_TIMEOUT].state = TM_RESET;
							Code[0]=actuelle_taste;
							Zustand_Eingabe = INPUT_S2;
						break;
						case INPUT_S2:
							MyTimers[TIMER_TIMEOUT].state = TM_RESET;
							Code[1]=actuelle_taste;
							Zustand_Eingabe = INPUT_S3;
						break;
						case INPUT_S3:
							MyTimers[TIMER_TIMEOUT].state = TM_RESET;
							Code[2]=actuelle_taste;
							Zustand_Eingabe = INPUT_S4;
						break;
						case INPUT_S4:
							MyTimers[TIMER_TIMEOUT].state = TM_RESET;
							Code[3]=actuelle_taste;
							Zustand_Eingabe = INPUT_S5;
						break;
						case INPUT_S5:
							MyTimers[TIMER_TIMEOUT].state = TM_RESET;
							Code[4]=actuelle_taste;
							Zustand_Eingabe = INPUT_S6;
						break;
						case INPUT_S6:
							MyTimers[TIMER_TIMEOUT].state = TM_RESET;
							Code[5]=actuelle_taste;
							Code[6]= 0;
							Zustand_Eingabe = INPUT_SCODE_READY;
						break;

					}
				break;
			}
		}
		actuelle_taste = 0;
		if (Zustand_AutoDoor==false)
		{
			set_led_color(F_ROT,BR_NOCHANGE,6);
		}
		else
		{
			set_led_color(F_BLAU,BR_NOCHANGE,6);
		}
		switch(Zustand_Eingabe)
		{
			case INPUT_WAIT:
				fill_bar_color(F_WEISS,BR_NOCHANGE);
				MyTimers[TIMER_TIMEOUT].state = TM_STOP;
			break;
			case INPUT_C2:
				set_led_color(F_BLAU,BR_NOCHANGE,1);
			break;
			case INPUT_C3:
				set_led_color(F_BLAU,BR_NOCHANGE,2);
			break;
			case INPUT_C4:
				set_led_color(F_BLAU,BR_NOCHANGE,3);
			break;
			case INPUT_CODE_READY:
				set_led_color(F_BLAU,BR_NOCHANGE,4);
			break;
			case INPUT_S1:
				fill_bar_color(F_LILA,BR_NOCHANGE);
			break;
			case INPUT_S2:
				set_led_color(F_BLAU,BR_NOCHANGE,0);
			break;
			case INPUT_S3:
				set_led_color(F_BLAU,BR_NOCHANGE,1);
			break;
			case INPUT_S4:
				set_led_color(F_BLAU,BR_NOCHANGE,2);
			break;
			case INPUT_S5:
				set_led_color(F_BLAU,BR_NOCHANGE,3);
			break;
			case INPUT_S6:
				set_led_color(F_BLAU,BR_NOCHANGE,4);
			break;
			case INPUT_SCODE_READY:
				set_led_color(F_BLAU,BR_NOCHANGE,5);
			break;
			case INPUT_BLOCKADE:
				fill_bar_color(F_ROT,BR_MAX);
			break;
			case INPUT_IO:
				fill_bar_color(F_GRUEN,BR_MAX);
			break;
		}
		set_led_autobright( (uint16_t)fHelligkeit );
		// set_led_autobright(illu.Get_Data());
		refresh_led();
		if( (Zustand_Eingabe != INPUT_BLOCKADE) && (MyTimers[TIMER_TIMEOUT].state == TM_STOP) )
			Zustand_Eingabe = INPUT_WAIT;

		switch(Zustand_Eingabe)
		{
			case INPUT_CODE_READY:
        Valid_Random[0] = false;
        cmulti.sendCommand(klingelNode,'R','0','G'); //send_command(KNET, "ZRG");
        Zustand_Eingabe = INPUT_CODE_WAIT_RANDOM;
			break;
			case INPUT_CODE_WAIT_RANDOM:
        if(Valid_Random[0] == true)
        {
          cmulti.setEncryption(Actual_Random[0]);
          cmulti.sendStandardByteArray((uint8_t *)Code,4,klingelNode,'C','1','t','T');
          Valid_Random[0] = false;
          Zustand_Eingabe = INPUT_BLOCKADE; // wird sicherheitshalber gesetzt -> wird aufgehoben, falls Code ok
        }
			break;
			case INPUT_SCODE_READY:
				if (try_scode(Code))
				{
					//output_str(KNET,"SCode ok.\n");
					cmulti.sendInfo("SCode ok.",klingelNode);
					Zustand_Eingabe = INPUT_WAIT;
				}
				else
				{
					//output_str(KNET,"Timeout.\n");
					cmulti.sendInfo("Timeout.",klingelNode);
				}
			break;
			case INPUT_SCODE_WAIT_RANDOM:
        if(Valid_Random[0] == true)
        {
          cmulti.setEncryption(Actual_Random[0]);
          cmulti.sendStandardByteArray((uint8_t *)Code,6,klingelNode,'C','1','t','T');
          Valid_Random[0] = false;
          Zustand_Eingabe = INPUT_BLOCKADE; // wird sicherheitshalber gesetzt -> wird aufgehoben, falls Code ok
        }
			break;
		}
//		LED_KLINGEL_ON;
		if (do_sleep)
		{
			if( iLichtKleinStatus == STATUS_LICHT_KLEIN_AUTO )
				LICHT_KLEIN_OFF;
			WDT_Disable();
			MyTimers[TIMER_SLEEP].state = TM_STOP;
			fill_bar_color(F_ORANGE,5);
			refresh_led();
			_delay_ms(2);
			LED_KLINGEL_OFF;
			LED_ROT_OFF;
			set_sleep_mode(SLEEP_MODE_PWR_DOWN);
			sleep_enable();
			sleep_cpu();
			sleep_disable();
			MyTimers[TIMER_SLEEP].state = TM_START;
			do_sleep = 0;
			WDT_EnableAndSetTimeout(WDT_SHORT);
			WDT_Reset();
			if (iLichtKleinStatus == STATUS_LICHT_KLEIN_AUTO)
			{
        ///////////////////////////////////////////////////////////////////////////
				//if(illu.Get_Data()<SCHALTSCHWELLE_LICHT_KLEIN)
				//	LICHT_KLEIN_ON;
			}
		}
		WDT_Reset();
	} while (1);
}

void init_io()
{
	PORTA_DIRSET = BEEPER_PIN;
	PORTA_DIRCLR = TASTATUR_DA_PIN | TASTATUR_D_PIN | TASTATUR_C_PIN | TASTATUR_B_PIN | TASTATUR_A_PIN;
	PORTB_DIRSET = LED_ROT_PIN | KLINGEL_LED_PIN | TEST_PIN;
	PORTB_DIRCLR = KLINGEL_TASTER_PIN;
	PORTC_DIRSET = BELEUCHTUNG_PIN | BEL_PIN | NETZ_1_PIN | NETZ_2_PIN | NETZ_3_PIN;
	PORTC_DIRCLR = PIR_PIN;
	PORTD_DIRSET = RC522_RESET_PIN | RC522_SDA_PIN | RC522_MOSI_PIN | RC522_SCK_PIN | LETMASTER_PIN;
	PORTD_DIRCLR = WANTMASTER_PIN | RC522_IRQ_PIN | RC522_MISO_PIN;
//	PORTD_DIRSET = RC522_RESET_PIN | RC522_SDA_PIN | RC522_MOSI_PIN | RC522_SCK_PIN;
//	PORTD_DIRCLR = RC522_IRQ_PIN | RC522_MISO_PIN;
	PORTE_DIRSET = RS485_TE_PIN | RS485_RE_PIN | RS485_TxD_PIN;
	PORTE_DIRCLR = RS485_RxD_PIN;

	PORTA_OUTSET = BEEPER_PIN;
	PORTB_OUTSET = LED_ROT_PIN;
	PORTB_OUTCLR = KLINGEL_LED_PIN;
	PORTC_OUTSET = BELEUCHTUNG_PIN | NETZ_1_PIN | NETZ_2_PIN | NETZ_3_PIN;
	PORTC_OUTCLR = BEL_PIN;
	PORTC_PIN4CTRL = PORT_INVEN_bm | PORT_SRLEN_bm;
	PORTD_OUTSET = RC522_SDA_PIN | RC522_MOSI_PIN | RC522_SCK_PIN;
	PORTD_OUTCLR = RC522_RESET_PIN | LETMASTER_PIN | RC522_MISO_PIN;
	PORTE_OUTCLR = RS485_TE_PIN | RS485_RE_PIN | RS485_TxD_PIN;
	PORTE_PIN2CTRL = PORT_OPC_PULLUP_gc;
}
