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

		uart_puts("s:");
		puthex(comm_state);
		
		uart_puts(" x:");
		puthex((uint8_t)(resultvector[0]>>8));
		puthex((uint8_t)resultvector[0]);
		
		uart_puts(" y:");
		puthex((uint8_t)(resultvector[1]>>8));
		puthex((uint8_t)resultvector[1]);

		uart_puts(" z:");
		puthex((uint8_t)(resultvector[2]>>8));
		puthex((uint8_t)resultvector[2]);
		
		uart_puts(" l:");
		puthex(loops);	
		
		uart_putc('\n');
		
		loops++;
    }
}