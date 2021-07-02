/*
 * usart0_tiny1606.c
 *
 * Created: 2021/02/03 22:25:28
 *  Author: Y.Tsurui
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "uart.h"

#define USART0_BAUD_RATE(BAUD_RATE) ((float)(10000000 * 64 / (16 * (float)BAUD_RATE)) + 0.5)
//#define USART0_BAUD_RATE(BAUD_RATE) ((float)(F_CPU * 64 / (16 * (float)BAUD_RATE)) + 0.5)

uint8_t dataBuf;

#define UART_DATABUF_SIZE	48
uint8_t dataBuf2[UART_DATABUF_SIZE];
uint8_t dataBuf2SendPos = 0;
uint8_t dataBuf2InsertPos = 0;

/*
ISR(USART0_RXC_vect)
{
	uint8_t recvData;
	
	USART0.STATUS |= USART_RXCIF_bm;
	USART0.RXDATAH |= USART_RXCIF_bm;	
	recvData = USART0.RXDATAL;
	
	
	//if (recvData & 0x80) return;
	sendUart(recvData);
}
*/

ISR(USART0_DRE_vect)
{
	USART0.STATUS |= USART_DREIF_bm;
	
	/*
	cli();
	USART0.TXDATAL = dataBuf;
	USART0.CTRLA &= ~USART_DREIE_bm;
	sei();
	*/

	//if (dataBuf2SendPos == dataBuf2InsertPos) {
	//	USART0.CTRLA &= ~USART_DREIE_bm;
	//	disableUART();
	//}
	
	cli();
	USART0.TXDATAL = dataBuf2[dataBuf2SendPos];
	dataBuf2SendPos++;
	if (dataBuf2SendPos >= UART_DATABUF_SIZE) dataBuf2SendPos = 0;
	if (dataBuf2SendPos == dataBuf2InsertPos) {
		USART0.CTRLA &= ~USART_DREIE_bm;
	}
	sei();
	
}

void initUART()
{
	//PORTA.DIRCLR = PIN7_bm;
	//PORTA.DIRSET = PIN6_bm;
	PORTB.DIRSET |= PIN2_bm;
	PORTB.OUTSET |= PIN2_bm;
	
	USART0.BAUD = (uint16_t)USART0_BAUD_RATE(250000);
	
	USART0.CTRLB = 0 << USART_MPCM_bp       // Multi-processor Communication Mode: disabled
		| 0 << USART_ODME_bp				// Open Drain Mode Enable: disabled
		| 0 << USART_RXEN_bp				// Receiver Enable: disabled
		| USART_RXMODE_NORMAL_gc			// Normal mode
		| 0 << USART_SFDEN_bp				// Start Frame Detection Enable: disabled
		| 1 << USART_TXEN_bp;				// Transmitter Enable: enabled
	
	//USART0.CTRLA |= USART_RXCIE_bm;
	
	USART0.CTRLC = USART_CHSIZE_8BIT_gc;
}

void enableUART()
{
	USART0.CTRLB = 0 << USART_MPCM_bp       // Multi-processor Communication Mode: disabled
		| 0 << USART_ODME_bp				// Open Drain Mode Enable: disabled
		| 0 << USART_RXEN_bp				// Receiver Enable: disabled
		| USART_RXMODE_NORMAL_gc			// Normal mode
		| 0 << USART_SFDEN_bp				// Start Frame Detection Enable: disabled
		| 1 << USART_TXEN_bp;				// Transmitter Enable: enabled
}

void disableUART()
{
	USART0.CTRLB = 0 << USART_MPCM_bp       // Multi-processor Communication Mode: disabled
		| 0 << USART_ODME_bp				// Open Drain Mode Enable: disabled
		| 0 << USART_RXEN_bp				// Receiver Enable: disabled
		| USART_RXMODE_NORMAL_gc			// Normal mode
		| 0 << USART_SFDEN_bp				// Start Frame Detection Enable: disabled
		| 0 << USART_TXEN_bp;				// Transmitter Enable: enabled
	
	PORTB.OUTSET |= PIN2_bm;
}

void sendUart(uint8_t data)
{
	/*
	dataBuf = data;
	USART0.CTRLA |= USART_DREIE_bm;
	sei();
	*/
	
	dataBuf2[dataBuf2InsertPos] = data;
	dataBuf2InsertPos++;
	if (dataBuf2InsertPos >= UART_DATABUF_SIZE) dataBuf2InsertPos = 0;
	USART0.CTRLA |= USART_DREIE_bm;
	sei();
	
}
