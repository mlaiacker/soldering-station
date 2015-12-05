/* 
* max-Lader 
* entwickelt von:
* Maximilian Laiacker post@mlaiacker.de http://mlaiacker.de/
*/


#ifndef PWM_H
#define PWM_H
//#include "hardware.h"
#define PWM_MAX_1A 511

#ifdef PWM_MAX_1A
void pwmInit1A(void);
void pwmSet1A(signed short value);
unsigned short pwmGet1A(void);
#endif

#endif

