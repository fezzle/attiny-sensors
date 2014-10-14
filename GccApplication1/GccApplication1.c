/*
 * GccApplication1.c
 *
 * Created: 9/1/2014 2:37:32 PM
 *  Author: eric
 */ 

#define F_CPU 8000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "uart.h"
#include "main.h"
#include "micromag.h"
#include "ppm.h"


extern uint8_t comm_state;
extern uint16_t resultvector[];
extern uint8_t micromag_reads;
extern int16_t ch6;
extern int16_t ch7;

uint8_t loops = 0;

void putnibble(uint8_t val) {
	if (val >= 10) {
		uart_putc('a' - 10 + val);
	} else {
		uart_putc('0' + val);
	}
}

void puthex(uint8_t val) {
	putnibble(val >> 4);
	putnibble(val & 0xf);
}

void putsigned(int16_t val) {
	/*
	** Prints a 16-bit signed value using uart_putc.
	*/
	static int16_t NUMS[] = { 10000, 1000, 100, 10, 1 };
	if (val < 0) {
		uart_putc('-');
		val = -val;
	}
	char started = FALSE;
	for (int8_t i=0; i<sizeof(NUMS)/sizeof(int16_t); i++) {
		uint8_t quot = 0;
		uint16_t den = NUMS[i];
		while (val > den) {
			val -= den;
			quot++;
		}
		if (started || quot || den==1) {
			started = TRUE;
			uart_putc('0' + quot);
		}
	}
}


uint16_t hackatan(int16_t y, int16_t x) {
	// x is percentage of angle [0,100] 
	// y is angle 0..45
	// y ={ [-150 + 310 * x - (x*x DIV 2) - (x*x DIV 3) ] DIV 50 + 5 } DIV 10
	char flipped = FALSE;
	char xneg = FALSE;
	char yneg = FALSE;
	if (x < 0) {
		xneg = TRUE;
		x = -x;
	}
	if (y < 0) {
		yneg = TRUE;
		y = -y;
	}
	
	if (x < y) {
		x = (y >> 7) * 100 / (x>>7);
	} else {
		flipped = TRUE;
		x = (x >> 7) * 100 / (y>>7);
	}
	
	uint16_t x2 = x * x;
	int16_t res = ((-150 + (310 * x) - (x2 / 3) - (x2 / 3)) / 50 + 5) / 10;

	if (flipped) {
		res = 90 - res;
	}
	if (xneg ^ yneg) {
		res = -res;
	}
	if (!yneg && xneg) {
		res = res + 90;
	} else if (yneg && xneg) {
		res = res - 90;
	} else if (!yneg && xneg==0) {
		res = 45;
	} else if (yneg && xneg==0) {
		res = -45;
	} else {
		res = 0;
	}
	return res;
}


int main(void) {
	_delay_ms(100);
	TIMSK = 0;
	DDRD = 0;
	DDRB = _BV(MOSI)|_BV(SCLK)|_BV(SS)|_BV(RESET); //BITS4(MOSI, SCLK, SS, RESET);
	init_uart();
	setup_micromag();
	init_ppm();
	sei();
		
	while(1) {
        //TODO:: Please write your application code 
		_delay_ms(500);
		
		ch6 = hackatan(resultvector[1], (resultvector[0]>>7));
		ch7 = hackatan(resultvector[2], (resultvector[1]>>7));
	 	
		uart_puts("s:");
		puthex(comm_state);
		/*
		uart_puts(" x:");
		puthex((uint8_t)(resultvector[0]>>8));
		puthex((uint8_t)resultvector[0]);
		
		uart_puts(" y:");
		puthex((uint8_t)(resultvector[1]>>8));
		puthex((uint8_t)resultvector[1]);

		uart_puts(" z:");
		puthex((uint8_t)(resultvector[2]>>8));
		puthex((uint8_t)resultvector[2]);
		*/
		
		#if FALSE
			
			uart_puts(" x:");
			putsigned(resultvector[0]);

			uart_puts(" y:");
			putsigned(resultvector[1]);
			
			uart_puts(" z:");
			putsigned(resultvector[2]);
		# else 
			uart_puts(" x:");
			putsigned(ch6);
		
			uart_puts(" y:");
			putsigned(ch7);
		# endif
			
		uart_puts(" l:");
		puthex(loops);	
		
		uart_puts(" r:");
		puthex(micromag_reads);
		
		uart_putc('\n');
		
		loops++;
    }
}