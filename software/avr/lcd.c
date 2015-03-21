/*! \file lcd.c \brief Character LCD driver for HD44780/SED1278 displays. */
//*****************************************************************************
//
// File Name	: 'lcd.c'
// Title		: Character LCD driver for HD44780/SED1278 displays
//					(usable in mem-mapped, or I/O mode)
// Author		: Pascal Stang
// Created		: 11/22/2000
// Revised		: 4/30/2002
// Version		: 1.1
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#include <avr/io.h>
#include <avr/pgmspace.h>
#include "global.h"

#include "lcd.h"

char lcdPresend=1;

/*************************************************************/
/********************** LOCAL FUNCTIONS **********************/
/*************************************************************/

void lcdInitHW(void)
{
	// initialize I/O ports
	// initialize LCD control lines
//	cbi(LCD_CTRL_PORT, LCD_CTRL_RS); // nich unbedingt nötig da nach reset sowiso alles auf 0
//	cbi(LCD_CTRL_PORT, LCD_CTRL_RW);
//	cbi(LCD_CTRL_E_PORT, LCD_CTRL_E);
	// initialize LCD control lines to output
	sbi(LCD_CTRL_DDR, LCD_CTRL_RS);
	sbi(LCD_CTRL_DDR, LCD_CTRL_RW);
	sbi(LCD_CTRL_E_DDR, LCD_CTRL_E);
	// initialize LCD data port to input
	// initialize LCD data lines to pull-up
	//	LCD_DATA_DDR = (LCD_DATA_DDR)&0x0F;		// set data I/O lines to input (4bit)
	//	LCD_DATA_POUT = (LCD_DATA_POUT)|0xF0;	// set pull-ups to on (4bit)
	cbi(LCD_DATA_DDR_D4, LCD_DATA_D4);
	sbi(LCD_DATA_PORT_D4, LCD_DATA_D4);
	cbi(LCD_DATA_DDR_D5, LCD_DATA_D5);
	sbi(LCD_DATA_PORT_D5, LCD_DATA_D5);
	cbi(LCD_DATA_DDR_D6, LCD_DATA_D6);
	sbi(LCD_DATA_PORT_D6, LCD_DATA_D6);
	cbi(LCD_DATA_DDR_D7, LCD_DATA_D7);
	sbi(LCD_DATA_PORT_D7, LCD_DATA_D7);
	lcdPresend = 1;
}

void lcdDataPortInput(void)
{
	cbi(LCD_DATA_DDR_D4, LCD_DATA_D4);
	cbi(LCD_DATA_DDR_D5, LCD_DATA_D5);
	cbi(LCD_DATA_DDR_D6, LCD_DATA_D6);
	cbi(LCD_DATA_DDR_D7, LCD_DATA_D7);
	sbi(LCD_DATA_PORT_D7, LCD_DATA_D7); // busy line pull up
}

void lcdDataPortOutput(void)
{
	sbi(LCD_DATA_DDR_D4, LCD_DATA_D4);
	sbi(LCD_DATA_DDR_D5, LCD_DATA_D5);
	sbi(LCD_DATA_DDR_D6, LCD_DATA_D6);
	sbi(LCD_DATA_DDR_D7, LCD_DATA_D7);
}


u08 lcdDataPortRead(void)
{
	u08 res=0;
	if(LCD_DATA_PIN_D4 & (1<<LCD_DATA_D4)) res |=16;
	if(LCD_DATA_PIN_D5 & (1<<LCD_DATA_D5)) res |=32;
	if(LCD_DATA_PIN_D6 & (1<<LCD_DATA_D6)) res |=64;
	if(LCD_DATA_PIN_D7 & (1<<LCD_DATA_D7)) res |=128;
	return res;
}

void lcdDataPortWrite(u08 data)
{
	if(data&16 ) sbi(LCD_DATA_PORT_D4, LCD_DATA_D4); else cbi(LCD_DATA_PORT_D4, LCD_DATA_D4);
	if(data&32 ) sbi(LCD_DATA_PORT_D5, LCD_DATA_D5); else cbi(LCD_DATA_PORT_D5, LCD_DATA_D5);
	if(data&64 ) sbi(LCD_DATA_PORT_D6, LCD_DATA_D6); else cbi(LCD_DATA_PORT_D6, LCD_DATA_D6);
	if(data&128) sbi(LCD_DATA_PORT_D7, LCD_DATA_D7); else cbi(LCD_DATA_PORT_D7, LCD_DATA_D7);
}

void lcdBusyWait(void)
{
	unsigned char i;
	int k=0;
	// wait until LCD busy bit goes to zero
	// do a read from control register

	cbi(LCD_CTRL_PORT, LCD_CTRL_RS);				// set RS to "control"
//	LCD_DATA_DDR &= 0x0F;	// set data I/O lines to input (4bit)
//	LCD_DATA_POUT |= 0xF0;	// set pull-ups to on (4bit)
	lcdDataPortInput();
	sbi(LCD_CTRL_PORT, LCD_CTRL_RW);				// set R/W to "read"
	sbi(LCD_CTRL_E_PORT, LCD_CTRL_E);					// set "E" line
	LCD_DELAY;								// wait
	if(lcdPresend)
	{
	while((lcdDataPortRead() & (1<<LCD_BUSY)) && k<=2000)
//	while(lcdDataPortRead() & (1<<LCD_BUSY))
	{
		k++;
		for(i=0;i<4;i++)
		{
			if((i%2)==0) cbi(LCD_CTRL_E_PORT, LCD_CTRL_E);		// clear "E" line
			if((i%2)==1) sbi(LCD_CTRL_E_PORT, LCD_CTRL_E);		// clear "E" line
			LCD_DELAY;									// wait
			LCD_DELAY;									// wait
		}
	}
	if(k>=2000) lcdPresend = 0;
	}
	cbi(LCD_CTRL_E_PORT, LCD_CTRL_E);			// clear "E" line
	//	leave data lines in input mode so they can be most easily used for other purposes
}
void lcdWrite(u08 type,u08 data) 
{
// write the control byte to the display controller
	lcdBusyWait();							// wait until LCD not busy
	if(type==LCD_WRITE_DATA) sbi(LCD_CTRL_PORT, LCD_CTRL_RS);			// set RS to "data"
	else cbi(LCD_CTRL_PORT, LCD_CTRL_RS);			// set RS to "control"
	cbi(LCD_CTRL_PORT, LCD_CTRL_RW);			// set R/W to "write"
		// 4 bit write
		sbi(LCD_CTRL_E_PORT, LCD_CTRL_E);	// set "E" line
//		LCD_DATA_DDR = LCD_DATA_DDR|0xF0;	// set data I/O lines to output (4bit)
		lcdDataPortOutput();
		//LCD_DATA_POUT= (LCD_DATA_POUT&0x0F) | (data&0xF0) ;	// output data, high 4 bits
		lcdDataPortWrite(data&0xF0);
		LCD_DELAY;								// wait
		LCD_DELAY;								// wait
		cbi(LCD_CTRL_E_PORT, LCD_CTRL_E);	// clear "E" line
		LCD_DELAY;								// wait
		LCD_DELAY;								// wait
		sbi(LCD_CTRL_E_PORT, LCD_CTRL_E);	// set "E" line
	//	LCD_DATA_POUT = (((LCD_DATA_POUT)&0x0F) | (data<<4) );	// output data, low 4 bits
		lcdDataPortWrite(data<<4);
		LCD_DELAY;								// wait
		LCD_DELAY;								// wait
		cbi(LCD_CTRL_E_PORT, LCD_CTRL_E);	// clear "E" line
	//	leave data lines in input mode so they can be most easily used for other purposes
		//LCD_DATA_DDR = LCD_DATA_DDR&0x0F;		// set data I/O lines to input (4bit)
		//LCD_DATA_POUT= LCD_DATA_POUT|0xF0;	// set pull-ups to on (4bit)
		lcdDataPortInput();
}

void lcdDataWrite(u08 data)
{
	lcdWrite(LCD_WRITE_DATA,data);
}

void lcdControlWrite(u08 data)
{
	lcdWrite(LCD_WRITE_CONTROL,data);
}

/*
u08 lcdControlRead(void)
{
// read the control byte from the display controller
	register u08 data;
#ifdef LCD_PORT_INTERFACE
	lcdBusyWait();				// wait until LCD not busy
	#ifdef LCD_DATA_4BIT
		LCD_DATA_DDR =  ((LCD_DATA_DDR)&0x0F);		// set data I/O lines to input (4bit)
		LCD_DATA_POUT = ((LCD_DATA_POUT)|0xF0);	// set pull-ups to on (4bit)
	#else
		outb(LCD_DATA_DDR, 0x00);			// set data I/O lines to input (8bit)
		outb(LCD_DATA_POUT, 0xFF);			// set pull-ups to on (8bit)
	#endif
	cbi(LCD_CTRL_PORT, LCD_CTRL_RS);		// set RS to "control"
	sbi(LCD_CTRL_PORT, LCD_CTRL_RW);		// set R/W to "read"
	#ifdef LCD_DATA_4BIT
		// 4 bit read
		sbi(LCD_CTRL_PORT, LCD_CTRL_E);	// set "E" line
		LCD_DELAY;						// wait
		LCD_DELAY;						// wait
		data = (LCD_DATA_PIN)&0xF0;	// input data, high 4 bits
		cbi(LCD_CTRL_PORT, LCD_CTRL_E);	// clear "E" line
		LCD_DELAY;						// wait
		LCD_DELAY;						// wait
		sbi(LCD_CTRL_PORT, LCD_CTRL_E);	// set "E" line
		LCD_DELAY;						// wait
		LCD_DELAY;						// wait
		data |= (LCD_DATA_PIN)>>4;	// input data, low 4 bits
		cbi(LCD_CTRL_PORT, LCD_CTRL_E);	// clear "E" line
	#else
		// 8 bit read
		sbi(LCD_CTRL_PORT, LCD_CTRL_E);	// set "E" line
		LCD_DELAY;						// wait
		LCD_DELAY;						// wait
		data = inb(LCD_DATA_PIN);		// input data, 8bits
		cbi(LCD_CTRL_PORT, LCD_CTRL_E);	// clear "E" line
	#endif
	//	leave data lines in input mode so they can be most easily used for other purposes
#else
	//sbi(MCUCR, SRW);			// enable RAM waitstate
	lcdBusyWait();				// wait until LCD not busy
	data = *((volatile unsigned char *) (LCD_CTRL_ADDR));
	//cbi(MCUCR, SRW);			// disable RAM waitstate
#endif
	return data;
}
*/

/*
u08 lcdDataRead(void)
{
// read a data byte from the display
	register u08 data;
#ifdef LCD_PORT_INTERFACE
	lcdBusyWait();				// wait until LCD not busy
	#ifdef LCD_DATA_4BIT
		outb(LCD_DATA_DDR, inb(LCD_DATA_DDR)&0x0F);		// set data I/O lines to input (4bit)
		outb(LCD_DATA_POUT, inb(LCD_DATA_POUT)|0xF0);	// set pull-ups to on (4bit)
	#else
		outb(LCD_DATA_DDR, 0x00);			// set data I/O lines to input (8bit)
		outb(LCD_DATA_POUT, 0xFF);			// set pull-ups to on (8bit)
	#endif
	sbi(LCD_CTRL_PORT, LCD_CTRL_RS);		// set RS to "data"
	sbi(LCD_CTRL_PORT, LCD_CTRL_RW);		// set R/W to "read"
	#ifdef LCD_DATA_4BIT
		// 4 bit read
		sbi(LCD_CTRL_PORT, LCD_CTRL_E);	// set "E" line
		LCD_DELAY;								// wait
		LCD_DELAY;								// wait
		data = inb(LCD_DATA_PIN)&0xF0;	// input data, high 4 bits
		cbi(LCD_CTRL_PORT, LCD_CTRL_E);	// clear "E" line
		LCD_DELAY;								// wait
		LCD_DELAY;								// wait
		sbi(LCD_CTRL_PORT, LCD_CTRL_E);	// set "E" line
		LCD_DELAY;								// wait
		LCD_DELAY;								// wait
		data |= inb(LCD_DATA_PIN)>>4;			// input data, low 4 bits
		cbi(LCD_CTRL_PORT, LCD_CTRL_E);	// clear "E" line
	#else
		// 8 bit read
		sbi(LCD_CTRL_PORT, LCD_CTRL_E);	// set "E" line
		LCD_DELAY;								// wait
		LCD_DELAY;								// wait
		data = inb(LCD_DATA_PIN);			// input data, 8bits
		cbi(LCD_CTRL_PORT, LCD_CTRL_E);	// clear "E" line
	#endif
	//	leave data lines in input mode so they can be most easily used for other purposes
#else
	// memory bus read
	//sbi(MCUCR, SRW);			// enable RAM waitstate
	lcdBusyWait();				// wait until LCD not busy
	data = *((volatile unsigned char *) (LCD_DATA_ADDR));
	//cbi(MCUCR, SRW);			// disable RAM waitstate
#endif
	return data;
}
*/


/*************************************************************/
/********************* PUBLIC FUNCTIONS **********************/
/*************************************************************/

void lcdInit()
{
	// initialize hardware
	lcdInitHW();
	// LCD function set
	lcdControlWrite(LCD_FUNCTION_DEFAULT);
	// clear LCD
	lcdControlWrite(1<<LCD_CLR);
	//delay(60000);	// wait 60ms
	// set entry mode
	lcdControlWrite(1<<LCD_ENTRY_MODE | 1<<LCD_ENTRY_INC);
	// set display to on
	//lcdControlWrite(1<<LCD_ON_CTRL | 1<<LCD_ON_DISPLAY | 1<<LCD_ON_BLINK);
	lcdControlWrite(1<<LCD_ON_CTRL | 1<<LCD_ON_DISPLAY );
	// move cursor to home
	lcdControlWrite(1<<LCD_HOME);
	// set data address to 0
//	lcdControlWrite(1<<LCD_DDRAM | 0x00);

}
/*
void lcdHome(void)
{
	// move cursor to home
	lcdControlWrite(1<<LCD_HOME);
}
*/
/*
void lcdClear(void)
{
	// clear LCD
	lcdControlWrite(1<<LCD_CLR);
}
*/
/*
void lcdGotoXY(u08 x, u08 y)
{
	register u08 DDRAMAddr;

	// remap lines into proper order
	switch(y)
	{
	case 0: DDRAMAddr = LCD_LINE0_DDRAMADDR+x; break;
	case 1: DDRAMAddr = LCD_LINE1_DDRAMADDR+x; break;
//	case 2: DDRAMAddr = LCD_LINE2_DDRAMADDR+x; break;
//	case 3: DDRAMAddr = LCD_LINE3_DDRAMADDR+x; break;
	default: DDRAMAddr = LCD_LINE0_DDRAMADDR+x;
	}

	// set data address
	lcdControlWrite(1<<LCD_DDRAM | DDRAMAddr);
}
*/
// geht immer zum anfang der der zeile "y"
void lcdGotoY(u08 y)
{
	// set data address
	lcdControlWrite(1<<LCD_DDRAM | (0x40*y));
}
/*
void lcdPrintData(char* data, u08 nBytes)
{
	register u08 i;

	// check to make sure we have a good pointer
	if (!data) return;

	// print data
	for(i=0; i<nBytes; i++)
	{
		lcdDataWrite(data[i]);
	}
}
*/

void lcdPrintProgString(const char *str)
{
	// print a string stored in program memory
	register char c;

	// check to make sure we have a good pointer
//	if (!str) return;
	
	// print the string until the null-terminator
	while((c = pgm_read_byte(str++)))
		lcdDataWrite(c);
}
void lcdPrintSpaces(unsigned char n)
{
	for(;n;n--)lcdDataWrite(' ');
}
// zahlen ausgeben, eventuell mit dezimalpunkt an  "pointpos" mit länge "numDigits"
void lcdNum(signed short x,unsigned char numDigits,unsigned char pointPos)
{
    unsigned char c;//,minus=0;
	unsigned char firstNoneZero=0;
	unsigned short divisor=1;
	unsigned char pos = numDigits;
    if(x<0) 
    {
//		minus=1;
    	x=-x;
	    	lcdDataWrite('-');
			firstNoneZero=1;
    }
	for(;(numDigits>1);numDigits--) divisor*=10;
	
    do {
//		if(minus)
//		{
//	    	lcdDataWrite('-');
//			minus=0;
//		}
		if(pointPos==pos) lcdDataWrite('.');
		pos--;
        c = ( unsigned char)((x/divisor)%10);
		if(c||(pos<=pointPos)) firstNoneZero=1;
		if((c==0)&&(firstNoneZero==0)) lcdDataWrite(' ');
        else 
		{
			lcdDataWrite(c +'0');
		}
        divisor /= 10;
    } while (divisor);
}
