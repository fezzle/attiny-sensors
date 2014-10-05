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

void putsigned(int16_t val) {
	static int16_t NUMS[] = { 10000, 1000, 100, 10, 1 };
	if (val < 0) {
		uart_putc('-');
		val = -val;
	}
	
	uint8_t b;
	for (int8_t i=0; i<sizeof(NUMS)/sizeof(int16_t); i++) {
		if (val > NUMS[i]) {
			b = div(val, NUMS[i]);
			uart_putc('0' + b);
			val -= b;
		}
	}
}

extern int16_t ch6;
extern int16_t ch7;


int main(void) {
	_delay_ms(100);
	TIMSK = 0;
	DDRB = _BV(MOSI)|_BV(SCLK)|_BV(SS)|_BV(RESET); //BITS4(MOSI, SCLK, SS, RESET);
	init_uart();
	setup_micromag();
	//init_ppm();
	sei();
		
	while(1) {
        //TODO:: Please write your application code 
		//_delay_ms(500);
		for (uint8_t j=248; j!=0;) {
			j++;
			for (uint16_t i=1; i!=0;) {
				i++;
			}
		}
		
		ch6 = (resultvector[1] / (resultvector[0]>>7));
		ch7 = (resultvector[2] / (resultvector[1]>>7));
	 	
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
		
		//uart_puts(" x:");
		//putsigned(ch6);
		
		//uart_puts(" y:");
		//putsigned(ch7);
			
		uart_puts(" l:");
		puthex(loops);	
		
		uart_puts(" r:");
		puthex(micromag_reads);
		
		uart_putc('\n');
		
		loops++;
    }
}