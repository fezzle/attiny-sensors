/*
 * Implements generic buffer functionality.
 * 
 * Usage:

 #define RX_BUFFER_SIZE 28
 uint8_t *rxstruct[sizeof(buffer_t) + RX_BUFFER_SIZE - 1];
 buffer_t * RX_BUFFER = (buffer_t*)rxstruct;

 #define TX_BUFFER_SIZE 4
 uint8_t* txstruct[sizeof(buffer_t) + TX_BUFFER_SIZE - 1];
 buffer_t* TX_BUFFER = (buffer_t*)txstruct;
 */ 


#ifndef BUFFER_H
#define BUFFER_H

#include <stdint.h>

typedef struct buffer {
	uint8_t buffsize;
	uint8_t readpos;
	uint8_t writepos;
	uint8_t overflows;
	char buff[1];
} buffer_t;

buffer_t *buffer_init(buffer_t *b, uint8_t size);

uint8_t buffer_read(buffer_t *b);

void buffer_write(buffer_t *b, char c);

uint8_t buffer_isempty(buffer_t *b);

char buffer_getlastchar(buffer_t *b);

void buffer_write_int(buffer_t *b, uint16_t n);

void buffer_write_str(buffer_t *b, char *str, int maxlen);

char *buffer_read_str(buffer_t *b);

#endif