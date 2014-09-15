/*
 * spi.c
 *
 * Created: 9/13/2014 9:28:06 PM
 *  Author: eric
 */ 
#include <avr/interrupt.h>
#include <avr/io.h>
#include "main.h"
#include "spi.h"

uint8_t spi_tx_byte = 0;
uint8_t spi_tx_bitcount = 0;
uint8_t spi_tx_state = DISABLED;
uint8_t spi_tx_buff[SPI_TX_BUFF];
uint8_t spi_tx_head = 0;
uint8_t spi_tx_tail = 0;
uint8_t spi_overflow = 0;
uint8_t spi_tx_count = 0;

uint8_t spi_rx_byte = 0;
uint8_t spi_rx_bitcount = 0;
uint8_t spi_rx_state = FALSE;
uint8_t spi_rx_expectedbytes = 0;
uint8_t spi_rx_buff[SPI_RX_BUFF];
uint8_t spi_rx_head = 0;
uint8_t spi_rx_tail = 0;


ISR(PCINT_vect) {
	//spi_rx_state = TRUE;
}


ISR(TIMER0_OVF_vect) {
	// set CLOCK bit
	if (spi_tx_state == INPROGRESS || spi_tx_state == INITIATING) {
		if (spi_tx_bitcount == 8) {
			if (spi_tx_isempty()) {
				spi_tx_state = TEARDOWN;
				return;
			}
			spi_tx_count++;
			spi_tx_bitcount = 0;
			spi_tx_byte = spi_tx_pop();
		}
		if (spi_tx_byte & 0x80) {
			PORTB = PORTB | _BV(MOSI);
		} else {
			PORTB = PORTB & ~_BV(MOSI);
		}
		spi_tx_byte = spi_tx_byte << 1;
		spi_tx_bitcount++;
		
		spi_tx_state = INPROGRESS;
	} 

 }

// DISABLED, INITIATING, INPROGRESS, TEARDOWN
ISR(TIMER0_COMPA_vect) {
	if (spi_tx_state == INPROGRESS || spi_tx_state == TEARDOWN) {
		PORTB = PORTB | _BV(SCLK);
	} else {
		PORTB = PORTB & (~_BV(SCLK));
	}
	
	if (spi_rx_state == INITIATING) {
		spi_rx_state = INPROGRESS;
	}
	
}

ISR(TIMER0_COMPB_vect) {
   if (spi_rx_state == INPROGRESS) {
	   if (spi_rx_bitcount == 8) {
		   if (spi_rx_len() == spi_rx_expectedbytes - 1) {
			   spi_rx_state = DISABLED;
			   return;
		   }
		   spi_rx_bitcount = 0;
		   spi_rx_push(spi_rx_byte);
	   }
	   spi_rx_byte = (spi_rx_byte << 1) | ((PINB >> MISO) & 1);
	   spi_rx_bitcount++;
   }
	
	// clear CLOCK bit
	// something is in progress or in teardown
	PORTB = PORTB & (~_BV(SCLK));

	if (spi_tx_state == TEARDOWN) {
		spi_tx_state = DISABLED;
	}
}


uint8_t spi_rx_len() {
	return (spi_rx_tail + SPI_RX_BUFF - spi_rx_head) % SPI_RX_BUFF;
}

void spi_rx_push(uint8_t val) {
	uint8_t next_rx_tail = (spi_rx_tail + 1) % (SPI_RX_BUFF);
	if (next_rx_tail == spi_rx_head) {
		spi_overflow++;
	} else {
		spi_rx_buff[next_rx_tail]	= val;
		spi_rx_tail = next_rx_tail;
	}
}

uint8_t spi_rx_isempty() {
	return spi_rx_head == spi_rx_tail;
}

uint8_t spi_tx_isempty() {
	return spi_tx_head == spi_tx_tail;
}

void spi_tx_push(uint8_t val) {
	uint8_t next_tx_tail = (spi_tx_tail + 1) % SPI_TX_BUFF;
	if (next_tx_tail == spi_tx_head) {
		spi_overflow++;
	} else {
		spi_tx_buff[spi_tx_tail] = val;
		spi_tx_tail = next_tx_tail;
	}
}

uint8_t spi_tx_pop() {
	/* pops char from tx buffer */
	if (spi_tx_isempty()) {
		spi_overflow++;
		return 0;
	}
	uint8_t val = spi_tx_buff[spi_tx_head];
	spi_tx_head = (spi_tx_head + 1) % SPI_TX_BUFF;
	return val;
}

void spi_tx_start() {
	if (spi_tx_isempty()) {
		return;
	}
	if (spi_tx_state == DISABLED) {
		spi_tx_bitcount = 8;
		spi_tx_state = INITIATING;
	}
}
