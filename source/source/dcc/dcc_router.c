/*
 * dcc_router.c
 *
 * Created: 2021/01/25 19:49:46
 *  Author: Y.Tsurui
 */ 

#include <avr/io.h>

#include "cv_value.h"
#include "cv.h"
#include "../app/train_ctrl.h"
#include "../app/func_ctrl.h"

#include "dcc_prog_mode.h"


uint8_t serviceModeFlag = 0;


// Normal Operation Variables
uint8_t consistFlag = 0;
uint8_t speed128stepCache = 0xFF;
uint8_t speed28stepCache = 0xFF;
uint8_t directionFlag = 0;

// Operation Prod Mode Variables
uint8_t opsModeCache[3];
uint8_t opsModeAutoResetFlag = 0;
#define OPS_MODE_RECV_COUNT_MAX 100


void dccNormalOperation(uint8_t packetLength, uint8_t packetData[]);


void dccPacketRouter(uint8_t packetLength, uint8_t packetData[])
{
	
	// 2-Byte Packet (None)
	if (packetLength == 2) return;
	
	if (serviceModeFlag && ((packetData[0] & 0x70) == 0x70)) {
		// DCC Service Mode
		serviceModeFlag = 30;
		
		switch (packetLength) {
			case 3:
				if ((packetData[0] == 0) && (packetData[1] == 0) && (packetData[2] == 0)) {
					// Reset
					serviceModeFlag = 30;
					return;
				} else if ((packetData[0] == 0xFF) && (packetData[1] == 0) && (packetData[2] == 0xFF)) {
					// Idle Packet
					return;
				} else {
					// Paged Mode, Phys Mode, Address Only Mode
					dccProgPageMode(packetLength, packetData);
				}
				break;
			case 4:
				// Direct-Mode
				dccProgDirectMode(packetLength, packetData);
				break;
		}
	} else {
		dccNormalOperation(packetLength, packetData);
	}
	
	
}

void dccRouterClockReceiver()
{
	if (serviceModeFlag) {
		serviceModeFlag--;
		if (serviceModeFlag == 0) {
			
		}
		
		dccProgModeClockReceiver();
	}
	
}


void dccNormalOperation(uint8_t packetLength, uint8_t packetData[])
{
	uint8_t packetReadStartPos = 0;
	uint8_t packetInstruction;
	uint8_t spdCache;
	uint8_t i;
		
	if (packetLength == 3) {
		if ((packetData[0] == 0) && (packetData[1] == 0) && (packetData[2] == 0)) {
			// Reset
			serviceModeFlag = 30;
			resetPageFlag();
			return;
		} else if ((packetData[0] == 0xFF) && (packetData[1] == 0x00) && (packetData[2] == 0xFF)) {
			// Idle, Exit Service-Mode
			serviceModeFlag = 0;
			return;
		}
	}
	
	if (CV19 != 0) {
		// Consist Mode
		if (((packetData[0] & 0x80) == 0) && ((packetData[1] & 0xE0) != 0xE0)) {
			consistFlag = CV19;
		}
	}
	
	if (consistFlag) {
		if ((packetData[0]) && (consistFlag == (packetData[0] & 0x7F))) {
			// 7-Bit Addr Consist Mode
			packetReadStartPos = 1;
		} else {
			return;
		}
	} else if (((packetData[0] & 0x80) == 0) && (CV29 & 0x20) == 0) {
		// 7-Bit Addr
		if (packetData[0] != CV1_6[0]) {
			return;
		}
		packetReadStartPos = 1;
	} else if (((packetData[0] & 0xC0) == 0xC0) && (CV29 & 0x20)) {
		// 14-Bit Addr
		if ((packetData[0] != CV17) || (packetData[1] != CV18)) {
			return;
		}
		packetReadStartPos = 2;
	} else {
		// Not for Mobile Decoder Packet
		return;
	}
	
	packetInstruction = packetData[packetReadStartPos] & 0xE0;
	
	switch (packetInstruction) {
		case 0x00:
			break;
		case 0x20:
			
			if (packetLength != (packetReadStartPos + 3)) {
				return;
			}
			
			
			if ((packetData[packetReadStartPos] & 0x1F) != 0x1F) return;
			
			// Advanced Instructions (128-step)
			spdCache = packetData[packetReadStartPos + 1] & 0x7F;
			
			if (1) {
			//if (speed128stepCache == spdCache) {
				speed128stepCache = 0xFF;
				
				if (packetData[packetReadStartPos + 1] & 0x80) {
					// Forward
					if (CV29 & 0x01) directionFlag = 2;
					else directionFlag = 1;
				} else {
					// Backward
					if (CV29 & 0x01) directionFlag = 1;
					else directionFlag = 2;
				}
				
				if (checkABCreverseDirection()) {
					if (directionFlag == 1) directionFlag = 2;
					else directionFlag = 1;
				}
				setspeed_128step(directionFlag, spdCache);
			} else {
				speed128stepCache = spdCache;
			}
			break;
		
		case 0x40:
		case 0x60:
			// Speed Control (14-step, 28-step)
			if (packetLength != (packetReadStartPos + 2)) {
				return;
			}
			
			if (1) {
			//if (speed28stepCache == packetData[packetReadStartPos]) {
				speed28stepCache = 0xFF;
				
				if (packetInstruction & 0x20) {
					// Forward
					if (CV29 & 0x01) directionFlag = 2;
					else directionFlag = 1;
				} else {
					// Backward
					if (CV29 & 0x01) directionFlag = 1;
					else directionFlag = 2;
				}
				
				if (checkABCreverseDirection()) {
					if (directionFlag == 1) directionFlag = 2;
					else directionFlag = 1;
				}
				setspeed_28step(directionFlag, packetData[packetReadStartPos]);
			} else {
				speed28stepCache = packetData[packetReadStartPos];
			}
			break;
		case 0x80:
			// Function1 (F0-F4)
			if (packetLength != (packetReadStartPos + 2)) {
				return;
			}
			funcCtrl(1, packetData[packetReadStartPos], directionFlag);
			break;
		case 0xA0:
			// Function2 (F5-F8, F9-F12)
			if (packetLength != (packetReadStartPos + 2)) {
				return;
			}
			if (packetData[packetReadStartPos] & 0x10) {
				// F5-F8
				funcCtrl(2, packetData[packetReadStartPos], directionFlag);
			} else {
				// F9-F12
				funcCtrl(3, packetData[packetReadStartPos], directionFlag);
			}
			break;
		case 0xC0:
			// Future Expansion Instruction
			if (packetData[packetReadStartPos] == 0xDE) {
				// F13-F20
				funcCtrl(4, packetData[packetReadStartPos + 1], directionFlag);
			} else if (packetData[packetReadStartPos] == 0xDF) {
				// F21-F28
				funcCtrl(5, packetData[packetReadStartPos + 1], directionFlag);
			}
			break;
		case 0xE0:
			// Operation Mode CV Instruction
			for (i = 0; i < 3; i++) {
				if (opsModeCache[i] != packetData[packetReadStartPos + i]) {
					opsModeCache[0] = packetData[packetReadStartPos];
					opsModeCache[1] = packetData[packetReadStartPos + 1];
					opsModeCache[2] = packetData[packetReadStartPos + 2];
					opsModeAutoResetFlag = 1;
				}
			}
			
			opsModeAutoResetFlag = 0;
			
			if (packetData[packetReadStartPos] & 0x08) {
				// Write
				
				if (packetData[packetReadStartPos] & 0x04) {
					//Byte Mode
					write_cv_byte((((packetData[packetReadStartPos] & 0x03) << 8) + packetData[packetReadStartPos + 1] + 1), packetData[packetReadStartPos + 2]);
				} else {
					//Bit Mode
					write_cv_bit((((packetData[packetReadStartPos] & 0x03) << 8) + packetData[packetReadStartPos + 1] + 1), packetData[packetReadStartPos + 2]);
				}
			}
			break;
			
	}
	
	
	
}

uint8_t readDirectionFlag(void)
{
	return (directionFlag);
}

uint8_t checkServiceMode(void)
{
	if (serviceModeFlag != 0) {
		return (1);
	}
	return (0);
}