/*
 * main.c
 *
 *  Created on: 15 cze 2020
 *      Author: MIKE
 */


#include <avr/io.h>
#include <string.h>
#include <stdio.h>
#include "uart/uart.h"
#include "spi/spi.h"
#include "nrf24l01/nrf24l01.h"
#include "nrf24l01/nrf24l01_memory_map.h"

void parse_nrf_data(char *buff);

char nrf24l01_tx_buff[32];
char nrf24l01_rx_buff[200];
uint8_t init_status;
uint8_t status = 0xFF;
char print_buff[32];

int main(void) {
	DDRD |= (1<<7);
	PORTD |= (1<<7);
	DDRB |= (1<<0);
	PORTB &= ~(1<<0);
	uart_init(UBRR_VAL);
	sei();
	uart_putsn("RX NRF24L01");
	init_status = nrf24l01_init();
	nrf24l01_init_external_interrupt();
	nrf24l01_open_wrting_pipe(0xF0F0F0F0E101);
	nrf24l01_open_reading_pipe(1, 0xF0F0F0F0F101);
	nrf24l01_set_pa_level(RF24_PA_MAX);
	sprintf(print_buff, "STATUS\t\t = 0x%02x RX_DR=%x TX_DS=%x MAX_RT=%x RX_P_NO=%x TX_FULL=%x\r\n", status,
			(status & _BV(RX_DR))?1:0,
					(status & _BV(TX_DS))?1:0,
							(status & _BV(MAX_RT))?1:0,
									((status >> RX_P_NO) & 0x07),
									(status & _BV(TX_FULL))?1:0);
	uart_puts(print_buff);
	nrf24l01_print_reg_addr("TX_ADDR: ", TX_ADDR);
	nrf24l01_print_reg_addr("RX_ADDR: ", RX_ADDR_P1);
	nrf24l01_set_start_char('#');
	nrf24l01_set_end_char('$');
	nrf24l01_start_listening();
	nrf24l01_set_on_receive_long_package_callback(parse_nrf_data);

	while(1) {
		nrf24l01_on_received_long_package(nrf24l01_rx_buff);
	}
}


void parse_nrf_data(char *buff) {
	uart_puts("RX[: ");
	uint8_t len = strlen(buff);
	uart_putint(len, 10);
	uart_puts("]");
	uart_puts(buff);
	uart_putsn("");
	PORTB ^= (1<<0);
}
