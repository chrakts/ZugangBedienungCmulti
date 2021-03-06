#ifndef WS2812HARDWARE_H_INCLUDED
#define WS2812HARDWARE_H_INCLUDED

//----- Device specific stuff

//  ATTINY

#if defined WS_ARCH_TINY

#define     WS_OCRA     OCR0A           // OCRxA to use
#define     WS_OCRB     OCR0B           // OCRxB to use
#define     WS_OCFA     OCF0A           // Flag in TIFR for OCRxA
#define     WS_TCCRA    TCCR0A          // TCCRxA to use
#define     WS_TCCRB    TCCR0B          // TCCRxB to use
#define     WS_TCNT     TCNT0           // TCNTx to use
#define     WS_TIFR     TIFR            // TIFR to use ATtiny
#define     WS_OUTPORT  PORTB           // data output port ATtiny
#define     WS_OUTDDR   DDRB            // data output ddr ATtiny
#define     WS_OUTPIN   PB1             // data output pin ATtiny

//  ATMEGA

#elif defined WS_ARCH_MEGA

#define     WS_OCRA     OCR0A           // OCRxA to use
#define     WS_OCRB     OCR0B           // OCRxB to use
#define     WS_OCFA     OCF0A           // Flag in TIFR for OCRxA
#define     WS_TIFR     TIFR0           // TIFR to use ATmega
#define     WS_TCCRA    TCCR0A          // TCCRxA to use
#define     WS_TCCRB    TCCR0B          // TCCRxB to use
#define     WS_TCNT     TCNT0           // TCNTx to use
#define     WS_OUTPORT  PORTD           // data output port ATmega
#define     WS_OUTDDR   DDRD            // data output ddr ATmega
#define     WS_OUTPIN   PD5             // datat output pin ATmega

//  XMEGA

#elif defined WS_ARCH_XMEGA

/*  Voraussetzung bisher:
    Timer 0
    Port und Timer hängen zusammen (Timer C0 -> PORTC)
    Port C: PIN 0/1/2/3
    Port D: PIN 0/1/2/3
    Port E: PIN 0/1/2/3
    Ports C/D können remapped werden so, dass Pin 0->4 / 1->5 / 2->6 / 3->7 wird
    Achtung beim remapping von Pin 0 und 1: Der PWM-Ausgang wird verodert mit dem Timer 1
    getestet: C0,C1,C2,C3,C4,!C5!,C6,C7
              D0,!D1!,!D2!,!D3!,D4,!D5!,!D6!,!D7!
*/
#define     WS_BUFNUM       6              // Output bytes to buffer
#define     WS_BUFSIZE      (WS_BUFNUM*8)   // required buffer size for this

#define     WS_DMA_A		CH0         // DMA channel A to use
#define     WS_DMA_B		CH1         // DMA channel B to use
#define     WS_DMA_DBUFMODE DMA_DBUFMODE_CH01_gc // DMA DBUFMODE to use
#define     WS_DMA_DBUFMODE_bm DMA_DBUFMODE0_bm
#define     WS_DMA_BUSY_bm  (DMA_CH0BUSY_bm|DMA_CH1BUSY_bm)
#define     WS_DMA_INTLVL   0x03        // Interruptlevel for DMA: Original war 0x02
#define     WS_DMA_TRIGSRC  0x01        // TRIGSRC = EVSYS CH0
#define     WS_EVSYS_CH     0           // Eventchannel to use
#define     WS_WGMODE       0x03        // WGMODE_SS

#ifdef PLUG128
  #define     WS_OUTPIN 7         // output pin
#endif // PLUG128
#ifdef BEDIENUNG_V03
  #define     WS_OUTPIN 4         // output pin
#endif // BEDIENUNG_V03

#define     WS_C_PORT
#define     WS_PIN_INVERT

#if WS_OUTPIN == 0
  #define CCx          CCA
  #define CCxBUF       CCABUF
  #define TC0_CCxEN_bm TC0_CCAEN_bm
#elif WS_OUTPIN == 1
  #define CCx    CCB
  #define CCxBUF CCBBUF
  #define TC0_CCxEN_bm TC0_CCBEN_bm
#elif WS_OUTPIN == 2
  #define CCx    CCC
  #define CCxBUF CCCBUF
  #define TC0_CCxEN_bm TC0_CCCEN_bm
#elif WS_OUTPIN == 3
  #define CCx    CCD
  #define CCxBUF CCDBUF
  #define TC0_CCxEN_bm TC0_CCDEN_bm
#elif WS_OUTPIN == 4
  #define CCx    CCA
  #define CCxBUF CCABUF
  #define TC0_CCxEN_bm TC0_CCAEN_bm
  #define TCC_REMAPPING PORT_TC0A_bm
#elif WS_OUTPIN == 5
  #define CCx    CCB
  #define CCxBUF CCBBUF
  #define TC0_CCxEN_bm TC0_CCBEN_bm
  #define TCC_REMAPPING PORT_TC0B_bm
#elif WS_OUTPIN == 6
  #define CCx    CCC
  #define CCxBUF CCCBUF
  #define TC0_CCxEN_bm TC0_CCCEN_bm
  #define TCC_REMAPPING PORT_TC0C_bm
#elif WS_OUTPIN == 7
  #define CCx    CCD
  #define CCxBUF CCDBUF
  #define TC0_CCxEN_bm TC0_CCDEN_bm
  #define TCC_REMAPPING PORT_TC0D_bm
#else
  #error "Ungültige Pinnummer für WS_OUTPIN"
#endif // WS_OUTPIN


#ifdef WS_C_PORT
#define     WS_TCC          TCC0        // Timer/Counter to use
#define     WS_OUTPORT      PORTC       // output port
#define     WS_EVSYS_MUXVAL 0b11000000
#endif // WS_OUTPORT
#ifdef WS_D_PORT
#define     WS_TCC          TCD0        // Timer/Counter to use
#define     WS_OUTPORT      PORTD       // output port
#define     WS_EVSYS_MUXVAL 0b11010000
#endif // WS_OUTPORT
#ifdef WS_E_PORT
#define     WS_TCC          TCE0        // Timer/Counter to use
#define     WS_OUTPORT      PORTE       // output port
#define     WS_EVSYS_MUXVAL 0b11100000
#endif // WS_OUTPORT

#endif // WS_ARCH_XMEGA

#endif // WS2812HARDWARE_H_INCLUDED
