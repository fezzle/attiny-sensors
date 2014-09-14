/*
 * spi.h
 *
 * Created: 9/13/2014 9:28:19 PM
 *  Author: eric
 */ 


#ifndef SPI_H_
#define SPI_H_

void spi_rx_start();

uint8_t spi_rx_len();

uint8_t spi_rx_isempty();

uint8_t spi_tx_isempty();

void spi_tx_push(uint8_t val);

uint8_t spi_tx_pop();

void spi_tx_start();

void spi_rx_push(uint8_t val);

#endif /* SPI_H_ */