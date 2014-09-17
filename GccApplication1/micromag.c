/*
** Timer0 based MicroMag SPI async communication
 */ 

/*
 * spi.c
 *
 * Created: 9/13/2014 9:28:06 PM
 *  Author: eric
 */ 
#include <avr/interrupt.h>
#include <avr/io.h>
#include "main.h"
#include "micromag.h"

#define PS2 6
#define PS1 5
#define PS0 4
#define AS1 1
#define AS0 0

enum { INITIATING, 
	START_RESET, 
	COMPLETE_RESET, 
	SPI_TX_INITIATING,
	SPI_TX_INPROGRESS,
	SPI_RX_WAIT,
	SPI_RX_INITIATING,
	SPI_RX_INPROGRESS,
	} state = INITIATING;

enum { X=1, Y=2, Z=3} axis;
volatile int16_t resultword;
volatile int16_t resultvector[3];
volatile uint8_t commandbyte;
volatile uint8_t spi_tx_bitcount;
volatile uint8_t spi_rx_bitcount;
volatile uint8_t micromag_reads;

/* number of tcnt0 overflows before next command is executed */
volatile uint8_t comm_delay;
volatile uint8_t comm_state;

uint8_t next_commandbyte() {
	micromag_reads++;
	axis = (axis + 1) & 0b11;
	if (!axis) {
		axis++;
	}
	return _BV(PS2)|_BV(PS1)|axis;
}

ISR(TIMER0_OVF_vect) {
	if (comm_delay != 0) {
		comm_delay--;
		return;
	}
	
	if (comm_state == INITIATING) {
		PORTB = PORTB & ~(_BV(SS)|_BV(RESET));
		comm_state = START_RESET;
	
	} else if (comm_state == START_RESET) {
		PORTB = PORTB | _BV(RESET);
		comm_state = COMPLETE_RESET;
	
	} else if (comm_state == COMPLETE_RESET) {
		PORTB = PORTB & ~_BV(RESET);
		comm_state = SPI_TX_INITIATING;
		spi_tx_bitcount = 0;
		commandbyte = next_commandbyte();
		
	} else if (comm_state == SPI_TX_INITIATING || comm_state == SPI_TX_INPROGRESS) {
		if (spi_tx_bitcount == 8) {
			comm_state = SPI_RX_WAIT;
			return;	
		}
		if (commandbyte & 0x80) {
			PORTB = PORTB | _BV(MOSI);
		} else {
			PORTB = PORTB & ~_BV(MOSI);
		}
		commandbyte = commandbyte << 1;
		spi_tx_bitcount++;
		
		comm_state = SPI_TX_INPROGRESS;
		
	} else if ((comm_state == SPI_RX_WAIT) && (PINB & _BV(DRDY))) {
		comm_state = SPI_RX_INITIATING;
	}
 }

// DISABLED, INITIATING, INPROGRESS, TEARDOWN
ISR(TIMER0_COMPA_vect) {
	if (comm_state == SPI_TX_INPROGRESS 
		|| comm_state == SPI_RX_INPROGRESS 
		|| comm_state == SPI_RX_INITIATING) {
			
		PORTB = PORTB | _BV(SCLK);
		
		if (comm_state == SPI_RX_INITIATING) {
			comm_state = SPI_RX_INPROGRESS;
			spi_rx_bitcount = 0;
		}
		
	} else {
		PORTB = PORTB & (~_BV(SCLK));
	}
}

ISR(TIMER0_COMPB_vect) {
	PORTB = PORTB & (~_BV(SCLK));

	if (comm_state == SPI_RX_INPROGRESS) {
		resultword = (resultword << 1) | ((PINB >> MISO) & 1);
		if (spi_rx_bitcount++ == 16) {
			resultvector[axis-X] = resultword;
			comm_state = START_RESET;
			comm_delay = 10;
		}
	}
}

void setup_micromag() {
	// enable interrupt on pcint3 (connected to DRDY)
	PCMSK = _BV(DRDY);
	GIMSK = _BV(PCIE);

	// enable timer0
	TCCR0B = _BV(CS01);
	TIMSK = _BV(TOIE0) | _BV(OCIE0A) | _BV(OCIE0B);

	// use Timer0_ovf to clock out UDR and CompareA,CompareB for toggling CLKC pin
	// TX will set pin on Timer0_ovf, RX will read pin on CompareB before clearing pin
	TCNT0 = 1;
	OCR0A = 256-10-128;
	OCR0B = 256-10;
	TIFR = TIFR | _BV(OCF0B) | _BV(OCF0A) | _BV(TOV0);	
	
	comm_state = INITIATING;
	comm_delay = 5;
}