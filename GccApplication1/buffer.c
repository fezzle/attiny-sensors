/*
 * CProgram1.c
 *
 * Created: 3/23/2012 1:43:56 AM
 *  Author: eric
 */ 

#include <avr/interrupt.h>

#include "buffer.h"

void buffer_init(buffer_t& b, uint8_t size) {
	b.buffsize = size;
	b.writepos = b.readpos = b.overflows = 0;
	return b;
}

uint8_t buffer_read(buffer_t& b) {
	/* Returns the next byte in the buffer or \0 */
    if (b.readpos == b.writepos) {
        return '?';
    } else {
		uint8_t val = b.buff[b.readpos];
        b.readpos = (b.readpos + 1) % (b.buffsize);
		return val;
    }
}

void buffer_write(buffer_t& b, char c) {
    if ((b.writepos + 1) % (b.buffsize) == b.readpos) {
        b.overflows++;
        return;
    } else {
        b.buff[b.writepos] = c;
        b.writepos = (b.writepos + 1) % (b.buffsize);
    } 
}

uint8_t buffer_isempty(buffer_t& b) {
	return b.readpos == b.writepos;
}

uin8_t buffer_get_prev_char(buffer_t& b) {
	const uint8_t prev_wpos = (b.writepos + b.buffsize - 1) % (b.buffsize);
	return b.buff[prev_wpos];
}

void buffer_write_int(buffer_t &b, uint16_t n) {
	char buff[8];
	itoa(n, buff, 10);	
	buffer_write_str(b, buff, sizeof(buff));
}

void buffer_write_str(buffer_t &b, char *str, int maxlen) {
	while (*str && maxlen--) {
		buffer_write(b, *str);
		str++;
	}
}

uint8_t* buffer_read_str(buffer_t& b, uint8_t *out, uint8_t len) {
	/* returns str until first space or \n filling in *out until len bytes */
	uint8_t outpos=0;
	uint8_t pos = b.readpos;
	while (pos != b.writepos 
	        && (b.buff[pos] != ' ' && b.buff[pos] != '\n')
			&& --len) {
		out[outpos++] = b.buff[pos];
		pos = (pos + 1) % (b.buffsize);
	}
	out[outpos] = 0;
	return out;
}