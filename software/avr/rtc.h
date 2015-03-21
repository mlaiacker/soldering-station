// Real Time Clock
/* 
* max-Lader 
* entwickelt von:
* Maximilian Laiacker post@mlaiacker.de http://mlaiacker.de/
*/


/* 
* !!! noch nicht fertig !!!
* Getestet nur auf einem mega32, mega8 mit 8MHz Tackt!!
* ohne externen 32,768kHz Quarz 
*/

#ifndef RTC_H
#define RTC_H

#include "hardware.h"

/*--- System Parameter ---*/

// haben wir einen externen 32,768kHz Uhrenquarz an TOSC1, TOSC2 ?
//#define EXTERNAL_32768

#ifndef EXTERNAL_32768

// wenn nicht hier weiter
  #ifndef F_CPU
    // wenn es noch nich wo anders definiert wurde dann hier anpassen
    #define F_CPU 8000000
  #endif

  #define RTC_PRESCALER		(64L)
  #define RTC_TICKS_TO_OVERFLOW	(250L)

#if defined (__AVR_ATmega168__)
  #define RTC_OVERFLOW_TIME	(4) //((RTC_PRESCALER*RTC_TICKS_TO_OVERFLOW*1000)/F_CPU) //[ms] eigendlich 3.2ms bei 20Mhz
//  #define RTC_OVERFLOW_FRACTION	(200) // µs
#elif defined (__AVR_ATmega8__)
  #define RTC_OVERFLOW_TIME	((RTC_PRESCALER*RTC_TICKS_TO_OVERFLOW*1000L)/F_CPU) //[ms]
#endif

  #define RTC_START_VAL		(0xff-RTC_TICKS_TO_OVERFLOW)
#else

#ifdef RTC_OVERFLOW_FRACTION
  extern volatile unsigned short Time_us;
#endif
// ok wir haben also einen externen Quarz dann Timer0 Asynchron laufen lassen
//...

#endif

/* */
#define mSec(time)	(((time)%1000) )
#define Sec(time)	((((time)/1000)%60) )
#define Min(time)	((((time)/60000)%60))
#define Hour(time)	((((time)/3600000)%24))


/* für Kompatibilität*/
#define RTC_Init          rtcInit
#define UpdateTime        rtcInt
#define GetTime           rtcGetTime
#define SetTime           rtcSetTime
#define RTC_Delay         rtcDelay
#define RTC_Einschaltverz rtcEinschaltverz
#define RTC_Ausschaltverz rtcAusschaltverz
/* ------ */

// Zeittyp (32bit)
typedef unsigned long t_Time;

extern volatile t_Time Time;
/* muss aus dem Hauptrogramm aufgerufen werden
* z.B.:
* SIGNAL(SIG_OVERFLOW0)
* {
* 	rtcInt();
* )
*/
void          rtcInt(void);

void          rtcInit(void);
//t_Time        rtcGetTime(void);
//t_Time        rtcSetTime(t_Time t);
/* Wartet ms Millisekunden */
void          rtcDelay(t_Time ms);
/*
* Einschaltverzögerung der Bedingung 'bedingung', 
*  mit der Verzögerung 'verz', 
*  und ein Speicherplatz für die Zeit 'timevar'
* gibt -1 zurück, wenn für 'verz' ms 'bedingung'!=0 sonst 0
*/
//unsigned char rtcEinschaltverz(unsigned char bedingung, t_Time verz, t_Time* timevar);
//#define se rtcEinschaltverz

/* 
* gibt 0 zurück, wenn für 'verz' ms bedingung==0 sonst -1
*/
//unsigned char rtcAusschaltverz(unsigned char bedingung, t_Time verz, t_Time* timevar);
//#define sa rtcAusschaltverz

// wird wenn "bedingung"!=0 für "verz" ms für einen aufruf wahr zurück
//unsigned char rtcImpulsverz(unsigned char bedingung,t_Time verz,t_Time* timevar);

#endif
