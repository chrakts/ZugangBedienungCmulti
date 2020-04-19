#ifndef OUTPUT_H_
#define OUTPUT_H_


#include "Bedienung.h"

typedef  char byte;

extern volatile uint8_t UART0_ring_received;
extern volatile char UART0_ring_buffer[];
extern volatile uint8_t UART0_ring_interpreted;

extern volatile uint8_t UART1_ring_received;
extern volatile char UART1_ring_buffer[];
extern volatile uint8_t UART1_ring_interpreted;

void output(uint8_t serial, byte data);
void output_str(uint8_t serial, const char *data);
void output_bin(uint8_t serial, byte x, byte ch0, byte ch1);
void output_hex(uint8_t serial, byte i);
void output_hex16(uint8_t serial, uint16_t i);
void output_dec(uint8_t serial, unsigned int x, unsigned char n, unsigned char fillch);
void output_dec_signed(uint8_t serial, int x, unsigned char n, unsigned char fillch);
void output_print10(uint8_t serial, unsigned long x);
int output_print(uint8_t serial, const char *format, ...);
int input_line( byte file, char *input, byte max_length, int16_t timeout );
char input_char( byte file );			// wartet bis Zeichen eingegeben wird
void input_flush( byte file );
byte get_char(byte file, char *got_byte);
byte get_char_0(char *got_byte);
byte get_char_1(char *got_byte);

#endif /* OUTPUT_H_ */