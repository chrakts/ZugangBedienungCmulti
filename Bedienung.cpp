/*
 * test01.cpp
 *
 * Created: 15.11.2015 17:39:41
 *  Author: Christof
 */

#include "Bedienung.h"

int main(void)
{
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
					// blockade_status = 0; muss wieder rein !!!!!!!!!!!!!!
					Zustand_Eingabe = INPUT_IO;
					MyTimers[TIMER_BLOCKADE].value= TIME_OFF_OPEN_DOOR;
					MyTimers[TIMER_BLOCKADE].state = TM_START;
				}
				else
				{
					// make_blocking();   muss wieder rein !!!!!!!!!!!!!!
				}

			}
		}
		if (actuelle_taste!=0)
		{
			char address='1';
			char function = 'T';
			char job='p';
			switch(actuelle_taste)
			{
				case 'A':
          function = 'L';
          job      = 't';
				break;
				case 'B':
          function = 'L';
          job      = 't';
          address  = '2';
				break;
				case '#':
					function = 'P';
					address  = 'H';
				break;
				case '*':
					function = 'P';
					address  = 'S';
				break;
				case 'C':
					function = 'M';
				break;
				case 'D':
					function = 'D';
				break;
				default:
          address  = actuelle_taste;

			}
			cmulti.sendCommand(klingelNode,function,address,job);
      actuelle_taste = 0;
		}
		//set_led_autobright( (uint16_t)fHelligkeit );
		// set_led_autobright(illu.Get_Data());
		refresh_led();

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
