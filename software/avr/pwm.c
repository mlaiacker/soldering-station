/* 
* max-Lader 
* entwickelt von:
* Maximilian Laiacker post@mlaiacker.de http://mlaiacker.de/
*/


#include <avr/io.h>
//#include "global.h"
#include "pwm.h"
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif
#ifdef PWM_MAX_1A


void pwmInit1A(void){
  OCR1AH = 0; // highbyte setzen
  OCR1AL = 0;
#if (PWM_MAX_1A == 255)
  // Cleared on compare match, up-counting. Set on compare match, down-counting (non-inverted PWM). 8Bit FastPWM
//  TCCR1A = (1<<COM1A1|0<<COM1A0|1<<COM1B1|1<<COM1B0|0<<WGM11|1<<WGM10);
#elif (PWM_MAX_1A == 511)
  // Cleared on compare match, up-counting. Set on compare match, down-counting (non-inverted PWM). 9Bit FastPWM
  TCCR1A = (1<<COM1A1|0<<COM1A0|1<<COM1B1|1<<COM1B0|1<<WGM11|0<<WGM10);
#elif (PWM_MAX_1A == 1023)
  // Cleared on compare match, up-counting. Set on compare match, down-counting (non-inverted PWM). 10Bit FastPWM
  TCCR1A = (1<<COM1A1|0<<COM1A0|1<<COM1B1|1<<COM1B0|1<<WGM11|1<<WGM10);
#else
#error "PWM_MAX_1A value wrong"
#endif
// fTCK1 = CK 00000001
  // fast pwm
//  TCCR1B = (0<<WGM13|1<<WGM12|1<<CS10|0<<CS11|0<<CS12);
  // phase correct pwm
  TCCR1B = (0<<WGM13|0<<WGM12|1<<CS10|0<<CS11|0<<CS12);

  
  cbi(PORTB,1); // port = 0
  sbi(DDRB,1); // OC1B
}


/* 0..511 */
void pwmSet1A(signed short value)
{
	if (value > PWM_MAX_1A) value = PWM_MAX_1A;
	if (value < 0) value = 0;
	OCR1AL = value;
	OCR1AH = value>>8;
}

#endif
