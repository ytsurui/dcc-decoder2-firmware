/*
 * dcc_poller_tiny1606.c
 *
 * Created: 2021/02/03 22:20:22
 *  Author: Y.Tsurui
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

#include "uart.h"
#include "../dcc/railcom.h"


uint8_t portTestFlag = 0;

uint8_t oldInputPortStat = 0;		

uint16_t dccTimerRightStart = 0;
uint16_t dccTimerLeftStart = 0;
uint16_t dccTimerCounter = 0;
uint16_t dccTimerCounterLeft = 0;

uint8_t railcomCutoutFlag = 0;
uint16_t railcomCutoutCounter = 0;

uint8_t _preambleCount = 0;
uint8_t _dccPulseReceiveBit;

uint8_t _dccPulseBitCount;
uint8_t _dccPacketCount;
uint8_t _dccPacketError;

#define DCC_PACKET_LENGTH	32
uint8_t _dccPacketData[DCC_PACKET_LENGTH];

uint8_t dccRecvPacketCache[DCC_PACKET_LENGTH];
uint8_t dccRecvPacketCacheLength;
uint8_t dccRecvPacketCacheEnableFlag;


//uint8_t railcomSendMode = 0;
uint8_t railcomSendFlag = 0;

/* BaseClock = 20MHz/2 = 10MHz */
#define DCC_PULSE_LENGTH_ONE_MIN	220		//44us (counter 1cycle = 200ns @ 8MHz)
#define DCC_PULSE_LENGTH_ONE_MAX	450		//80us
#define DCC_PULSE_LENGTH_ZERO_MIN	500		//90us
#define DCC_PULSE_LENGTH_ZERO_MAX	50000	//10000us
//#define DCC_PULSE_LENGTH_ZERO_MAX	20000	//20000us

#define DCC_PULSE_LENGTH_RAILCOM_CUTOUT_MIN		130	// 22us
#define DCC_PULSE_LENGTH_RAILCOM_CUTOUT_MAX		190	// 32us
#define DCC_PULSE_LENGTH_RAILCOM_CHANNEL1_START	30	// 40us (32us + 8us)


//void sendPacketToUart(void);
//void packetTestRecv(void);
//void sendUartToASCII(uint8_t data);
//uint8_t convToASCII(uint8_t data);

//void sendUartDecimal(uint16_t decimal);
//void sendUartDecimalStub(uint8_t data);

/*
uint16_t analogCheckCounterRight = 0;
uint16_t analogCheckCounterLeft = 0;
#define ANALOG_COUNTER_MAX				1000
#define ANALOG_COUNTER_THRESHOLD		600
#define ANALOG_COUNTER_DECREASE_VALUE	5
*/

uint16_t dccTimeoutCounter = 0;
#define DCCTIMEOUT_COUNTER_MAX	300

uint16_t ABCpollerTimerLeft;
uint16_t ABCpollerTimerRight;
uint8_t ABCpollerExecFlag = 0;
#define ABC_POLLER_INTERVAL	200


ISR(PORTA_PORT_vect)
{
	PORTA.INTFLAGS |= (PIN2_bm | PIN4_bm);
	
	if (PORTA.IN & PIN2_bm) {		// RAIL+
		if ((oldInputPortStat & 0x01) == 0) {
			oldInputPortStat |= 0x01;
			
			if (dccTimerRightStart < TCB0.CNT) {
				dccTimerCounter = TCB0.CNT - dccTimerRightStart;
			} else {
				dccTimerCounter = 0xD000 - dccTimerRightStart + TCB0.CNT;
			}
			
			//if (analogCheckCounterRight > ANALOG_COUNTER_DECREASE_VALUE) analogCheckCounterRight -= ANALOG_COUNTER_DECREASE_VALUE;
			//else analogCheckCounterRight = 0;
			
			ABCpollerExecFlag |= 0x01;
			if ((0xD000 - ABC_POLLER_INTERVAL) < TCB0.CNT) {
				ABCpollerTimerRight = (TCB0.CNT + ABC_POLLER_INTERVAL) - 0xD000;
			} else {
				ABCpollerTimerRight = TCB0.CNT + ABC_POLLER_INTERVAL;
			}
			
		}
	} else {
		// Start Right Count
		if (oldInputPortStat & 0x01) {
			oldInputPortStat &= ~0x01;
			dccTimerRightStart = TCB0.CNT;
			ABCpollerExecFlag &= ~0x01;
		}
	}
	
	if (PORTA.IN & PIN4_bm) {		// RAIL-
		if ((oldInputPortStat & 0x02) == 0) {
			oldInputPortStat |= 0x02;
			
			if (dccTimerLeftStart < TCB0.CNT) {
				dccTimerCounterLeft = TCB0.CNT - dccTimerLeftStart;
			} else {
				dccTimerCounterLeft = 0xD000 - dccTimerLeftStart + TCB0.CNT;
			}
			
			//if (analogCheckCounterLeft > ANALOG_COUNTER_DECREASE_VALUE) analogCheckCounterLeft -= ANALOG_COUNTER_DECREASE_VALUE;
			//else analogCheckCounterLeft = 0;
			
			ABCpollerExecFlag |= 0x02;
			if ((0xD000 - ABC_POLLER_INTERVAL) < TCB0.CNT) {
				ABCpollerTimerLeft = (TCB0.CNT + ABC_POLLER_INTERVAL) - 0xD000;
			} else {
				ABCpollerTimerLeft = TCB0.CNT + ABC_POLLER_INTERVAL;
			}
			
		}
	} else {
		// Start Left Count
		if (oldInputPortStat & 0x02) {
			oldInputPortStat &= ~0x02;
			dccTimerLeftStart = TCB0.CNT;
			ABCpollerExecFlag &= ~0x02;
		}
	}
	
}


uint8_t ABCpollerReaderFlag(void) {
	if (ABCpollerExecFlag & 0x01) {
		if (TCB0.CNT > ABCpollerTimerRight) {
			ABCpollerExecFlag &= ~0x01;
			return (1);
		}
	}
	if (ABCpollerExecFlag & 0x02) {
		if (TCB0.CNT > ABCpollerTimerLeft) {
			ABCpollerExecFlag &= ~0x02;
			return (2);
		}
	}
	return (0);
}


void DCCpollerClkReceiver(void) {
	//if (analogCheckCounterRight < ANALOG_COUNTER_MAX) analogCheckCounterRight++;
	//if (analogCheckCounterLeft < ANALOG_COUNTER_MAX) analogCheckCounterLeft++;
	if (dccTimeoutCounter < DCCTIMEOUT_COUNTER_MAX) dccTimeoutCounter++;
}

uint8_t readAnalogStat(void)
{
	if (dccTimeoutCounter < DCCTIMEOUT_COUNTER_MAX) return (0);	// Digital Operation
	return (1);	// Analog Operation
}

/*
uint8_t readAnalogStat(void) {
	if ((analogCheckCounterRight > ANALOG_COUNTER_THRESHOLD) || (analogCheckCounterLeft > ANALOG_COUNTER_THRESHOLD)) {
		if (PORTA.IN & PIN2_bm) {
			// Analog, Forward
			return (1);
		} else if (PORTA.IN & PIN4_bm) {
			// Analog, Backward
			return (2);
		} else {
			if (~PORTA.IN & PIN2_bm) {
				// Backward
				return (2);
			} else if (~PORTA.IN & PIN4_bm) {
				// Forward
				return (1);
			}
		}
		return (0);
	}
	
	if (analogCheckCounterRight > ANALOG_COUNTER_THRESHOLD) {
		return (1);		// Analog, Forward
	} else if (analogCheckCounterLeft > ANALOG_COUNTER_THRESHOLD) {
		return (2);		// Analog, Backward
	}
	return (0);		// Digital
}
*/

void initDCCpoller(void)
{
	// Input: PA1 / PA2 (ATtiny202 Pin4/Pin5)
	//PORTA.DIRCLR = PIN1_bm | PIN2_bm;
	
	// Input: PA2 / PA4 (ATtiny1606-M Pin1/Pin5)
	PORTA.DIRCLR = PIN2_bm | PIN4_bm;
	
	PORTA.PIN2CTRL = PORT_ISC_BOTHEDGES_gc;
	PORTA.PIN4CTRL = PORT_ISC_BOTHEDGES_gc;

	// Max 6.55msec (1 / 5MHz * 32767, 1cycle=200ns)
	//TCB0.INTCTRL = TCB_CAPT_bm;
	//TCB0.CTRLA = TCB_CLKSEL_CLKDIV2_gc | TCB_ENABLE_bm | TCB_SYNCUPD_bm;
	TCB0.CTRLA = TCB_CLKSEL_CLKDIV2_gc | TCB_ENABLE_bm;
	TCB0.CCMP = 0xCFFF;
	
	//sei();
}

uint8_t checkRailcomCutout(uint8_t lengthRight, uint8_t lengthLeft)
{
	if ((lengthRight > DCC_PULSE_LENGTH_RAILCOM_CUTOUT_MIN) && (lengthRight < DCC_PULSE_LENGTH_RAILCOM_CUTOUT_MAX)) {
		return (1);
	}
	if ((lengthLeft > DCC_PULSE_LENGTH_RAILCOM_CUTOUT_MIN) && (lengthLeft < DCC_PULSE_LENGTH_RAILCOM_CUTOUT_MAX)) {
		return (1);
	}
	return (0);
}

//void dccPacketShifter(void)
void dccPacketShifter(uint8_t* recvPacketLength, uint8_t* recvPacket)
{
	uint16_t newCounter;
	
	uint16_t dccTimerTemp;
	uint16_t dccTimerTemp2;
	
	if (railcomCutoutFlag) {
		if (railcomCutoutCounter < TCB0.CNT) {
			newCounter = TCB0.CNT - railcomCutoutCounter;
		} else {
			newCounter = 0xD000 + TCB0.CNT - railcomCutoutCounter;
		}
		
		if (newCounter >= DCC_PULSE_LENGTH_RAILCOM_CHANNEL1_START) {
			/*
			if (railcomSendMode == 0) {
				channel1Send(0x01, 0x84);
				railcomSendMode = 1;
			} else if (railcomSendMode == 1) {
				channel1Send(0x02, 0xE7);
				railcomSendMode = 0;
			}
			*/
			railcomSendFlag |= 0x01;	// Channel1
			railcomCutoutFlag = 0;
		}
	}
	
	
	if ((dccTimerCounter == 0) && (dccTimerCounterLeft == 0)) return;
	dccTimerTemp = dccTimerCounterLeft;
	dccTimerTemp2 = dccTimerCounter;
	dccTimerCounter = 0;
	dccTimerCounterLeft = 0;
	
	//PORTA.OUTTGL = PIN3_bm;
	
	
	if (checkRailcomCutout(dccTimerTemp, dccTimerTemp2)) {
		if (railcomCutoutFlag == 0) {
			railcomCutoutFlag = 1;
			railcomCutoutCounter = TCB0.CNT;
		}
		
		return;
	}
	
	
	
	if ((dccTimerTemp > DCC_PULSE_LENGTH_ONE_MIN) && (dccTimerTemp < DCC_PULSE_LENGTH_ONE_MAX)) {
		// bit is 1
		_dccPulseReceiveBit = 1;
		_preambleCount++;
		
		_dccPacketData[_dccPacketCount] = (_dccPacketData[_dccPacketCount] << 1) + 1;
		
	} else if ((dccTimerTemp > DCC_PULSE_LENGTH_ZERO_MIN) && (dccTimerTemp < DCC_PULSE_LENGTH_ZERO_MAX)) {
		// bit is 0
		_dccPulseReceiveBit = 0;
		
		if (_preambleCount >= 14) {
			// Packet Start Bit
			_dccPulseBitCount = 1;
			_dccPacketData[0] = 0;
			_dccPacketCount = 0;
			_dccPacketError = 0;
			_preambleCount = 0;
			return;
		}
		
		_preambleCount = 0;
		_dccPacketData[_dccPacketCount] = (_dccPacketData[_dccPacketCount] << 1);
		
	} else {
		return;
	}
	

	if (_dccPacketCount >= DCC_PACKET_LENGTH) {	// Ignore
		return;
	}
	
	if ((_dccPulseBitCount == 0) && (_dccPulseReceiveBit == 1)) {
		// Packet End Bit
		if (_dccPacketError == 0) {
			
			//dccRecvPacketCacheLength = 0;
			//dccRecvPacketCacheEnableFlag = 0;
			
			//for (dccRecvPacketCacheLength = 0; dccRecvPacketCacheLength < _dccPacketCount; dccRecvPacketCacheLength++) {
			//	dccRecvPacketCache[dccRecvPacketCacheLength] = _dccPacketData[dccRecvPacketCacheLength];
			//}
			//dccRecvPacketCacheEnableFlag = 1;
			
			for ((*recvPacketLength) = 0; (*recvPacketLength) < _dccPacketCount; (*recvPacketLength)++) {
				recvPacket[(*recvPacketLength)] = _dccPacketData[(*recvPacketLength)];
			}
			
			//packetTestRecv();
			//sendPacketToUart();
			
			
			
		} else {
			(*recvPacketLength) = 0;
		}
	} else {
		(*recvPacketLength) = 0;
	}
	
	_dccPulseBitCount++;
	if (_dccPulseBitCount == 9) {
		// Byte End (1-8: Packet Pulse / 9 = end)
		if ((_dccPacketCount == 0) && (_dccPacketData[0] == 0xFF)) {
			// Preamble
			_dccPacketCount = 0xFF;
			return;
		}
		
		
		_dccPacketError ^= _dccPacketData[_dccPacketCount];
		_dccPacketCount++;
		_dccPacketData[_dccPacketCount] = 0;
		_dccPulseBitCount = 0;
		
		dccTimeoutCounter = 0;
	}
	
	
}


uint8_t checkRailcomSendChannel1(void)
{
	if (railcomSendFlag & 0x01) {
		railcomSendFlag &= ~0x01;
		return (1);
	}
	return (0);
}
