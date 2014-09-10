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
uint8_t sends;
uint8_t t0_ovf;

#define SPI_BUFF 4
uint8_t spi_tx_inprogress = FALSE;
uint8_t spi_tx_buff[SPI_BUFF];
uint8_t spi_tx_head = 0;
uint8_t spi_tx_tail = 0;
uint8_t spi_overflow = 0;

uint8_t spi_rx_expectedbytes = 0;
uint8_t spi_rx_buff[SPI_BUFF];
uint8_t spi_rx_head = 0;
uint8_t spi_rx_tail = 0;


ISR(PCINT_vect) {
	if (PINB & DRDY) {
		receiving = TRUE;
	}
}

ISR(TIMER0_COMPA_vect) {
	// set CLOCK bit
	if (spi_tx_inprogress || spi_rx_inprogress) {
		PORTB = PORTB | _BV(SCLK);
	}
}

ISR(TIMER0_COMPA_vect) {
	// clear CLOCK bit
	if (spi_tx_inprogress || spi_rx_inprogress) {
		PORTB = PORTB | _BV(SCLK);
	}
}

uint8_t spi_rx_len() {
	return (spi_rx_tail + SPI_BUFF - spi_rx_head) % SPI_BUFF;
}

uint8_t spi_tx_isempty() {
	return spi_tx_head == spi_tx_tail;
}

void spi_write(uint8_t val) {
	uint8_t next_tx_tail = (spi_tx_tail + 1) % SPI_BUFF;
	if (next_tx_tail == spi_tx_head) {
		spi_overflow++;
	} else {
		spi_tx_buff[spi_tx_head] = val;
		spi_tx_tail = next_tx_tail;
	}
}

uint8_t spi_write_pop() {
	if (spi_tx_isempty()) {
		return 0;
	}
	uint8_t val = spi_tx_buffer[spi_tx_head];
	spi_tx_head = (spi_tx_head + 1) % SPI_BUFF;
	return val;
}

void spi_start() {
	if (spi_tx_isempty()) {
		return;
	}
	spi_tx_inprogress = TRUE;
	
	// doesn't work as Attiny2313 cannot drive SCLK pin from Timer0_ovf
	// so use Timer0_ovf to clock out UDR and Timer0-CompareA for toggling CLKC pin
	TCNT0 = 1;
	OCR0A = 64;
	OCR0B = 192;
	TIFR = TIFR | _BV(0CF0B) | _BV(OCF0A) | _BV(TOV0);
	
	USIDR = spi_write_pop();
	USISR = _BV(USIOIF);
	USICR = _BV(USIWM0)|_BV(USICS0)|_BV(USICLK)|_BV(USIOIE);
}


ISR(TIMER0_OVF_vect) {
}


ISR(USI_OVERFLOW_vect) {
	// load next SPI send value
	if (spi_tx_inprogress) {
		if (spi_tx_isempty()) {
			spi_tx_inprogress = FALSE;
		} else {
			USIDR = spi_write_pop();
		}
	} else if (spi_rx_inprogress) {
		uint8_t next_rx_tail = (spi_rx_tail + 1) % (SPI_BUFF);
		if (next_rx_tail == spi_rx_head) {
			spi_overflow++;
		} else {
			spi_rx_buff[spi_rx_tail] = USIDR;
			spi_rx_	
		}
		
		USIDR = 
		spi_rx_head = (spi_rx_head + 1) % (SPI_BUFF);
	} else {
		spi_overflow++;
	}
}

void micromag_spi_setup() {
	// enable interrupt on pcint3 (connected to DRDY)
	PCMSK = BIT(DRDY); 
	GIMSK = BIT(PCIE);
	
	// enable timer0	
	TCCR0B = _BV(CS00);
	TIMSK = _BV(TOIE0) | _BV(OCIE0A) | _BV(OCIE0B);
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
		spi_write('a');
		spi_start();
		loops++;
    }
}