/*
 * spi.c
 *
 *  Created on: 6 maj 2020
 *      Author: MIKE
 *      based on SPI library by Julien Delvaux
 */

#include "spi.h"

volatile uint8_t spi_tx_buff[TX_BUFF_SIZE];
volatile uint8_t spi_tx_buff_mask;
volatile uint8_t spi_rx_buff[RX_BUFF_SIZE];
volatile uint8_t spi_rx_buff_mask;
volatile uint8_t cts;
volatile uint8_t bytes_request;
volatile uint8_t spi_tx_head;
volatile uint8_t spi_tx_tail;
volatile uint8_t spi_rx_head;
volatile uint8_t spi_rx_tail;
void (*tx_callback)();
void (*rx_callback)();
volatile uint8_t tx_flag = 0;
volatile uint8_t rx_flag = 0;


void spi_init(uint8_t mode, uint8_t clock) {
	// Pin Configuration
	SPI_DDR |= (1<<SPI_PIN_SS);
	SPI_PORT|= (1<<SPI_PIN_SS);

	cts	 = SPI_INACTIVE;
	// Set MOSI and SCK output, all others input
	SPI_DDR |= (1<<SPI_PIN_MOSI)|(1<<SPI_PIN_SCK);
	// Enable SPI, Master, set clock rate
	SPCR = (1<<SPE)|(1<<MSTR)|(mode<<CPHA)|(clock<<SPR0);
//	SPCR |= (1<<SPE);
//	SPCR |= (1<<MSTR);
//	SPCR |= (mode<<CPHA);
//	SPCR |= (clock<<SPR0);

}

//void spi_send_bytes(const char *str) {
//	uint16_t tmptail=0;
//
//	// Stores datas in buffer
//	while (*str) {
//		spi_putc(*str++, 0);
//	}
//
//	// Checks if ready to send and proceed
//	if(cts==SPI_INACTIVE){
//		cts=SPI_ACTIVE;
//		SPI_PORT &= ~(1<<SPI_PIN_SS); // Pull-down the line
//		if ( spi_tx_head != spi_tx_tail) {
//			tmptail = (spi_tx_tail + 1) & TX_BUFF_MASK;
//			spi_tx_tail = tmptail;
//			/* get one byte from buffer and write it to UART */
//			SPDR = spi_tx_buff[tmptail];  /* start transmission */
//		}
//	}
//}
//
//void spi_read_data(uint8_t bytes_count) {
//	bytes_request = bytes_count;
//	if (cts == SPI_INACTIVE) {
//		cts = SPI_ACTIVE;
//		SPI_PORT &= ~(1<<SPI_PIN_SS);
//		SPDR = 0x00;
//	}
//}
//
//void spi_close(void) {
//	spi_flush();
//	SPCR = (0x00);
//	SPI_DDR &= ~(1<<SPI_PIN_SS);
//	SPI_PORT &= ~(1<<SPI_PIN_SS);
//}
//
//uint8_t spi_getc() {
//	uint16_t temp_tail;
//	uint8_t data;
//
//	if (spi_rx_head == spi_rx_tail) {
//
//	}
//	temp_tail = (spi_rx_tail + 1) & RX_BUFF_MASK;
//	spi_rx_tail = temp_tail;
//	data = spi_rx_buff[temp_tail];
//	return data;
//}
//
//
//
//void spi_putc(uint8_t data, uint8_t mode) {
//	uint16_t temp_head;
//	if (mode == 0) {
//		temp_head = (spi_tx_head + 1) & TX_BUFF_MASK;
//		if (temp_head != spi_tx_tail) {
//			spi_tx_buff[temp_head] = data;
//			spi_tx_head = temp_head;
//		}
//	} else {
//		uint16_t tmptail=0;
//		temp_head = (spi_tx_head + 1) & TX_BUFF_MASK;
//		if (temp_head != spi_tx_tail) {
//			spi_tx_buff[temp_head] = data;
//			spi_tx_head = temp_head;
//		}
//		if(cts==SPI_INACTIVE){
//			cts=SPI_ACTIVE;
//			SPI_PORT &= ~(1<<SPI_PIN_SS); // Pull-down the line
//			if ( spi_tx_head != spi_tx_tail) {
//				tmptail = (spi_tx_tail + 1) & TX_BUFF_MASK;
//				spi_tx_tail = tmptail;
//				/* get one byte from buffer and write it to UART */
//				SPDR = spi_tx_buff[tmptail];  /* start transmission */
//			}
//		}
//
//	}
//}

uint8_t spi_transfer(uint8_t data) {
		SPDR = data;
		/*
		 * The following NOP introduces a small delay that can prevent the wait
		 * loop form iterating when running at the maximum speed. This gives
		 * about 10% more speed, even if it seems counter-intuitive. At lower
		 * speeds it is unnoticed.
		 */
		asm volatile("nop");
		while (!(SPSR & _BV(SPIF))) ; // wait

		return SPDR;
	}

//	void spi_puts(const char *str) {
//		while (*str) {
//			spi_putc(*str++, 0);
//		}
//	}
//
//	void spi_flush(void) {
//		spi_rx_head = spi_rx_tail;
//	}
//
//	uint8_t spi_available(void) {
//		return (RX_BUFF_SIZE + spi_rx_head - spi_rx_tail) & spi_rx_buff_mask;
//	}


//	ISR(SPI_STC_vect)
//	/*************************************************************************
//Function: SPI interrupt
//Purpose:  called when the SS pin has been put low
//	 **************************************************************************/
//	{
//		uint16_t temp_head=0;
//		uint16_t temp_tail=0;
//
//		//RECEIVE
//		// calculate buffer index
//		temp_head = ( spi_rx_head + 1) & RX_BUFF_MASK;
//		if ( temp_head == spi_rx_tail ) {
//			// error: receive buffer overflow
//
//		} else {
//			// store new index
//			spi_rx_head = temp_head;
//			// store received data in buffer
//			spi_rx_buff[temp_head] = SPDR;
//			rx_flag = 1;
//		}
//
//		// SEND
//		if ( spi_tx_head != spi_tx_tail) {
//			// calculate and store new buffer index
//			temp_tail = (spi_tx_tail + 1) & TX_BUFF_MASK;
//			spi_tx_tail = temp_tail;
//			// get one byte from buffer and write it to UART
//			SPDR = spi_tx_buff[temp_tail];  // start transmission
//			tx_flag = 1;
//		}
//		else if(bytes_request > 0){
//			bytes_request--;
//			SPDR = 0x00;
//		}
//		else {
//			// tx buffer empty, STOP the transmission
//			SPI_PORT|= (1<<SPI_PIN_SS);
//			cts = SPI_INACTIVE;
//		}
//	}
//
//	void spi_set_on_writed_data_callback(void (*callback)()) {
//		tx_callback = callback;
//		tx_flag = 0;
//	}
//
//	void spi_set_on_readed_data_callback(void (*callback)()) {
//		rx_callback = callback;
//		rx_flag = 0;
//	}
//
//	void spi_on_writed_data() {
//		(*tx_callback)();
//	}
//	void spi_on_readed_data() {
//		(*rx_callback)();
//	}
