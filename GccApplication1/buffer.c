/*
 * CProgram1.c
 *
 * Created: 3/23/2012 1:43:56 AM
 *  Author: eric
 */ 

#include <avr/interrupt.h>

#include "buffer.h"


buffer_t *buffer_init(buffer_t *b, uint8_t size) {
	b->buffsize = size;
	b->writepos = b->readpos = b->overflows = 0;
	return b;
}

uint8_t buffer_read(buffer_t *b) {
    if (b->readpos == b->writepos) {
        return '?';
    } else {
		uint8_t val = b->buff[b->readpos];
        b->readpos = (b->readpos + 1) % (b->buffsize);
		return val;
    }
}

void buffer_write(buffer_t *b, char c) {
    if ((b->writepos + 1) % (b->buffsize) == b->readpos) {
        b->overflows++;
        return;
    } else {
        b->buff[b->writepos] = c;
        b->writepos = (b->writepos + 1) % (b->buffsize);
    } 
}

uint8_t buffer_isempty(buffer_t *b) {
	return b->readpos == b->writepos;
}

char buffer_getlastchar(buffer_t *b) {
	const uint8_t prev_wpos = (b->writepos + b->buffsize - 1) % (b->buffsize);
	return b->buff[prev_wpos];
}

void buffer_write_int(buffer_t *b, uint16_t n) {
	char buff[8];
	itoa(n, buff, 10);	
	buffer_write_str(b, buff, sizeof(buff));
}

void buffer_write_str(buffer_t *b, char *str, int maxlen) {
	while (maxlen--) {
		if (!*str) {
			break;
		}
		buffer_write(b, *str);
		str++;
	}
}

char *buffer_read_str(buffer_t *b) {
	/* returns str until first space or \n - must free returned ptr! */
	uint8_t size=0;
	uint8_t pos = b->readpos;
	while ((pos != b->writepos) && (b->buff[pos] != ' ' && b->buff[pos] != '\n')) {
		pos = (pos + 1) % (b->buffsize);
		size++;
	}
	
	char *res = malloc(size + 1);
	res[size] = 0;
	
	for (uint8_t i=0; size--; i++) {
		res[i] = buffer_read(b);
	}
    return res;
}