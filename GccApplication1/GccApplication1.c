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

#define MOSI PB5
#define MISO PB6
#define SCLK PB7
#define SS PB4
#define DRDY PB3
#define RESET PB2

int16_t rotx = 0;
int16_t roty = 0;
uint8_t current_axis = 0;
int16_t axisx = 0;
int16_t axisy = 0;
int16_t axisz = 0;
uint8_t receiving = FALSE;
uint8_t loops = 0;
ISR(PCINT_vect) {
	if (PINB & DRDY) {
		receiving = TRUE;
	}
}

uint8_t sends;
uint8_t t0_ovf;

uint8_t spi_send_buff[2];
uint8_t spi_send_pos=0;


ISR(USI_OVERFLOW_vect) {
	// load next SPI send value
	if (spi_send_pos < SIZEOF(spi_send_buff)) {
		USIDR = spi_send_buff[spi_send_pos++];
	} else {
		spi_send_enabled = FALSE;
	}
}

ISR(TIMER0_COMPA_vect) {
	
}

ISR(TIMER0_OVF_vect) {
	if (spi_send_enabled) {
		USICR = (1<<USIWM0)|(1<<USICS1)|(1<<USICLK)|(1<<USITC);
	}
}



void spi_send_hw(uint8_t val) {
	// doesn't work as Attiny2313 cannot drive SCLK pin from Timer0_ovf
	TCNT0 = 0;
	USIDR = val;
	USISR = _BV(USIOIF);
	USICR = _BV(USIWM0)|_BV(USICS0)|_BV(USICLK)|_BV(USIOIE);
}

void spi_send(uint8_t val) {
	USIDR = val;
	USISR = (1<<USIOIF);
	do {
		USICR = (1<<USIWM0)|(1<<USICS1)|(1<<USICLK)|(1<<USITC)|(1<<USIOIE);
	} while ((USISR & (1<<USIOIF)) != 0);
}

void micromag_spi_setup() {
	// enable interrupt on pcint3 (connected to DRDY)
	PCMSK = BIT(DRDY); 
	GIMSK = BIT(PCIE);
	
	// enable timer0	
	TCCR0B = _BV(CS00);
	TIMSK = _BV(TOIE0);
}


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
	micromag_spi_setup();
	init_uart();
	sei();
	while(1) {
        //TODO:: Please write your application code 
		_delay_ms(1000);
		uart_puts("USISR:");
		puthex(USISR);
		
		uart_puts(" TCNTO:");
		puthex(TCNT0);
		
		uart_puts(" s:");
		puthex(sends);
		
		uart_puts(" loops:");
		puthex(loops);
		
		uart_puts(" t0_ovf:");
		puthex(t0_ovf);
		uart_putc('\n');
		
		//PORTB = PORTB ^ 0xff;
		spi_send('a');
		loops++;
    }
}