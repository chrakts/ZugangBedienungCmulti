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

#define     WS_BUFNUM       6              // Output bytes to buffer
#define     WS_BUFSIZE      (WS_BUFNUM*8)   // required buffer size for this

#define     WS_DMA_A		CH0         // DMA channel A to use
#define     WS_DMA_B		CH1         // DMA channel B to use
#define     WS_DMA_DBUFMODE DMA_DBUFMODE_CH01_gc // DMA DBUFMODE to use
#define     WS_DMA_DBUFMODE_bm DMA_DBUFMODE0_bm
#define     WS_DMA_BUSY_bm  (DMA_CH0BUSY_bm|DMA_CH1BUSY_bm)
#define     WS_DMA_INTLVL   0x03        // Interruptlevel for DMA: Original war 0x02
#define     WS_TCC          TCC0        // Timer/Counter to use

#define     WS_DMA_TRIGSRC  0x01        // TRIGSRC = EVSYS CH0
#define     WS_EVSYS_CH     0           // Eventchannel to use
#define     WS_EVSYS_MUXVAL 0b11000000  // Event selection == TCC0OVF
#define     WS_WGMODE       0x03        // WGMODE_SS
#define     WS_OUTPORT      PORTC       // output port
#define     WS_OUTPIN       4           // output pin

#endif

#endif // WS2812HARDWARE_H_INCLUDED
