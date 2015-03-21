
/* 
* max-Lader 
* entwickelt von:
* Maximilian Laiacker post@mlaiacker.de http://mlaiacker.de/
*/

// migration mega8 -> mega168

#ifndef UBRRL


#define UCSRA UCSR0A
/* UCSR0A */
#define RXC 	RXC0    
#define TXC 	TXC0   
#define UDRE 	UDRE0   
#define FE 		FE0     
#define DOR	 	DOR0    
#define UPE 	UPE0    
#define U2X 	U2X0    
#define MPCM 	MPCM0   

#define UCSRB  UCSR0B
/* UCSR0B */
#define RXCIE RXCIE0  
#define TXCIE TXCIE0  
#define UDRIE UDRIE0  
#define RXEN RXEN0   
#define TXEN TXEN0   
#define UCSZ2 UCSZ02  
#define RXB8 RXB80   
#define TXB8 TXB80   

#define UCSRC UCSR0C
/* UCSR0C */
#define UMSEL1	UMSEL01  
#define UMSEL0	UMSEL00  
#define UPM1 UPM01    
#define UPM0 UPM00    
#define USBS USBS0    
#define UCSZ1 UCSZ01  
#define UDORD UDORD0   
#define UCSZ0 UCSZ00   
#define UCPHA UCPHA0   
#define UCPOL UCPOL0   

#define UBRR UBRR0  
#define UBRRL UBRR0L  
#define UBRRH UBRR0H 
#define UDR UDR0    



// eeprom

#define EEMWE	EEMPE   
#define EEWE EEPE   

#define SPMCR SPMCSR

#define TIMSK	TIMSK0
#endif
