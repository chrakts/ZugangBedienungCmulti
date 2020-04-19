#include "Bedienung.h"

void output(uint8_t serial, byte data)
{
	switch(serial)
	{
		case KNET:
			TransmitByte0( data );
		break;
		default:
			TransmitByte1( data );
		break;
	}
}

/*-----------------27.5.99 20:30--------------------
 *  Display null terminated string
 *--------------------------------------------------*/
void output_str(uint8_t serial, const char *data)
{
    while(*data) {
        output(serial,*data);
        data++;
    }
}

/*-----------------31.8.99 20:29--------------------
 *  Display 8bit bin value
 *   printbin(0xAA,'0','1') -> 10101010
 *   printbin(0xAA,'_','*') -> *_*_*_*_
 *--------------------------------------------------*/
void output_bin(uint8_t serial, byte x, byte ch0, byte ch1)
{
  byte i;
  for (i=128;i>0;i>>=1)
  {
    if ((x&i)==0) output(serial,ch0);
    else output(serial,ch1);
  }
}

/*-----------------1.6.99 16:39---------------------
 *  Display byte in hex (8bit)
 *--------------------------------------------------*/
void output_hex(uint8_t serial, byte i)
{
    byte hi,lo;

    hi=i&0xF0;               // High nibble
    hi=hi>>4;
    hi+='0';
    if (hi>'9')
        hi+=7;

    lo=(i&0x0F)+'0';         // Low nibble
    if (lo>'9')
        lo=lo+7;

    output(serial,hi); output(serial,lo);
}

/*-----------------1.6.99 16:39---------------------
 *  Display byte in hex (8bit)
 *--------------------------------------------------*/
void output_hex16(uint8_t serial, uint16_t i)
{
	output_hex(serial,i>>8);
	output_hex(serial,i&0xff);
}
/*-----------------31.08.99 18:34-------------------
 *   Display 0..65535 with or without leading zeroes
 *     printdec(12345,6,'0') -> 012345
 *     printdec(12345,5,'')  -> 12345
 *     printdec(12345,6,' ') ->  12345
 *--------------------------------------------------*/
void output_dec(uint8_t serial, unsigned int x, unsigned char n, unsigned char fillch)
{
    unsigned char i;
    unsigned char s[10];
    for (i = 0; i < n; i++) {
        s[n - i - 1] = '0' + (x % 10);
        x /= 10;
    }
    for (i=0; i<(n - 1); i++) {
        if (s[i] == '0') s[i] = fillch; else break;
    }
    for (i=0; i<n; i++) output(serial,s[i]);
}

/*-----------------15.04.04 22:34-------------------
 *   Display -32768..32768 with or without leading zeros
 *     printdec(12345,6,'0') -> 012345
 *     printdec(12345,5,'')  -> 12345
 *     printdec(12345,6,' ') ->  12345
 *--------------------------------------------------*/
void output_dec_signed(uint8_t serial,int16_t x, unsigned char n, unsigned char fillch)
{
	if( x<0 )
		output(serial,'-');
	else
		output(serial,'+');
	x = abs(x);
	output_dec(serial,x,n,fillch);
}


/*-----------------31.5.99 18:18--------------------
 *  Display unsigned long 0-4294967296 (32bit)
 *--------------------------------------------------*/
void output_print10(uint8_t serial, unsigned long x)
{
  unsigned  long int y;
  if (x<4294967295ul)
  {
    y=x/1000000000;output(serial, y+0x30);x-=(y*1000000000);
    y=x/100000000;output(serial, y+0x30);x-=(y*100000000);
    y=x/10000000;output(serial, y+0x30);x-=(y*10000000);
    y=x/1000000;output(serial, y+0x30);x-=(y*1000000);
    y=x/100000;output(serial, y+0x30);x-=(y*100000);
    y=x/10000;output(serial, y+0x30);x-=(y*10000);
    y=x/1000;output(serial, y+0x30);x-=(y*1000);
    y=x/100;output(serial, y+0x30);x-=(y*100);
    y=x/10;output(serial, y+0x30);x-=(y*10);
    output(serial, x+0x30);
  }
  else output_str(serial, "Err");
}


/*-----------------28.08.99 22:49-------------------
 *   Simple printf function (no fp, and strings),
 *   but it works for signed numbers
 *--------------------------------------------------*/

int output_print(uint8_t serial, const char *format, ...) {
  static const char hex[]= "0123456789ABCDEF";
  char format_flag;
  unsigned int u_val, div_val, base;
//  char *ptr;
  va_list ap;

  va_start (ap, format);
  for (;;) {
     while ((format_flag = *format++) != '%') {
       if (!format_flag) {
         va_end (ap);
         return (0);
       }
       output(serial, format_flag);
     }

     switch (format_flag = *format++) {
       case 'c': format_flag = va_arg(ap,int);
       default:  output(serial, format_flag); continue;

       case 'd': base = 10; div_val = 10000; goto CONVERSION_LOOP;
       case 'x': base = 16; div_val = 0x10;

       CONVERSION_LOOP:
       u_val = va_arg(ap,int);
       if (format_flag == 'd') {
         if (((int)u_val) < 0) {
           u_val = - u_val;
           output(serial,'-');
         }
         while (div_val > 1 && div_val > u_val) div_val /= 10;
       }
       do {
         output(serial, hex[u_val / div_val]);
         u_val %= div_val;
         div_val /= base;
       } while (div_val);
    }
  }
}

int input_line( byte file, char *input, byte max_length, int16_t timeout )
{
uint8_t temp,zeiger;
char gotchar;
	
//	input_flush(file);	

	zeiger = 0;
	temp = false;
	MyTimers[TIMER_TIMEOUT].value = timeout;
	MyTimers[TIMER_TIMEOUT].state = TM_START;
	do
	{
		if(get_char(KNET,&gotchar)) // **********************************************************
		{
			input[zeiger]=gotchar;
			if(			(zeiger == max_length-2) 
					|	( (input[zeiger]=='\\')&&(input[zeiger-1]=='<') )  
					|	( (input[zeiger]=='>')&&(input[zeiger-1]=='.') )
					|	( (input[zeiger]=='>')&&(input[zeiger-1]=='!') )
																			) // | (input[zeiger]==13)
				temp = true;
			else
				zeiger++;
		}
	}
	while(  (temp==false) && (MyTimers[TIMER_TIMEOUT].state!=TM_STOP)  );
	input[zeiger+1] = 0;
//	PMIC_CTRL = PMIC_LOLVLEX_bm | PMIC_HILVLEN_bm | PMIC_MEDLVLEN_bm;

	if (temp==true)
	{
		_delay_us(250);
		return( zeiger );
	}
	else
	{
		return( zeiger );
	}
}

char input_char( byte file )			// wartet bis Zeichen eingegeben wird
{
	byte temp_byte;
	char temp_char;
	temp_byte = false;
	do
	{
		temp_byte = get_char( file, &temp_char );
	}
	while( temp_byte==false );
	return( temp_char );
}

void input_flush( byte file )
{
	switch( file )
	{
		case KNET:
			UART0_ring_interpreted = UART0_ring_received;
		break;
		default:
			UART1_ring_interpreted = UART1_ring_received;
		break;
	}
}

byte get_char(byte file, char *got_byte) // gibt Zeichen zurueck, wartet aber nicht
{
	switch( file )
	{
		case KNET:
			return(get_char_0(got_byte));
		break;
		default:
			return(get_char_1(got_byte));
		break;
	}
}

byte get_char_0(char *got_byte) // gibt Zeichen zurueck, wartet aber nicht
{
	if(UART0_ring_received!=UART0_ring_interpreted) //
	{
		(UART0_ring_interpreted)++;
		if(UART0_ring_interpreted == UART0_RING_BUFFER_SIZE)  //RING_BUFFER_SIZE
			UART0_ring_interpreted = 0;
		*got_byte = UART0_ring_buffer[UART0_ring_interpreted];
		return( true );
	}
	else
		return( false );
}

byte get_char_1(char *got_byte) // gibt Zeichen zurueck, wartet aber nicht
{
	if(UART1_ring_received!=UART1_ring_interpreted) //
	{
		(UART1_ring_interpreted)++;
		if(UART1_ring_interpreted == UART1_RING_BUFFER_SIZE)  //RING_BUFFER_SIZE
		UART1_ring_interpreted = 0;
		*got_byte = UART1_ring_buffer[UART1_ring_interpreted];
		return( true );
	}
	else
	return( false );
}
