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
extern int16_t resultvector[];
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

static const int16_t NUMS[] = { 10000, 1000, 100, 10, 1 };
void putsigned(int16_t val) {
	/*
	** Prints a 16-bit signed value using uart_putc.
	*/
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

int16_t ox, oy;
int16_t xy;
int16_t xy2;
int16_t hackatan(int16_t y, int16_t x) {
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
	while (x>256 || y>256) {
		x = x >> 1;
		y = y >> 1;	
	}
	ox = x;
	oy = y;
	if (!yneg && x==0) {
		return 90;
	} else if (yneg && x==0) {
		return -90;
	} else if (y==0) {
		if (!xneg) {
			return 0;
		} else if (yneg) {
			return -180;
		} else {
			return 180;
		}
	}
	
	
	if (y < x) {
		xy = (y * 100) / x;
	} else {
		flipped = TRUE;
		xy = (x * 100) / y;
	}
	
	xy2 = xy * xy;
	int16_t acc = -150;
	acc = acc + (int16_t)310 * xy;
	acc = acc - (xy2 / 2);
	acc = acc - (xy2 / 3);
	acc = acc / 50;
	acc = acc + 5;
	acc = acc / 10;

	if (flipped) {
		acc = 90 - acc;
	}
	if (xneg ^ yneg) {
		acc = -acc;
	}
	if (!yneg && xneg) {
		acc = acc + 180;
	} else if (yneg && xneg) {
		acc = acc - 180;
	} 
	return acc;
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
		
		ch6 = hackatan(resultvector[1], resultvector[0]);
		ch7 = hackatan(resultvector[2], resultvector[1]);
	 	
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
		
		#if TRUE
			uart_puts(" x:");
			//putsigned(resultvector[0]);
			putsigned(ox);

			uart_puts(" y:");
			//putsigned(resultvector[1]);
			putsigned(oy);
			
			uart_puts(" xy:");
			putsigned(xy);
			
			uart_puts( " xy^2:");
			putsigned(xy2);
			
			uart_puts(" 6:");
			putsigned(ch6);			
		# else 
		
			uart_puts(" z:");
			putsigned(resultvector[2]);
		
			uart_puts(" 7:");
			putsigned(ch7);
		# endif
			
		//uart_puts(" l:");
		//puthex(loops);	
		//uart_puts(" r:");
		//puthex(micromag_reads);
		
		uart_putc('\n');
		
		loops++;
    }
}