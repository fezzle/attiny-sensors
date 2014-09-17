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
#include "fxpt_atan2.h"

/*
int16_t rotx = 0;
int16_t roty = 0;
uint8_t current_axis = 0;
int16_t axisx = 0;
int16_t axisy = 0;
int16_t axisz = 0;
*/
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

extern uint8_t comm_state;
extern uint16_t resultvector[];
extern uint8_t micromag_reads;

uint8_t div(uint8_t num, uint8_t den) {
	uint8_t counter=0;
	while (num > den) {
		counter++;
		num -= den;
	}
	return counter;
}

void putsigned(int8_t val) {
	if (val < 0) {
		uart_putc('-');
		val = -val;
	}
	uint8_t b;
	if (val > 100) {
		b = div(val, 100);
		uart_putc('0' + b);
		val -= b;
	}
	if (val > 10) {
		b = div(val, 10);
		uart_putc('0' + b);
		val -= b;
	}
	uart_putc('0' + val);
}

#define Q 7
int8_t Q7_div(int8_t a, int8_t b) {
	int16_t temp;
	// pre-multiply by the base (Upscale to Q16 so that the result will be in Q8 format)
	temp = (int16_t)a << Q;
	// So the result will be rounded ; mid values are rounded up.
	temp += b/2;
	return temp/b;
}

int8_t Q7_atan2(int8_t x, int8_t y) {
	return 0;
}



int main(void) {
	_delay_ms(100);
	
	DDRB = _BV(MOSI)|_BV(SCLK)|_BV(SS)|_BV(RESET); //BITS4(MOSI, SCLK, SS, RESET);
	init_uart();
	sei();
	setup_micromag();
		
	while(1) {
        //TODO:: Please write your application code 
		//_delay_ms(500);
		for (uint8_t j=248; j!=0;) {
			j++;
			for (uint16_t i=1; i!=0;) {
				i++;
			}
		}
		
		int8_t x = fxpt_atan2(resultvector[1], resultvector[0]);
		int8_t y = fxpt_atan2(resultvector[2], resultvector[1]);

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
		uart_puts(" x:");
		putsigned(x);
		
		uart_puts(" y:");
		putsigned(y);
			
		uart_puts(" l:");
		puthex(loops);	
		
		uart_puts(" r:");
		puthex(micromag_reads);
		
		uart_putc('\n');
		
		loops++;
    }
}