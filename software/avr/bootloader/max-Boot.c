/**********************************************************/
/* Serial Bootloader for Atmel megaAVR Controllers        */
/*                                                        */
/* tested with ATmega8								      */
/* should work with other mega's, see code for details    */
/*                                                         */
/* ATmegaBOOT.c                                           */
/*                                                        */
/* build: 050815                                          */
/* date : 12   .2005                                      */
/*                                                        */
/*                                                        */
/* Thanks to Karl Pitrich for fixing a bootloader pin     */
/* problem and more informative LED blinking!             */
/*                                                        */
/* For the latest version see:                            */
/* http://www.chip45.com/                                 */
/*                                                        */
/* ------------------------------------------------------ */
/*                                                        */
/* based on stk500boot.c                                  */
/* Copyright (c) 2003, Jason P. Kyle                      */
/* All rights reserved.                                   */
/* see avr1.org for original file and information         */
/*                                                        */
/* This program is free software; you can redistribute it */
/* and/or modify it under the terms of the GNU General    */
/* Public License as published by the Free Software       */
/* Foundation; either version 2 of the License, or        */
/* (at your option) any later version.                    */
/*                                                        */
/* This program is distributed in the hope that it will   */
/* be useful, but WITHOUT ANY WARRANTY; without even the  */
/* implied warranty of MERCHANTABILITY or FITNESS FOR A   */
/* PARTICULAR PURPOSE.  See the GNU General Public        */
/* License for more details.                              */
/*                                                        */
/* You should have received a copy of the GNU General     */
/* Public License along with this program; if not, write  */
/* to the Free Software Foundation, Inc.,                 */
/* 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA */
/*                                                        */
/* Licence can be viewed at                               */
/* http://www.fsf.org/licenses/gpl.txt                    */
/*                                                        */
/* Target = Atmel AVR m128,m64,m32,m16,m8,m162,m163,m169, */
/* m8515,m8535. ATmega161 has a very small boot block so  */
/* isn't supported.                                       */
/*                                                        */
/* Tested with m128,m8,m163 - feel free to let me know    */
/* how/if it works for you.                               */
/*                                                        */
/**********************************************************/

/* some includes */
#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
//#include <avr/wdt.h>
//#include <avr/eeprom.h>
#include "../migration8-168.h"


#if defined (F16000000)
	#define F_CPU     16000000
#elif defined (F20000000)
	#define F_CPU     20000000
#else
	#define F_CPU     8000000
#endif

#define NOINIT  __attribute__ ((section (".noinit")))

/* set the UART baud rate */
#define BAUD_RATE   38400


/* SW_MAJOR and MINOR needs to be updated from time to time to avoid warning message from AVR Studio */
/* never allow AVR Studio to do an update !!!! */
#define HW_VER	 0x02
#define SW_MAJOR 0x01
#define SW_MINOR 0x0f


/* Adjust to suit whatever pin your hardware uses to enter the bootloader */

#define BL_DDR  DDRD
#define BL_PORT PORTD
#define BL_PIN  PIND
#define BL      PIND2

/* onboard LED is used to indicate, that the bootloader was entered (3x flashing) */
/* if monitor functions are included, LED goes on after monitor was entered */

#define LED_DDR  DDRB
#define LED_PORT PORTB
#define LED_PIN  PINB
//#define LED      PINB0

/* define various device id's */
/* manufacturer byte is always the same */
#define SIG1	0x1E	// Atmel

#if defined (__AVR_ATmega8__)

#define SIG2	0x93
#define SIG3	0x07
#define PAGE_SIZE	32U	//32 words

#endif

#if defined (__AVR_ATmega168__)

//#include "lcd.h"
//#include "lcd.c"

#define SIG2	0x94
#define SIG3	0x06
#define PAGE_SIZE	64U	//64 words

#endif


/* function prototypes */
void putch(char);
char getch(void);
void getNch(uint8_t);
void byte_response(uint8_t);
void nothing_response(void);
char gethex(void);
void puthex(char);
//void flash_led(void);

/* some variables */
union address_union {
    uint16_t word;
    uint8_t  byte[2];
} address;

union length_union {
    uint16_t word;
    uint8_t  byte[2];
} length;

uint8_t eeprom NOINIT;
uint8_t	rampz NOINIT;

uint8_t buff[256] NOINIT;
uint8_t address_high NOINIT;

uint8_t i NOINIT;

char gethex(void) {
    char ah,al;

    ah = getch();
	putch(ah);

	al = getch();
	putch(al);

    if(ah >= 'a')
	{
		ah = ah - 'a' + 0x0a;
    } else if(ah >= '0')
	{
		ah -= '0';
    }

    if(al >= 'a')
	{
		al = al - 'a' + 0x0a;
    } else if(al >= '0')
	{
		al -= '0';
    }

    return (ah << 4) + al;

}


void puthex(char ch) {
    char a;

    a = (ch & 0xf0) >> 4;
    if(a >= 0x0a)
	{
		a = a - 0x0a + 'a';
    } else
	{
		a += '0';
    }
	putch(a);
    a = (ch & 0x0f);
    if(a >= 0x0a)
	{
		a = a - 0x0a + 'a';
    } else
	{
		a += '0';
    }
    putch(a);
}


void putch(char ch)
{
    /* m8,16,32,169,8515,8535,163 */
    while (!(UCSRA & _BV(UDRE)));
    UDR = ch;
}

char getch(void)
{
    /* m8,16,32,169,8515,8535,163 */
    while(!(UCSRA & _BV(RXC)));
    return UDR;
}

void getNch(uint8_t count)
{
    for(;count;count--)
    {
	getch();
    }
}

void byte_response(uint8_t val)
{
    if (getch() == ' ')
    {
		putch(0x14);
		putch(val);
		putch(0x10);
    }
}

void nothing_response(void)
{
    if (getch() == ' ')
    {
		putch(0x14);
		putch(0x10);
    }
}

void (*app_start)(void) = 0x0000;

/* main program starts here */
int main(void)
{
    uint8_t ch;
    uint16_t w;

    asm volatile("nop\n\t");

    /* set pin direction for bootloader pin */
    BL_PORT |= _BV(BL);
    BL_DDR &= ~_BV(BL);
    DDRB |= 2; // PWM to output
    PORTB &=~2; // pwm out low
    asm volatile("nop\n\t");
    asm volatile("nop\n\t");

    /* check if flash is programmed already, if not start bootloader anyway */
//#if defined (__AVR_ATmega168__)
//    if(pgm_read_byte_near(0x0000) != 0xFF)
//#endif
    {
	/* check if bootloader pin is set high */
		if(bit_is_set(BL_PIN, BL))
		{
           app_start();
        }
    }

#ifdef LED
    LED_DDR |= _BV(LED);
//    LED_PORT |= _BV(LED); // turn on led
#endif
    /* initialize UART(s) depending on CPU defined */
    /* m8,m16,m32,m169,m8515,m8535 */
    UBRRL = (uint8_t)(F_CPU/(BAUD_RATE*16L)-1);
    UBRRH = (F_CPU/(BAUD_RATE*16L)-1) >> 8;
//	UBRRL = 103; // 9600@16MHz
    UCSRB = _BV(TXEN)|_BV(RXEN);
#if defined (__AVR_ATmega168__)
		putch(0x14);
		putch('A');
		putch('V');
		putch('R');
		putch(' ');
		putch('I');
		putch('S');
		putch('P');
		putch(0x10);
//	lcdInit();
//	lcdPrint("MAX-Boot");
#endif

    /* forever loop */
    for (;;)
	{
	/* get character from UART */
	ch = getch();
	/* A bunch of if...else if... gives smaller code than switch...case ! */
	/* Hello is anyone home ? */
	if((ch=='0') || (ch=='P') || (ch=='Q') || (ch=='R') || (ch=='@'))
	{
	    nothing_response();
	}

	/* Request programmer ID */
	/* Not using PROGMEM string due to boot block in m128 being beyond 64kB boundry  */
	/* Would need to selectively manipulate RAMPZ, and it's only 9 characters anyway so who cares.  */
/*	else if(ch=='1') {
	    if (getch() == ' ') {
		putch(0x14);
		putch('A');
		putch('V');
		putch('R');
		putch(' ');
		putch('I');
		putch('S');
		putch('P');
		putch(0x10);
	    }
	}
*/
	/* AVR ISP/STK500 board commands  DON'T CARE so default nothing_response */
/*	else if(ch=='@') {
	    ch2 = getch();
	    if (ch2>0x85) getch();
	    nothing_response();
	}
*/
	/* AVR ISP/STK500 board requests */
	else if(ch=='A')
	{
	    ch =  getch();
	    if(ch==0x80) byte_response(HW_VER);		// Hardware version
	    else if(ch==0x81) byte_response(SW_MAJOR);	// Software major version
	    else if(ch==0x82) byte_response(SW_MINOR);	// Software minor version
	    else if(ch==0x98) byte_response(0x03);		// Unknown but seems to be required by avr studio 3.56
	    else    byte_response(0x00);				// Covers various unnecessary responses we don't care about
	}

	/* Device Parameters  DON'T CARE, DEVICE IS FIXED  */
	else if(ch=='B')
	{
	    getNch(20);
	    nothing_response();
	}

	/* Parallel programming stuff  DON'T CARE  */
	else if(ch=='E')
	{
	    getNch(5);
	    nothing_response();
	}

	/* Enter programming mode  */
//	else if(ch=='P') {
//	    nothing_response();
//	}

	/* Leave programming mode  */
//	else if(ch=='Q') {
//	    nothing_response();
//	}

	/* Erase device, don't care as we will erase one page at a time anyway.  */
//	else if(ch=='R') {
//	    nothing_response();
//	}

	/* Set address, little endian. EEPROM in bytes, FLASH in words  */
	/* Perhaps extra address bytes may be added in future to support > 128kB FLASH.  */
	/* This might explain why little endian was used here, big endian used everywhere else.  */
	else if(ch=='U')
	{
	    address.byte[0] = getch();
	    address.byte[1] = getch();
	    nothing_response();
	}

	/* Universal SPI programming command. Signature bytes.  Would be used for fuses and lock bits.  */
	else if(ch=='V')
	{
//		getNch(4);
//		byte_response(0x00);
		// schuld be 0x30 nn {0x00 0x01 0x02} mm
//	    getNch(2);
		getch();
		getch();
		ch=getch();
		getch();
	    if(ch==1) byte_response(SIG2);
	    else if(ch==2) byte_response(SIG3);
	    else byte_response(SIG1);
	}

	/* Write memory, length is big endian and is in bytes  */
	else if(ch=='d')
	{
#ifdef LED
		LED_PORT |= _BV(LED);
#endif
	    length.byte[1] = getch();
	    length.byte[0] = getch();
		//eeprom = 0;
//		if(getch() == 'E') eeprom = 1;
		getch();
	    for (w=0;w<length.word;w++)
		{
			buff[w] = getch();	                        // Store data in buffer, can't keep up with serial data stream whilst programming pages
	    }
	    if (getch() == ' ')
		{
//			if (eeprom)
			{		                //Write to EEPROM one byte at a time
//				for(w=0;w<length.word;w++)
//				{
//					eeprom_write_byte((void *)address.word,buff[w]);
//					address.word++;
//				}
			}
//			else
			{					        //Write to FLASH one page at a time
			    address.word = address.word << 1;	        //address * 2 -> byte location
 			    if ((length.byte[0] & 0x01)) length.word++;	//Even up an odd number of bytes
#if defined (__AVR_ATmega168__)
			    if ((address.word+length.word)<=(16384-2048))
#elif defined (__AVR_ATmega8__)
			    if ((address.word+length.word)<=(8192-1024))
#endif
			    {
#if defined (__AVR_ATmega168__)
//	lcdGotoY(1);
//	lcdControlWrite(1<<LCD_DDRAM | (0x40));
//	lcdDataWrite('w');
//	lcdNum(address.word,5,0);
//	lcdDataWrite('-');
//	lcdNum(length.word,4,0);
#endif
				cli();					//Disable interrupts, just to be sure
	//		    while(bit_is_set(EECR,EEWE));			//Wait for previous EEPROM writes to complete
				asm volatile(
					 "clr	r17		\n\t"	//page_word_count
					 "lds	r30,address	\n\t"	//Address of FLASH location (in bytes)
					 "lds	r31,address+1	\n\t"
					 "ldi	r28,lo8(buff)	\n\t"	//Start of buffer array in RAM
					 "ldi	r29,hi8(buff)	\n\t"
					 "lds	r24,length	\n\t"	//Length of data to be written (in bytes)
					 "lds	r25,length+1	\n\t"
					 "length_loop:		\n\t"	//Main loop, repeat for number of words in block
					 "cpi	r17,0x00	\n\t"	//If page_word_count=0 then erase page
					 "brne	no_page_erase	\n\t"
					 "wait_spm1:		\n\t"
					 "lds	r16,%0		\n\t"	//Wait for previous spm to complete
					 "andi	r16,1           \n\t"
					 "cpi	r16,1           \n\t"
					 "breq	wait_spm1       \n\t"
					 "ldi	r16,0x03	\n\t"	//Erase page pointed to by Z
					 "sts	%0,r16		\n\t"
					 "spm			\n\t"
					 "wait_spm2:		\n\t"
					 "lds	r16,%0		\n\t"	//Wait for previous spm to complete
					 "andi	r16,1           \n\t"
					 "cpi	r16,1           \n\t"
					 "breq	wait_spm2       \n\t"
					 "ldi	r16,0x11	\n\t"	//Re-enable RWW section
					 "sts	%0,r16		\n\t"
					 "spm			\n\t"
					 "no_page_erase:		\n\t"
					 "ld	r0,Y+		\n\t"	//Write 2 bytes into page buffer
					 "ld	r1,Y+		\n\t"
					 "wait_spm3:		\n\t"
					 "lds	r16,%0		\n\t"	//Wait for previous spm to complete
					 "andi	r16,1           \n\t"
					 "cpi	r16,1           \n\t"
					 "breq	wait_spm3       \n\t"
					 "ldi	r16,0x01	\n\t"	//Load r0,r1 into FLASH page buffer
					 "sts	%0,r16		\n\t"
					 "spm			\n\t"
					 "inc	r17		\n\t"	//page_word_count++
					 "cpi r17,%1	        \n\t"
					 "brlo	same_page	\n\t"	//Still same page in FLASH
					 "write_page:		\n\t"
					 "clr	r17		\n\t"	//New page, write current one first
					 "wait_spm4:		\n\t"
					 "lds	r16,%0		\n\t"	//Wait for previous spm to complete
					 "andi	r16,1           \n\t"
					 "cpi	r16,1           \n\t"
					 "breq	wait_spm4       \n\t"
					 "ldi	r16,0x05	\n\t"	//Write page pointed to by Z
					 "sts	%0,r16		\n\t"
					 "spm			\n\t"
					 "wait_spm5:		\n\t"
					 "lds	r16,%0		\n\t"	//Wait for previous spm to complete
					 "andi	r16,1           \n\t"
					 "cpi	r16,1           \n\t"
					 "breq	wait_spm5       \n\t"
					 "ldi	r16,0x11	\n\t"	//Re-enable RWW section
					 "sts	%0,r16		\n\t"
					 "spm			\n\t"
					 "same_page:		\n\t"
					 "adiw	r30,2		\n\t"	//Next word in FLASH
					 "sbiw	r24,2		\n\t"	//length-2
					 "breq	final_write	\n\t"	//Finished
					 "rjmp	length_loop	\n\t"
					 "final_write:		\n\t"
					 "cpi	r17,0		\n\t"
					 "breq	block_done	\n\t"
					 "adiw	r24,2		\n\t"	//length+2, fool above check on length after short page write
					 "rjmp	write_page	\n\t"
					 "block_done:		\n\t"
					 "clr	__zero_reg__	\n\t"	//restore zero register
					 : "=m" (SPMCR) : "M" (PAGE_SIZE) : "r0","r16","r17","r24","r25","r28","r29","r30","r31"
					 );
				/* Should really add a wait for RWW section to be enabled, don't actually need it since we never */
				/* exit the bootloader without a power cycle anyhow */
				}
			}
			putch(0x14);
			putch(0x10);
#ifdef LED
			LED_PORT &= ~_BV(LED);
#endif
	    }
	}

        /* Read memory block mode, length is big endian.  */
	else if(ch=='t')
	{
		length.byte[1] = getch();
		length.byte[0] = getch();
		getch();
/*	    if (getch() == 'E') eeprom = 1;
		else */
		{
	//		eeprom = 0;
			address.word = address.word << 1;	        // address * 2 -> byte location
		}
		if (getch() == ' ')
		{		                // Command terminator
#if defined (__AVR_ATmega168__)
//	lcdGotoY(1);
//	lcdControlWrite(1<<LCD_DDRAM | (0x40));
//	lcdDataWrite('r');
//	lcdNum(address.word,5,0);
//	lcdDataWrite('-');
//	lcdNum(length.word,4,0);
#endif
			putch(0x14);
			for (w=0;w < length.word;w++)
			{		        // Can handle odd and even lengths okay
	/*		    if (eeprom)
				{	                        // Byte access EEPROM read
					putch(eeprom_read_byte((void *)address.word));
					address.word++;
				}
				else */
				{

					//if (!flags.rampz)
					putch(pgm_read_byte_near(address.word));
					address.word++;
				}
			}
			putch(0x10);
		}
	}
/* Get device signature bytes  */
    else if(ch=='u') {
	    if (getch() == ' ') {
		putch(0x14);
		putch(SIG1);
		putch(SIG2);
		putch(SIG3);
		putch(0x10);
	    }
	}

        /* Read oscillator calibration byte */
/*        else if((ch=='v')) {
	    byte_response(0x00);
	}
*/
    }
    /* end of forever loop */

}

// end of file ATmegaBOOT.c
