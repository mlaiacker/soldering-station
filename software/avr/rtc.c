/* 
* max-Lader 
* entwickelt von:
* Maximilian Laiacker post@mlaiacker.de http://mlaiacker.de/
*/


#include <avr/io.h>
#include "rtc.h"


/* 
  Real Time Clock using Timer/Counter0
*/
#ifdef RTC_OVERFLOW_FRACTION
  volatile unsigned short Time_us;
#endif

volatile t_Time Time;

/* wird vom interrupt aufgerufen */
/*
inline void rtcInt(void)
{
  TCNT0=RTC_START_VAL;
  Time+=RTC_OVERFLOW_TIME;
}
*/
/*
void rtcInit(void)
{
	Time=0;
	// Counter Register
	TCNT0=RTC_START_VAL;
	// Overflow enable
	TIMSK |= _BV(TOIE0);
	// prescaler select F_CPU/1024
	TCCR0 = 0x04;
	// ergibt einen overflow alle 4 ms

}
*/
/* gibt die Zeit in ms zurück */
/*
t_Time rtcGetTime(void)
{
//  return Time+((RTC_START_VAL-TCNT0)*RTC_OVERFLOW_TIME/RTC_TICKS_TO_OVERFLOW);
  return Time;
}
*/
/*
t_Time rtcSetTime(t_Time t)
{
  return Time=t;
}
*/
void rtcDelay(t_Time ms)
{
	t_Time	t;
	t=Time+ms;
	while(t>=Time);
}
/*
// Einschaltverzögerung
unsigned char rtcEinschaltverz(unsigned char bedingung,t_Time verz,t_Time* timevar)
{
	if(!bedingung)
	{
		*timevar=0;
		return 0;
	}
	if(*timevar==0) *timevar=Time+verz;
	if(*timevar<=Time)
	{
		return -1;
	}
	return 0;
}
*/
/*
// Ausschaltverzögerung
unsigned char rtcAusschaltverz(unsigned char bedingung,t_Time verz,t_Time* timevar)
{
	if(bedingung)
	{
		*timevar=0;
		return -1;
	}
	if(*timevar==0) *timevar=rtcGetTime()+verz;
	if(*timevar<=rtcGetTime())
	{
		return 0;
	}
	return -1;
}
*/
// wird wenn "bedingung" "verz" ms wahr war für einen aufruf wahr
/*unsigned char rtcImpulsverz(unsigned char bedingung,t_Time verz,t_Time* timevar)
{
	if(!bedingung)
	{
		*timevar=0;
		return 0;
	}
	if(*timevar==0) *timevar=Time+verz;
	if(*timevar<=Time)
	{
		*timevar = 0;
		return -1;
	}
	return 0;
}
*/
