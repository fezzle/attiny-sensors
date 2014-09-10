/*
 * IncFile1.h
 *
 * Created: 9/1/2014 5:04:59 PM
 *  Author: eric
 */ 


#ifndef UART_H_
#define UART_H_

#include <stdio.h>

#define UART_NO_DATA 0x0100

// Initialize UART to 2400 baud with 8N1.
void init_uart(void);

// Send and receive functions, that run without ISRs
uint8_t receive_uart();
void send_uart(uint8_t c);

// Receive a single char or UART_NO_DATA, if nothing received
uint16_t uart_getc(void);
// Blocking call to receive a char
uint8_t uart_getc_wait(void);
int uart_getc_f(FILE *stream);

// Send a single char
void uart_putc(uint8_t c);
int uart_putc_f(char c, FILE *stream);

// Send a string
void uart_puts(const char *s);
// Send a PROGMEM string
void uart_puts_P(const char *s);

#endif /*UART_H_*/


