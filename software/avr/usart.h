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

// Size of receive ring buffer. Must be at least 2.
#define USART_BUFFER_SIZE 10

/* Baudrate settings. Refer to datasheet for baud rate error.
   Note also maximun baud rate.
   br = baudrate, fosc = clock frequency in megahertzs */
#define USART_BAUDRATE(br, fosc) (fosc*62500/br-1)

/* Initializes USART device. Use USART_BAUDRATE macro for argument or
   consult datasheet for correct value. */
void usartInit(void);

/* Transmit one character. No buffering, waits until previous character
   transmitted. */
void usartPutc(unsigned char a);

void usartNum(signed short x,unsigned char numDigits,unsigned char pointPos);

#define usartPrint(string)			(usart_puts_prog(PSTR(string)))
void usart_puts_prog(const char *str);
//#if defined (__AVR_ATmega168__)
/* Receive one character. Blocking operation, if no new data in buffer. */
char usart_getc(void);

/* Returns number of unread character in ring buffer. */
unsigned char usart_unread_data(void);

//#endif
