/*
 * Bedienung.h
 *
 * Created: 15.11.2015 20:19:17
 *  Author: Christof
 */
#include <avr/io.h>
#include <stdbool.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "stdio.h"
#include "output.h"

#include "ws2812.h"
#include "RandomTimer.h"
#include "rc522.hpp"
#include "twi_master_driver.h"
#include "tastatur.h"
#include "PIR.h"
#include "Klingel.h"
#include "myTimers.h"
#include "LED.h"
#include "Bed_Functions.h"
#include "watchdog.h"
#include "../Secrets/secrets.h"
#include "Communication.h"
#include "cmultiStandardCommands.h"
#include "xmegaClocks.h"
#include <string.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include "Licht.h"
#include "rfCards.h"
#include "adcMessung.h"

#ifndef TEST01_H_
#define TEST01_H_

#define INFO_NUM 10
#define KEY_NUM	 6
#define KEY_LENGTH 6
#define INFO_LENGTH 6


#define SCODE_LENGTH	6
#define BLOCKADE_NUM	7

#define WDT_SHORT		WDT_PER_1KCLK_gc
#define WDT_LONG		WDT_PER_8KCLK_gc

#define SCHALTSCHWELLE_LICHT_KLEIN 100

#include "External.h"

#define QUARZ_EXISTS

// ------------- PORT A -------------------
#define BEEPER_PIN  PIN1_bm
#define TASTATUR_DA_PIN  PIN2_bm
#define TASTATUR_D_PIN  PIN4_bm
#define TASTATUR_C_PIN  PIN5_bm
#define TASTATUR_B_PIN  PIN6_bm
#define TASTATUR_A_PIN  PIN7_bm

// ------------- PORT B -------------------
#define TEST_PIN			PIN0_bm
#define LED_ROT_PIN			PIN1_bm
#define KLINGEL_TASTER_PIN	PIN2_bm
#define KLINGEL_LED_PIN		PIN3_bm

// ------------- PORT E -------------------
#define RS485_TE_PIN		 PIN0_bm
#define RS485_RE_PIN		 PIN1_bm
#define RS485_RxD_PIN		 PIN2_bm
#define RS485_TxD_PIN		 PIN3_bm

// ------------- PORT C -------------------
#define PIR_PIN				 PIN2_bm
#define BEL_PIN				 PIN3_bm
#define BELEUCHTUNG_PIN		 PIN4_bm
#define NETZ_1_PIN			 PIN5_bm
#define NETZ_2_PIN			 PIN6_bm
#define NETZ_3_PIN			 PIN7_bm

// ------------- PORT D -------------------
#define WANTMASTER_PIN		 PIN0_bm
#define RC522_RESET_PIN		 PIN1_bm
#define RC522_IRQ_PIN		 PIN2_bm
#define LETMASTER_PIN		 PIN3_bm
#define RC522_SDA_PIN		 PIN4_bm
#define RC522_MOSI_PIN		 PIN5_bm
#define RC522_MISO_PIN		 PIN6_bm
#define RC522_SCK_PIN		 PIN7_bm

//#define BEEPER_ON					PORTA_OUTSET = BEEPER_PIN   // hiermit beept der Beeper
#define BEEPER_ON					PORTA_OUTCLR = BEEPER_PIN	// hiermit schweigt er
#define BEEPER_OFF					PORTA_OUTCLR = BEEPER_PIN
#define LED_ROT_ON					PORTB_OUTCLR = LED_ROT_PIN
#define LED_ROT_OFF					PORTB_OUTSET = LED_ROT_PIN
#define LED_KLINGEL_ON				PORTB_OUTSET = KLINGEL_LED_PIN
#define LED_KLINGEL_OFF				PORTB_OUTCLR = KLINGEL_LED_PIN
#define LED_KLINGEL_TOGGLE			PORTB_OUTTGL = KLINGEL_LED_PIN
#define TEST_TOGGLE					PORTB_OUTTGL = TEST_PIN
#define STROM_STOSS_ON				PORTC_OUTCLR = NETZ_1_PIN
#define STROM_STOSS_OFF				PORTC_OUTSET = NETZ_1_PIN
#define LICHT_KLEIN_ON				PORTC_OUTCLR = NETZ_3_PIN
#define LICHT_KLEIN_OFF				PORTC_OUTSET = NETZ_3_PIN

#define LETMASTER					PORTD_OUTSET = LETMASTER_PIN
#define RELEASEMASTER				PORTD_OUTCLR = LETMASTER_PIN

#define TIME_OFF_OPEN_DOOR			500
#define TIME_OFF_BLOCKADE			1000

#define KNET 1

enum {STATUS_LICHT_KLEIN_AUS,STATUS_LICHT_KLEIN_AN,STATUS_LICHT_KLEIN_AUTO};

enum {INPUT_WAIT,INPUT_C2,INPUT_C3,INPUT_C4,INPUT_CODE_READY,INPUT_CODE_WAIT_RANDOM,INPUT_S1,INPUT_S2,INPUT_S3,INPUT_S4,INPUT_S5,INPUT_S6,INPUT_SCODE_READY,INPUT_SCODE_WAIT_RANDOM,INPUT_BLOCKADE,INPUT_IO};

enum{UNBLOCKED=0,BLOCKED0,BLOCKED1,BLOCKED2,BLOCKED3,BLOCKED4,BLOCKED5,BLOCKED_LAST};

extern USART_t *mein_serial;

void init_io();
void doReport();

#endif /* TEST01_H_ */
