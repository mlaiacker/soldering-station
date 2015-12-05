#ifndef HARDWARE_H
#define HARDWARE_H

#if defined (__AVR_ATmega168__)
#include "migration8-168.h"
#endif

#define PROG_VERSION	(16)

#ifndef F_CPU
	#if defined (__AVR_ATmega168__)
		#define F_CPU        16000000L              		// 20MHz processor
	#elif defined (__AVR_ATmega8__)
		#define F_CPU        8000000L               		// 16MHz processor
	#endif
#endif

/* ### ADC ### */

#define ADC_CH_TEMP		1
#define ADC_CH_POTI		0


// Refferenzspannung 2.500V
#define ADC_U_REF	500L // in mv/10
#define ADC_MAX		(1024L)

// umrechnung von adc werten (10bit) in temeratur in °C
#define BITS2TEMP(x) ((x)*(x)/1864L + (x)*100/-282 + 83L) // aus messwerten ermittelt und dann mit matlab(basic fitting) ausgerechnet

#define ADC_PRESCALE			ADC_PRESCALE_DIV128
#define ADC_REFERENCE			ADC_REFERENCE_AVCC

#define A2D_SCAN_CHANNELS	(2)
#define A2D_SCAN_BUF	(16)

// oversampling benutzen statt laufende mittelwert bildung
//#define OVERSAMPLING
#define OVERSAMPLING_RATE		8

/* ### Beep ### */
/*
#define BEEP_DDR		DDRB
#define BEEP_PORT		PORTB
#define BEEP_BIT		0
#define BEEP_ON			sbi(BEEP_PORT,BEEP_BIT)
#define BEEP_OFF		cbi(BEEP_PORT,BEEP_BIT)
#define BEEP_INIT		(BEEP_DDR |= (1<<BEEP_BIT))
*/

/* ### LCD ### */
#define LCD_PORT_INTERFACE
#define LCD_CTRL_PORT	PORTD
#define LCD_CTRL_DDR	DDRD
#define LCD_CTRL_RS		5
#define LCD_CTRL_RW		4
#define LCD_CTRL_E		6
#define LCD_CTRL_E_PORT	PORTD
#define LCD_CTRL_E_DDR	DDRD
// port you will use for data lines
#define LCD_DATA_PORT_D4	PORTD
#define LCD_DATA_DDR_D4		DDRD
#define LCD_DATA_PIN_D4		PIND
#define LCD_DATA_D4		3
#define LCD_DATA_PORT_D5	PORTD
#define LCD_DATA_DDR_D5		DDRD
#define LCD_DATA_PIN_D5		PIND
#define LCD_DATA_D5		7
#define LCD_DATA_PORT_D6	PORTD
#define LCD_DATA_DDR_D6		DDRD
#define LCD_DATA_PIN_D6		PIND
#define LCD_DATA_D6		2
#define LCD_DATA_PORT_D7	PORTB
#define LCD_DATA_DDR_D7		DDRB
#define LCD_DATA_PIN_D7		PINB
#define LCD_DATA_D7		0

// access mode you will use (default is 8bit unless 4bit is selected)
#define LCD_DATA_4BIT

#define LCD_LINES				1	// visible lines
#define LCD_LINE_LENGTH			20	// line length (in characters)

#define LCD_DELAY	asm volatile ("nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n");
//#define LCD_DELAY	asm volatile ("nop\n nop\n nop\n nop\n nop\n nop\n nop\n");

/* ### Tasten ### */
#define TASTE_L	(bit_is_clear(TASTE_L_PIN,TASTE_L_BIT))
#define TASTE_M	(bit_is_clear(TASTE_M_PIN,TASTE_M_BIT))
#define TASTE_R	(bit_is_clear(TASTE_R_PIN,TASTE_R_BIT))

// tasten sind an einzelnen eingängen

#define TASTE_M_PORT	PORTD
#define TASTE_M_PIN		PIND
#define TASTE_M_BIT		2
#define TASTE_M_INIT (TASTE_M_PORT|=(1<<TASTE_M_BIT))


/* ### uart ### */
#define UART // wir machen Ausgaben über die serielle Schnittstelle
#if defined (__AVR_ATmega168__)

	#define UART_BAUD_RATE	9600
//	#define UART_BAUD_RATE	115200
// Baudrate
#elif defined (__AVR_ATmega8__)
//	#define UART_BAUD_RATE	9600L
	#define UART_BAUD_RATE	38400L
	//#define UART_UBRRL	(103) // 9600@16MHz U2X = 0
//	#define UART_UBRRL	(25) // 38400@16MHz U2X = 0
#endif

#endif
