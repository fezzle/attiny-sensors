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
#include "spi.h"

int16_t rotx = 0;
int16_t roty = 0;
uint8_t current_axis = 0;
int16_t axisx = 0;
int16_t axisy = 0;
int16_t axisz = 0;
uint8_t loops = 0;
uint8_t sends;


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

extern uint8_t spi_tx_count;

int main(void) {
	_delay_ms(100);
	
	DDRB = _BV(MOSI)|_BV(SCLK)|_BV(SS)|_BV(RESET); //BITS4(MOSI, SCLK, SS, RESET);
	init_uart();
	sei();
	
	// enable interrupt on pcint3 (connected to DRDY)
	PCMSK = _BV(DRDY);
	GIMSK = _BV(PCIE);
	
	// enable timer0
	TCCR0B = _BV(CS00);
	TIMSK = _BV(TOIE0) | _BV(OCIE0A) | _BV(OCIE0B);
	
	// Attiny2313 cannot drive SCLK pin from Timer0_ovf
	// so use Timer0_ovf to clock out UDR and Timer0-CompareA for toggling CLKC pin
	TCNT0 = 1;
	OCR0A = 64;
	OCR0B = 192;
	TIFR = TIFR | _BV(OCF0B) | _BV(OCF0A) | _BV(TOV0);
	loops=0;
	while(1) {
        //TODO:: Please write your application code 
		_delay_ms(50);
		
		uart_puts("spi_tx_count:");
		puthex(spi_tx_count);
		
		uart_puts(" TCNTO:");
		puthex(TCNT0);
		
		uart_puts(" s:");
		puthex(sends);
		
		uart_puts(" loops:");
		puthex(loops);
		uart_putc('\n');
		
		//PORTB = PORTB ^ 0xff;
		spi_tx_push('a');
		spi_tx_push('b');
		spi_tx_start();
		loops++;
    }
}