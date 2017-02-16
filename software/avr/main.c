/*	 main.c
* max-Lader 
* entwickelt von:
* Maximilian Laiacker post@mlaiacker.de http://mlaiacker.de/
*
*/
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/wdt.h> 
#include <avr/eeprom.h>

#include "global.h"
#include "usart.h"
#include "a2d.h"
#include "lcd.h"
#include "pwm.h"
#include "rtc.h"

//#define LCD_2X16	1

//#define LCD_SHOW_TIMEOUT // show timout counter (only on 2x16 LCD)
// for control
#define GAIN_KP 60L
#define GAIN_KI 1L
// for user input
#define POTI_OFF	10L			// bis zu diesem Wert Heizung aus
#define POTI_TEMP_START	200L	// Beginn das Poti temperatur bereich (L damit es keinen 16bit integer overflow gibt bei 1024*200/1024)
#define POTI_TEMP_END	400L	// Ende des Poti Temeratur bereich (Hardware erlaubt ca. 415 max)

#define SOLDER_MAX 	4100	// degC*10 2000==200.0degC Heizung abschalten wenn gemessene Temperatur so hoch ist
#define SOLDER_TEMP_STANDBY	(2000) // degC*10 2000==200.0degC
// gain
#define TEMP_GAIN	(4980L)
#define TEMP_OFFSET	(-8)
// standby
#define SOLDER_TIMEOUT		600 // seconds
#define SOLDER_TIMEOUT_OFF 	(SOLDER_TIMEOUT*10)
#define SOLDER_TIMEOUT_PWM_DIFF		20	// max pwm deviation around 10 second mean
#define SOLDER_TIMEOUT_TEMP_ERROR	50	// max temp deviation around setpoint in deg/10
// max heating power
#define SOLDER_MAX_PWM	(PWM_MAX_1A*2/3)

#define MENU_DISPLAY_INT	50 //ms
#define INT_CONTROL			16 //ms


struct
{
	t_Time tSekunde,tDisplay,tControl;
	char blinken;
	char menu_on;
	char menu_item;
} maindata;

struct {
	signed short pwm;
	signed short pwm_mean;
	signed short temp_des;
	signed short temp;
	int error_signal,i;
	char error;
	char standby;
	short Tpwm, Tstandby, Toff;

	char state;
	char on;
	unsigned short poti, poti_old;
} solder;

// not used yet !!
typedef struct
{
	unsigned char pwm_max; // in prozent 10..99
	unsigned char poti_offset; // in adc ticks 0..1023
	unsigned char lcd_mode; // 0 = disable 1 = 1x20 2=2x16
	unsigned char control_freq; // temperature measurment freq 4..50
	unsigned short temp_low; // minimal temp for poti in deg c 1..SOLDER_MAX/10
	unsigned short temp_high; // max temp for poti 1..SOLDER_MAX/10
	unsigned short standby_timeout; // in seconds 0..6000 10<disable
	unsigned short standby_temp; // in deg c 1..temp_high
	unsigned short standby_fact; // in fact from 0..10 0=disable 5 = normal 10=big changes will reset standby counter
	short control_p; // control p gain 1..1000
	short control_i; // control i gain 0..100
	short control_d; // control d gain -1000..1000
	unsigned int checksum; //sum off all values  + 0xabcdef
} solder_param_t;

solder_param_t param;
solder_param_t param_ee EEPROM;

void uartOutput(void);
unsigned int paramChecksum(solder_param_t *param);

//-------------Initalisierung------------------
// da die init fuktionen sowiso nur ein mal im programm aufgerufen werden stehen sie hier "inline"
// ist zwar nicht so übersichtlich spart aber mindestens 4 byte pro init funktion
void init(void){
	#ifdef BEEP_ON
	BEEP_INIT;
	#endif
	
	#ifdef UART
// Serielle Schnittstelle intalisieren
	#if defined (__AVR_ATmega168__)
		UBRRL = (uint8_t)(F_CPU/(UART_BAUD_RATE*8L)-1);
		UBRRH = (F_CPU/(UART_BAUD_RATE*8L)-1) >> 8;
		UCSR0A = _BV(U2X0);
		UCSR0B = _BV(TXEN)|_BV(RXEN)|_BV(RXCIE);
                       // Receiver enabled, Transmitter enabled
                       // RX Complete interrupt enabled
	#elif defined (__AVR_ATmega8__)
		UBRRL = (uint8_t)(F_CPU/(UART_BAUD_RATE*8L)-1);
		UBRRH = (F_CPU/(UART_BAUD_RATE*8L)-1) >> 8;
		UCSRA = _BV(U2X);
		UCSRB = _BV(TXEN)|_BV(RXEN)|_BV(RXCIE); // rx, tx, rx int
		sbi(PORTD,0);// enable pull up on rx pin
	#endif

	#endif
// timer0 für Zeitmessung
	Time=0;
	// Counter Register
	TCNT0=RTC_START_VAL;
	// Overflow enable
	#ifdef TIMSK
	TIMSK |= _BV(TOIE0);
	#else
	TIMSK0 |= _BV(TOIE0);
	#endif
	#ifdef TCCR0
	// prescaler select F_CPU/256
	// ergibt einen overflow alle 16MHz/256/250 = 4 ms
#if RTC_PRESCALER == 256
	TCCR0 = 0x04;
#elif RTC_PRESCALER == 64
	TCCR0 = 0x03;
#endif
	#else
	TCCR0B = 0x04;
	#endif
	
	
	ADMUX = (0<<ADLAR) | (ADC_REFERENCE<<6);
	ADCSR = ((0<<ADFR)|(1<<ADEN)|(1<<ADIE)|(1<<ADSC)|(1<<ADIF) | ADC_PRESCALE);

	pwmInit1A();
  
	sei(); // interupt enable

	//TASTE_M_INIT;
	wdt_enable(WDTO_1S);

	eeprom_read_block(&param,&param_ee,sizeof(param));
	if(param.checksum != paramChecksum(&param))
	{
	 // defaults
		usartPrint("loading default params...\r\n");
		param.control_p = GAIN_KP;
		param.control_i = GAIN_KI;
		param.control_d = 0;
		param.control_freq = 10;
		param.poti_offset = 10;
		param.lcd_mode = 1;
		param.pwm_max = 75;
		param.standby_fact = 5;
		param.standby_temp = 200;
		param.standby_timeout = 600;
		param.temp_high = 400;
		param.temp_low = 200;
		param.checksum = paramChecksum(&param);
		eeprom_write_block(&param, &param_ee, sizeof(param));
	}

	usartPrint("# Max Solder Build"__DATE__);
	usartPrint("Version:"GIT_VERSION);

#ifdef LCD_CLR
	rtcDelay(500);
	wdt_reset();
	lcdInit();
	lcdGotoY(0);
	#if defined (__AVR_ATmega168__)
		lcdPrint("   MAX-Solder16  ");
	#elif defined (__AVR_ATmega8__)
		lcdPrint("   MAX-Solder    ");
	#endif
#ifdef LCD_2X16
	lcdGotoY(1);
	lcdPrint(__DATE__)
#endif
	wdt_reset();
	rtcDelay(500);
	wdt_reset();
	rtcDelay(500);
	wdt_reset();
/*	lcdGotoY(0);
	lcdPrint("Version:");
	lcdNum(PROG_VERSION,2,1);
	rtcDelay(500);
	wdt_reset();*/
#endif

}// init end

/*---------------- Functions -------------------------*/

unsigned int paramChecksum(solder_param_t *param)
{
	unsigned int result = 0xabcd,i;
	for(i=0;i<sizeof(*param)-sizeof(param->checksum);i++)
	{
		result += ((unsigned char*)param)[i];
	}
	return result;
}

#ifdef UART


void uartOutput(void)
{
	//usart_puts_prog(modeName[charger.mode]);
	//usartNum(Time/100,5,1); // Ladezeit
	usartNum(solder.temp,4,1); // Temp
	usartNum(solder.temp_des,4,1); // Ausgangsstrom
	usartNum(solder.pwm,3,0);
	usartNum(solder.Toff,4,0);
//	usartNum(solder.i,4,0);
	usartPutc('\r');
	usartPutc('\n');
}

void uartMenu(char key)
{
	usartPrint("Solder Menu:\r\n");
	usartPrint("## CONTROL ##\r\n");
	usartPrint("P=");usartNum(param.control_p,3,0);
	usartPrint("\r\nI=");usartNum(param.control_i,3,0);
	usartPrint("\r\nD=");usartNum(param.control_d,3,0);
	usartPrint("\r\ninterval=");usartNum(param.control_freq,3,0);
	usartPrint("\r\nexit:q\r\n");
	if(key=='q') maindata.menu_on = 0;
}

#endif
/*----------------------------------------------------*/

/*----------------Timer Interrupts--------------------*/
ISR(TIMER0_OVF_vect)
{
  TCNT0=RTC_START_VAL;
  Time+=RTC_OVERFLOW_TIME;
}
/*----------------------------------------------------*/

/*----------------Main Loop---------------------------*/
int main(void)
{
	init();
	// watchdog einschalten
	
	for(;;)
	{
		wdt_reset();
#ifdef UART
		if(usart_unread_data()!=0)
		{
			if(!maindata.menu_on)
			{
				if(usart_getc()=='m'){
					maindata.menu_on = 1;
					uartMenu(0);
				}
			} else
			{
				solder.on = 0;
				solder.standby = 1;
				uartMenu(usart_getc());
			}
		}
#endif
		if(maindata.tControl <= Time)
		{
			solder.state++;
			if(solder.state==1)
			{
				// heizung aus um Temperatur zu messen
				pwmSet1A(0);
			} else if (solder.state==4)
			{
				// Temperatur messen und Heizung immer noch aus.
				solder.temp =  ((a2dConvert10bit(ADC_CH_TEMP)*TEMP_GAIN/(ADC_MAX*1L) + TEMP_OFFSET) + solder.temp)/2;
				if(solder.temp>SOLDER_MAX)
				{
					solder.state = 1;
				}
			} else if (solder.state==5)
			{
				// Temperatur noch mal messen und Ergebnisse mitteln
				solder.temp =  ((a2dConvert10bit(ADC_CH_TEMP)*TEMP_GAIN/(ADC_MAX*1L) + TEMP_OFFSET) + solder.temp)/2;
				if(solder.temp>SOLDER_MAX)
				{
					solder.state = 1;
				}
			}
			if (solder.state>=5)
			{
				// Heizung wieder aktivieren
				pwmSet1A(solder.pwm);
			}
			if (solder.state > 16) solder.state = 0; // neue messung starten

			if(solder.on && solder.error == 0)
			{
				// PI Temperaturregelung
				solder.error_signal = (solder.temp_des - solder.temp);
				solder.pwm = (solder.i + solder.error_signal*GAIN_KP)/100L;
				// anti wind-up
				if(solder.pwm >= -SOLDER_MAX_PWM && solder.pwm < SOLDER_MAX_PWM)
				{
					// Integral anteil
					solder.i += solder.error_signal*GAIN_KI/10L;//*INT_CONTROL/1000L;
					if(solder.i<0) solder.i = 0;
				}
				// min und max abfangen
				if(solder.pwm>SOLDER_MAX_PWM) solder.pwm=SOLDER_MAX_PWM;
				if(solder.pwm<0) solder.pwm=0;
			} else
			{
				solder.pwm = 0;
				solder.i = 0;
			}

			maindata.tControl += INT_CONTROL;
		}
		if(maindata.tDisplay <= Time)
		{
			solder.poti  = (a2dConvert10bit(ADC_CH_POTI) + a2dConvert10bit(ADC_CH_POTI))/2;
			if(solder.poti<POTI_OFF)
			{
				// Lötstation ist an aber Heizung ist aus
				solder.on = 0;
				solder.temp_des = 0;
				solder.error = 0; // fehler zurücksetzen wenn poti auf 0
				solder.standby = 0;
			} else
			{
				if(solder.standby)
				{
					solder.temp_des = SOLDER_TEMP_STANDBY;
				} else
				{
					// Soll-Temperatur Berechnung aus Wert vom Poti
					solder.on = 1;
					solder.temp_des = (POTI_TEMP_START + (solder.poti-POTI_OFF)*(POTI_TEMP_END-POTI_TEMP_START)/(ADC_MAX-POTI_OFF))*10;
				}
			}
			lcdGotoY(0);
			// Ist-Temperatur
			lcdPrint("Temp:");
			lcdNum(solder.temp/10,3,0);
			lcdDataWrite(223);
			lcdPrint("C ");
			// Soll-Temperatur
			lcdNum(solder.temp_des/10,3,0);
			lcdDataWrite(223);
			lcdPrint("C ");
#ifdef LCD_2X16
			lcdGotoY(1); // goto 2. line
			lcdPrint(" PWM:"); // Den text nur wenn 2x16 LCD angeschlossen
#endif
			if(solder.error)
			{
				lcdPrint("ERR");
			} else
			{
				if(maindata.blinken && solder.standby)
				{
					// Standby Zustand anzeigen
					lcdPrint("STA");
				} else
				{
					// PWM in Prozent anzeigen 0..99
					lcdNum(MIN(99,solder.pwm*10/((SOLDER_MAX_PWM)/10)),2,0);
					lcdPrint("%");
				}
			}
#if defined(LCD_2X16) && defined(LCD_SHOW_TIMEOUT)
			lcdPrint(" T:");
			lcdNum(solder.Toff,5,0); // standby timeout counter [s]
#endif
			maindata.tDisplay += MENU_DISPLAY_INT;
		}
		if(maindata.tSekunde <= Time) // jede sekunde
		{
			if(solder.on)
			{
				if(einschaltverz(solder.pwm >= SOLDER_MAX_PWM, 20, &solder.Tpwm))
				{
					// bei zu lange volle power muss ein fehler vorliegen und die Heizung wird abgeschaltet
					solder.error = 1;
					solder.standby = 0;
				} else
				{
					solder.pwm_mean = (solder.pwm_mean*9 + solder.pwm)/10;
					/* Standby Bedingungen:
					 * 1: abs(solder.poti - solder.poti_old)<10 Soll Temperatur Einstellung ändert sich nicht.
					 * 2: (solder.pwm < solder.temp_des/65)  Der PWM wert zum heizen ist klein (wenn man den Lötkolben benutzt muss ja stärker geheizt werden als wenn er nur rum liegt)
					 * 3: (solder.error_signal < 50) Die ist Temperatur ist maximal 5 Grad C kleiner als die soll Temperatur.
					 */
//					char standby = (solder.pwm < solder.temp_des/65) && (solder.error_signal < 50) && (abs(solder.poti - solder.poti_old)<10);
					char standby = (abs(solder.pwm - solder.pwm_mean)<SOLDER_TIMEOUT_PWM_DIFF || solder.pwm<=1) && (solder.error_signal <= SOLDER_TIMEOUT_TEMP_ERROR) && (abs(solder.poti - solder.poti_old)<=11);
					// standby bedingung muss immer erfüllt sein
					solder.standby = einschaltverz(standby,	SOLDER_TIMEOUT, &solder.Tstandby);
					// heizung komplett abgestellt wenn man den Lötkolben vergisst abzuschalten.
					if(einschaltverz(standby,SOLDER_TIMEOUT_OFF, &solder.Toff)) solder.on = 0;
				}
				solder.poti_old = solder.poti; // Poti Stellung merken für standby timeout
			}
			maindata.tSekunde += 1000L;
			maindata.blinken = !maindata.blinken;
			#ifdef UART
				if(!maindata.menu_on) uartOutput();
			#endif
		}

	}// for(ever) 
}// main

