/*
 * ppm.c
 *
 * Created: 9/27/2014 7:26:13 PM
 *  Author: eric
 */ 
#include <inttypes.h>
#include <avr/interrupt.h>
#include <avr/io.h>

#include "ppm.h"

int16_t ch6;
int16_t ch7;
uint8_t currentchannel;

/* 
** High signal is marked from current point until Timer1 OVF
** Low signal is from Timer1 OVF to Compare A.
*/

ISR(TIMER1_COMPA_vect) {
	// set PPM_PIN for channel[currentchannel] time plus 500
	int16_t val=0;
	switch (currentchannel) {
		case 6:	val = ch6;
				PORTD = PORTD | _BV(CH6_PIN);
				break;
		case 7: val = ch7;
				PORTD = PORTD | _BV(CH7_PIN);
				break;
		case 8: val = 5000;
				break;
	}
	TCNT1 = -(1500 + val);
	PORTD = PORTD | _BV(PPM_PIN);
}

ISR(TIMER1_OVF_vect) {
	// set PPM_PIN for channel[currentchannel] time plus 500
	int8_t val=0;
	switch (currentchannel) {
		case 6:	PORTD = PORTD & ~_BV(CH6_PIN);
				break;
		case 7: PORTD = PORTD & ~_BV(CH7_PIN);
				break;
	}

	PORTD = PORTD & ~_BV(PPM_PIN);

	currentchannel++;
	if (currentchannel == 9) {
		currentchannel = 1;
	}
}


void init_ppm() {
	TCCR1B = _BV(CS11);
	OCR1A = 200;
	TIMSK = TIMSK | _BV(TOIE1) | _BV(OCIE1A);
	DDRD = DDRD | _BV(PPM_PIN) | _BV(CH5_PIN) | _BV(CH6_PIN) | _BV(CH7_PIN) | _BV(CH8_PIN);
}
