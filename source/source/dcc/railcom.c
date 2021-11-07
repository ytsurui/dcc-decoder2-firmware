/*
 * railcom.c
 *
 * Created: 2021/01/25 9:41:22
 *  Author: Y.Tsurui
 */ 

#ifndef NO_RAILCOM

#include <avr/io.h>
#include <avr/pgmspace.h>

#include "../peripheral/uart.h"
#include "cv_value.h"

#include "../peripheral/motor.h"
#include "../peripheral/ABC_detector.h"

#include "../app/train_ctrl.h"


uint8_t railcomSendMode = 0;

const PROGMEM uint8_t encodeDataTable[64] = {	0xAC, 0xAA, 0xA9, 0xA5, 0xA3, 0xA6, 0x9C, 0x9A,
												0x99, 0x95, 0x93, 0x96, 0x8E, 0x8D, 0x8B, 0xB1,
												0xB2, 0xB4, 0xB8, 0x74, 0x72, 0x6C, 0x6A, 0x69,
												0x65, 0x63, 0x66, 0x5C, 0x5A, 0x59, 0x55, 0x53,
												0x56, 0x4E, 0x4D, 0x4B, 0x47, 0x71, 0xE8, 0xE4,
												0xE2, 0xD1, 0xC9, 0xC5, 0xD8, 0xD4, 0xD2, 0xC9,
												0xC6, 0xCC, 0x78, 0x17, 0x1B, 0x1D, 0x1E, 0x2E,
												0x36, 0x3A, 0x27, 0x2B, 0x2D, 0x35, 0x39, 0x33};

/*
void sendUartDecimalStub(uint8_t data) {
	sendUart(0x30 + data);
}

void sendUartDecimal(uint16_t decimal)
{
	uint16_t decimal2;
	uint8_t viewedFlag = 0;
	
	if (decimal >= 10000) {
		decimal2 = decimal / 10000;
		sendUartDecimalStub((uint8_t)(decimal2 & 0xFF));
		decimal = decimal % 10000;
		viewedFlag = 1;
	}
	
	if ((decimal >= 1000) || (viewedFlag != 0)) {
		decimal2 = decimal / 1000;
		sendUartDecimalStub((uint8_t)(decimal2 & 0xFF));
		decimal = decimal % 1000;
		viewedFlag = 1;
	}
	
	if ((decimal >= 100) || (viewedFlag != 0)) {
		decimal2 = decimal / 100;
		sendUartDecimalStub((uint8_t)(decimal2 & 0xFF));
		decimal = decimal % 100;
		viewedFlag = 1;
	}
	
	if ((decimal >= 10) || (viewedFlag != 0))  {
		decimal2 = decimal / 10;
		sendUartDecimalStub((uint8_t)(decimal2 & 0xFF));
		decimal = decimal % 10;
		viewedFlag = 1;
	}
	sendUartDecimalStub((uint8_t)(decimal & 0xFF));
}
*/

uint8_t convToASCII(uint8_t data) {
	if (data < 10) {
		return (0x30 + data);
	}
	if (data < 16) {
		return (0x41 + data - 10);
	}
	return (0x00);
}

void sendUartToASCII(uint8_t data) {
	uint8_t dataHigh;
	dataHigh = (data & 0xF0) >> 4;
	sendUart(convToASCII(dataHigh));
	sendUart(convToASCII(data & 0x0F));
}


uint8_t encodeData(uint8_t sixBitData)
{
	if (sixBitData > 0x3F) return (0);
	return (pgm_read_byte(&encodeDataTable[sixBitData]));
}


void channel1Send(uint8_t ID, uint8_t data)
{
	
	uint8_t data1, data2;
	
	data1 = encodeData((ID << 2) + ((data & 0xC0) >> 6));
	data2 = encodeData((data & 0x3F));
	
	//enableUART();
	//sendUart(0xF0);		// ACK
	sendUart(data1);
	sendUart(data2);
	
}

void railcomChannel1AddrSend() {
	
#ifndef ATTINY806_FUNC
	uint16_t bemfReadTemp;

	if (CV244 == 0x01) {
		bemfReadTemp = getBEMFvalue() >> 1;
		if (railcomSendMode == 0) {
			channel1Send(0x01, (uint8_t)((bemfReadTemp & 0xFF00) >> 8));
			railcomSendMode = 1;
		} else {
			channel1Send(0x02, (uint8_t)(bemfReadTemp & 0x00FF));
			railcomSendMode = 0;
		}
		return;
	} else if (CV244 == 0x02) {
		bemfReadTemp = getBEMFFixedSpdValue();
		if (railcomSendMode == 0) {
			channel1Send(0x01, (uint8_t)((bemfReadTemp & 0xFF00) >> 8));
			railcomSendMode = 1;
		} else {
			channel1Send(0x02, (uint8_t)(bemfReadTemp & 0x00FF));
			railcomSendMode = 0;
		}
		return;
	} else if (CV244 == 0x03) {
		if (railcomSendMode == 0) {
			channel1Send(0x01, 0x00);
			railcomSendMode = 1;
		} else {
			channel1Send(0x02, getSpdCache2());
			railcomSendMode = 0;
		}
		return;
	} else if (CV244 == 0x04) {
		bemfReadTemp = getCurrentValue();
		if (railcomSendMode == 0) {
			channel1Send(0x01, (uint8_t)((bemfReadTemp & 0xFF00) >> 8));
			railcomSendMode = 1;
		} else {
			channel1Send(0x02, (uint8_t)(bemfReadTemp & 0x00FF));
			railcomSendMode = 0;
		}
		return;
	} else if (CV244 == 0x05) {
		if (railcomSendMode == 0) {
			channel1Send(0x01, 0x00);
			railcomSendMode = 1;
			} else {
			channel1Send(0x02, getABCrightCount());
			railcomSendMode = 0;
		}
		return;
	} else if (CV244 == 0x06) {
		if (railcomSendMode == 0) {
			channel1Send(0x01, 0x00);
			railcomSendMode = 1;
		} else {
			channel1Send(0x02, getABCleftCount());
			railcomSendMode = 0;
		}
		return;
	}
#endif
	
	if ((CV29 & 0x08) == 0) return;
	
	if (CV19 != 0) {
		
		
	} else if ((CV29 & 0x20) == 0) {
		// 7-bit Addr
		if (railcomSendMode == 0) {
			channel1Send(0x01, 0x00);	// Addr High
			railcomSendMode = 1;
		} else {
			channel1Send(0x02, CV1_6[0]);	// Addr Low
			railcomSendMode = 0;
		}
	} else {
		// 14-bit Addr
		if (railcomSendMode == 0) {
			channel1Send(0x01, ((CV17 & 0x3F) + 0x80));	// Addr High
			railcomSendMode = 1;
		} else {
			channel1Send(0x02, CV18);		// Addr Low
			railcomSendMode = 0;
		}
	}
	
}

void railcomChannel2AddrSend() {
	
}

#endif
