/*! \file global.h \brief AVRlib project global include. */
//*****************************************************************************
//
// File Name	: 'global.h'
// Title		: AVRlib project global include 
// Author		: Pascal Stang - Copyright (C) 2001-2002
// Created		: 7/12/2001
// Revised		: 9/30/2002
// Version		: 1.1
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
// 
//	Description : This include file is designed to contain items useful to all
//					code files and projects.
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef GLOBAL_H
#define GLOBAL_H
 
#include "hardware.h"

#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))

#define outb(sfr, val) (_SFR_BYTE(sfr) = (val))
#define inb(sfr) _SFR_BYTE(sfr)
#define inw(sfr) _SFR_WORD(sfr)
#define outw(sfr, val) (_SFR_WORD(sfr) = (val))
//#define BV(bit) _BV(bit)

// global AVRLIB defines
#include "avrlibdefs.h"
// global AVRLIB types definitions
#include "avrlibtypes.h"


#define CYCLES_PER_US ((F_CPU+500000)/1000000) 	// cpu cycles per microsecond

// alle Textstellen EEPROM im Quellcode durch __attribute__ ... ersetzen
#define EEPROM  __attribute__ ((section (".eeprom")))

#define FORCEINLINE __attribute__ ((always_inline))

s16 sign(s16 x);
s16 abs(s16 x);
unsigned char impulsverz(unsigned char bedingung,unsigned char verz,unsigned char *timevar);
unsigned char einschaltverz(unsigned char bedingung, short verz,short *timevar);
s16 max(s16 a,s16 b);

#endif
