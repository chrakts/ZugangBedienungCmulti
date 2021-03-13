/*
 * test01.cpp
 *
 * Created: 15.11.2015 17:39:41
 *  Author: Christof
 */

#include "Bedienung.h"
//#undef USE_RF

int main(void)
{
uint8_t i;
uint16_t bremse = 0 ;
	WDT_Disable();
	init_clock(SYSCLK,PLL,true,CLOCK_CALIBRATION);
	init_io();

  cmulti.open(Serial::BAUD_57600,F_CPU);

#ifdef USE_TASTATUR
	init_tastatur();
#endif // USE_TASTATUR

#ifdef USE_PIR
	init_PIR();
#endif // USE_PIR

	init_klingel();
	init_mytimer();
  //
  LED_KLINGEL_ON;

	PMIC_CTRL = PMIC_LOLVLEX_bm | PMIC_HILVLEN_bm | PMIC_MEDLVLEN_bm;
	sei();

  cmulti.broadcastUInt8((uint8_t) RST.STATUS,'S','0','R');
	_delay_ms(11);
	BEEPER_OFF;
	WS_init();
  refresh_led_new();
	cmulti.sendInfo("Hallo","Bd");

	for (i=0;i<=20;i++)
	{
    uint8_t temp = sLEDStatus[NUMLEDS-1];
	  for(int8_t j=NUMLEDS-2;j>=0;j--)
    {
      sLEDStatus[j+1] = sLEDStatus[j];
    }
	  sLEDStatus[0] = temp;
    refresh_led_new();
		_delay_ms(100);
	}

	#ifdef USE_RF
	  rc522_setup();
	  AntennaOn();
	#endif // USE_RF

	WDT_EnableAndSetTimeout(WDT_LONG);
	WDT_Reset();
	do
	{
		WDT_Reset();
    cnetCom.comStateMachine();
    cnetCom.doJob();
    if(lichtKleinStatus==0)
      LICHT_KLEIN_OFF;
    else
      LICHT_KLEIN_ON;
		if (PIR_Trigger!=0)
		{
			cmulti.sendCommand(klingelNode,'P','1','t');
			PIR_Trigger = 0;
		}
		if (Klingel_Trigger!=0)
		{
			cmulti.sendCommand(klingelNode,'K','1','r');
			Klingel_Trigger = 0;
			PIR_Trigger = 1;
		}
#ifdef USE_RF     /*		rc522_loop();*/
    bremse++;
    if( bremse>30000 )
    {
      bremse=0;
      switch(cardStatus)
      {
        case CARD_UNSELECTED:
          if (selectCard(false))
          {
            //cmulti.sendCommand(klingelNode,'C',+ '0' ,'t');
            uint8_t cardNum = get_card_number();
            if( cardNum < KEY_NUM )
            {
              cmulti.sendCommand(klingelNode,'C',+ '0'+cardNum ,'f');
            }
            cardStatus=CARD_SELECTED;
            MyTimers[TIMER_CARDSTATUS].state = TM_START;
          }
        break;
        case CARD_WAITING_FOR_WRITE:
          MyTimers[TIMER_CARDSTATUS].state = TM_START;
          if (selectCard(false))
          {
            cardStatus=CARD_SELECTED;
            writeNewCard(cardNumber,cardKey,cardInfo);
            MyTimers[TIMER_CARDSTATUS].state = TM_START;
          }
        break;
        case CARD_WAITING_FOR_CLEAR:
          //to-do:
          MyTimers[TIMER_CARDSTATUS].state = TM_START;
          if (selectCard(false))
          {
            cardStatus=CARD_SELECTED;
            cmulti.sendCommand(klingelNode,'C',get_card_number()+'0','s');
            MyTimers[TIMER_CARDSTATUS].state = TM_START;
          }
        break;
      }
    }
#endif // USE_RF

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
          address  = '2';
          aktiviereStromStoss();
				break;
				case 'B':
          function = 'L';
          job      = 't';
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
		doReport();
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
	//PORTE_PIN2CTRL = PORT_OPC_PULLUP_gc;
}


void doReport()
{
  if(doNextReport)
  {
    toReport++;
    switch(toReport)
    {
      case REPORT_UPTEMPERATURE:
        double temperature;
        temperature = calcTemperatur();
        if(temperature>-40.0)
        {
          cmulti.broadcastFloat(temperature,'C','1','t');
        }
      break;
      case REPORT_LAST:
        toReport = REPORT_FIRST;
      break;
    }
  }
  doNextReport = false;
}
