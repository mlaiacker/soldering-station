/***************************************************************************
 * Atmel AVR USART Library for GCC
 * Version: 1.0
 * 
 * Works with AVR MCUs equiped with USART hardware (ATmega series). 
 * Does not work with older UART's or USI of ATtiny series.
 * Tested with ATmega8.
 * 
 * Uses USART Receive Complete Interrupt. Disabling Global Interrupts
 * after usart initialization disables data receive.
 *
 * Jaakko Ala-Paavola 2003/06/28
 * http://www.iki.fi/jap email:jap@iki.fi
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "usart.h"
#include "hardware.h"

#ifdef UART
/*
void usartInit(void) {

  // Baud rate selection
//  UBRR = baud_divider;
  UBRRL = UART_UBRRL;

  // USART setup
//  UCSRA = 0x02;        // 0000 0010
                       // U2X enabled
//  UCR = 0x8B;        // 1000 1011
	UCSRB = _BV(TXEN);
                       // Access UCSRC, Asyncronous 8N1
//UCSRB = 0x98;        // 1001 1000
                       // Receiver enabled, Transmitter enabled
                       // RX Complete interrupt enabled
//sei();               // Enable interrupts globally
}
*/
void usartPutc(unsigned char data) {
    while (!(UCSRA & 0x20)); // Wait untill USART data register is empty
    // Transmit data
    UDR = data;
}


void usartNum(signed short x,unsigned char numDigits,unsigned char pointPos)
{
    unsigned char c,minus=0;
	unsigned short divisor=1;
	unsigned char pos = numDigits;
    if(x<0) 
    {
		minus=1;
    	x=-x;
    } //else x=val;
	if(numDigits==0)
	{
		for(pos=1;((x/divisor)>=10)&&(pos<5);pos++) divisor*=10;
	}
	else for(;(numDigits>1);numDigits--) divisor*=10;

	usartPutc(' ');
	if(minus)
	{
    	usartPutc('-');
		minus=0;
	} else usartPutc(' ');
	
    do {
		if(pointPos==pos) usartPutc('.');
		pos--;
        c = ( unsigned char)((x/divisor)%10);
		usartPutc(c +'0');
        divisor /= 10;
    } while (divisor);
}

void usart_puts_prog(const char *str)
{
	// print a string stored in program memory
	register char c;

	// check to make sure we have a good pointer
//	if (!str) return;
	
	// print the string until the null-terminator
	while((c = pgm_read_byte(str++)))
		usartPutc(c);
}


//#if defined (__AVR_ATmega168__)

char usart_buffer[USART_BUFFER_SIZE];
volatile unsigned char usart_buffer_pos_first = 0, usart_buffer_pos_last = 0;
volatile unsigned char usart_buffer_overflow = 0;

/*
void usart_puts(char *data) {
  int len, count;
  
  len = strlen(data);
  for (count = 0; count < len; count++) 
    usart_putc(*(data+count));
}
*/
char usart_getc(void) {
  // Wait untill unread data in ring buffer
  if (!usart_buffer_overflow)
    while(usart_buffer_pos_first == usart_buffer_pos_last);
  usart_buffer_overflow = 0;
  // Increase first pointer
  if (++usart_buffer_pos_first >= USART_BUFFER_SIZE) 
    usart_buffer_pos_first = 0;
  // Get data from the buffer
  return usart_buffer[usart_buffer_pos_first];
}

unsigned char usart_unread_data(void) {
  if (usart_buffer_overflow)
    return USART_BUFFER_SIZE;
  if (usart_buffer_pos_last > usart_buffer_pos_first)
    return usart_buffer_pos_last - usart_buffer_pos_first;
  if (usart_buffer_pos_last < usart_buffer_pos_first)
    return USART_BUFFER_SIZE - usart_buffer_pos_first + usart_buffer_pos_last;
  return 0;
}


ISR(USART_RXC_vect) {
  // Increase last buffer 
  if (++usart_buffer_pos_last >= USART_BUFFER_SIZE)
    usart_buffer_pos_last = 0;
  if (usart_buffer_pos_first == usart_buffer_pos_last) 
    usart_buffer_overflow++;
  // Put data to the buffer
  usart_buffer[usart_buffer_pos_last] = UDR;
}
#endif

//#endif
