/* -----------------------------------------------------------------------
 * uart.c
 * baudrate 9600
 * adapted to ATtiny2313
 */

#include <inttypes.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "uart.h"
#include "main.h"

volatile static uint8_t tx_buffer[TX_BUFFER_SIZE];
volatile static uint8_t tx_head = 0;
volatile static uint8_t tx_tail = 0;

/*
 * init_uart
 */
void init_uart(void) {
  // set baud rate
  UBRRH = (uint8_t)(MYUBBR >> 8); 
  UBRRL = (uint8_t)(MYUBBR);
  // enable transmit
  UCSRB = (1 << TXEN);
  // set frame format
  UCSRC = (1 << USBS) | (3 << UCSZ0);	// asynchron 8n1
}


/*
 * uart_putc
 * Puts a single char. Will block until there is enough space in the
 * send buffer.
 */
void uart_putc(uint8_t c) {
  uint8_t tmp_head = (tx_head + 1) % TX_BUFFER_SIZE;
  // wait for space in buffer
  while (tmp_head == tx_tail) {
	;// return;
  }
  tx_buffer[tx_head] = c;
  tx_head = tmp_head;
  // enable uart data interrupt (send data)
  UCSRB |= (1<<UDRIE);
}


/*
 * uart_puts
 * Sends a string.
 */
void uart_puts(const char *s) {
  while (*s) {
    uart_putc(*s);
    s++;
  }
}


/*
 * ISR User Data Register Empty
 * Send a char out of buffer via UART. If sending is complete, the 
 * interrupt gets disabled.
 */
ISR(USART_UDRE_vect) {
  uint8_t tmp_tail = 0;
  if (tx_head != tx_tail) {
    tmp_tail = (tx_tail + 1) % TX_BUFFER_SIZE;
    UDR = tx_buffer[tx_tail];
    tx_tail = tmp_tail;
  }
  else {
    // disable this interrupt if nothing more to send
    UCSRB &= ~(1 << UDRIE);
  }
}


