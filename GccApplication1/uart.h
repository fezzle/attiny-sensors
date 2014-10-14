/*
 * IncFile1.h
 *
 * Created: 9/1/2014 5:04:59 PM
 *  Author: eric
 */ 


#ifndef UART_H_
#define UART_H_

#include <stdio.h>

void init_uart(void);
void uart_putc(uint8_t c);
void uart_puts(const char *s);

#endif /*UART_H_*/


