/*
 * main.h
 *
 * Created: 9/1/2014 10:06:03 PM
 *  Author: eric
 */ 


#ifndef MAIN_H_
#define MAIN_H_

#define BIT(A) (A<<1)
#define BITS2(A, B) ((A<<1)|(B<<1))
#define BITS3(A, B, C) ((A<<1)|(B<<1)|(C<<1))
#define BITS4(A, B, C, D) (BITS2(A, B) | BITS2(C, D))
#define BITS5(A, B, C, D, E) (BITS2(A, B) | BITS3(C, D, E))

#define CLRBIT(A) (~(A<<1))

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#define BAUD 2400

#define FREQ 8000000
#define MYUBBR ((FREQ / (BAUD * 16L)) - 1)
#define TX_BUFFER_SIZE 24
#define RX_BUFFER_SIZE 0

// spi on portb
#define MOSI PB5
#define MISO PB6
#define SCLK PB7
#define SS PB4
#define DRDY PB3
#define RESET PB2
#define RESETBUTTON PB1

// ppm on portd
#define PPM_PIN PD2
#define CH5_PIN PD3
#define CH6_PIN PD4
#define CH7_PIN PD5
#define CH8_PIN PD6
 
#endif /* MAIN_H_ */