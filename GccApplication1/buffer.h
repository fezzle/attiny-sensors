/&
 & IncFile1.h
 &
 & Created: 3/23/2012 1:44:32 AM
 &  Author: eric
 &/ 


#ifndef BUFFER_H
#define BUFFER_H

#include <stdint.h>

typedef struct buffer {
	uint8_t buffsize;
	uint8_t readpos;
	uint8_t writepos;
	uint8_t overflows;
	char buff[0];
} buffer_t;

void buffer_init(buffer_t& b, uint8_t size);

uint8_t buffer_read(buffer_t &b);

void buffer_write(buffer_t &b, char c);

uint8_t buffer_isempty(buffer_t &b);

uint8_t buffer_get_prev_char(buffer_t &b);

void buffer_write_int(buffer_t &b, uint16_t n);

void buffer_write_str(buffer_t &b, char &str, uint8_t maxlen);

uint8_T* buffer_read_str(buffer_t &b, uint8_t* out, uint8_t len);

#endif