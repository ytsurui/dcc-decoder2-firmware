/*
 * main.c
 *
 * Created: 2021/02/03 22:07:31
 * Author : Y.Tsurui
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "peripheral/cpuclk.h"
#include "peripheral/rtc.h"
#include "peripheral/dcc_poller.h"
#include "peripheral/uart.h"
#include "peripheral/ABC_detector.h"
#include "peripheral/motor.h"
#include "peripheral/funcport.h"

#include "dcc/cv.h"

#include "dcc/dcc_router.h"

#include "app/func_ctrl.h"
#include "app/train_ctrl.h"

#include "dcc/railcom.h"

#include "dcc/cv_value.h"


void delay_ms(uint16_t ms)
{
	while(ms--) {
		_delay_ms(1);
	}
}



void packetTestRecv3(uint8_t packetLength, uint8_t* packetData)
{
	if (packetLength == 0) return;
	
	if (packetData[0] != 3) return;
	
	if ((packetData[1] & 0xE0) == 0x80) {
		// Function 1 (F0-F4)
		if (packetData[1] & 0x01) {
			PORTA.OUTSET = PIN3_bm;
		} else {
			PORTA.OUTCLR = PIN3_bm;
		}
	}
}

void packetTestRecv2(uint8_t packetLength, uint8_t* packetData)
{
	packetTestRecv3(packetLength, packetData);
}



int main(void)
{
	
	uint8_t dccRecvPacketCache[16];
	uint8_t dccRecvPacketLength;
	
	uint8_t changeFreqCount = 0;
	uint8_t changeFreqCount2 = 0;
	
	uint8_t RTCclkFlag = 0;
	
	//uint16_t testcount = 0;
	
    setCLK();	// configure as 10MHz
	initRTC();
	initMotorModule();
	initFuncPort();
	initDCCpoller();
	initUART();
	initABCpoller();
	
	load_cv();
	
	sei();
	
	// Test Sample
	/*
	while (1)
	{
		if (chkHighSpeedRTCflag()) {
			testcount++;
			if (testcount >= 8000) {
				testcount = 0;
				PORTA.OUTTGL |= PIN3_bm;
			}
		}
		
		if (chkRTCflag()) {
			changeFreqCount++;
			if (changeFreqCount >= 250) {
				changeFreqCount = 0;
				PORTB.OUTTGL |= PIN3_bm;
			}
		}
	}
	*/
	
	while (1) 
    {
		dccPacketShifter(&dccRecvPacketLength, dccRecvPacketCache);
		
		if (dccRecvPacketLength) {
			//packetTestRecv2(dccRecvPacketLength, dccRecvPacketCache);
			dccPacketRouter(dccRecvPacketLength, dccRecvPacketCache);
			dccRecvPacketLength = 0;
		}
		/*
		PORTA.OUTSET |= PIN3_bm;
		PORTB.OUTCLR |= PIN3_bm;
		delay_ms(250);
		PORTA.OUTCLR |= PIN3_bm;
		PORTB.OUTSET |= PIN3_bm;
		delay_ms(250);
		*/
		
		if (checkRailcomSendChannel1()) {
			if (checkServiceMode() == 0) railcomChannel1AddrSend();
		}
		
		if (chkHighSpeedRTCflag()) {
			// 16kHz Event Timer
			if (checkServiceMode() == 0) {
				HSclockReceiverMotorCtrl();
			}
			//clockReceiverFuncCtrl();
		}
		
		if (chkRTCflag()) {
			// 1kHz Event Timer
			if (RTCclkFlag == 0) {
				RTCclkFlag = 1;
			}
		}
		
		if (RTCclkFlag != 0) {
			if (RTCclkFlag == 1) {
				DCCpollerClkReceiver();
			} else if (RTCclkFlag == 2) {
				dccRouterClockReceiver();
			} else if (RTCclkFlag == 3) {
				if (checkServiceMode() == 0) {
					clockReceiverFuncCtrl();
				}
			} else if (RTCclkFlag == 4) {
				if (checkServiceMode() == 0) {
					clock_receiver_train_ctrl();
				}
			} else if (RTCclkFlag == 5) {
				if (checkServiceMode() == 0) {
					ABCclkReceiver();
				}
			} else if (RTCclkFlag == 6) {
				if ((checkServiceMode() == 0) && (CV29 & 0x04)) {
					// Enable Analog Mode
					if (readAnalogStat()) {
						setspeed_analog(readAnalogStat());
						funcCtrlAnalog(readAnalogStat());
					}
				}
			} else if (RTCclkFlag == 7) {
				if ((checkServiceMode() == 0) && (CV47 & 0x80)) {
					changeFreqCount++;
				
					if (changeFreqCount >= 10) {
						changeFreqCount = 0;
						changeFreqCount2++;
					}
					if (changeFreqCount2 >= (CV47 & 0x3F)) {
						changeFreqCount2 = 0;
						if (CV47 & 0x40) {
							// 60Hz
							pwmChangeFreqOperation(PWM_FREQ_38HZ);
						} else {
							// 120Hz
							pwmChangeFreqOperation(PWM_FREQ_152HZ);
						}
					}
				}
				RTCclkFlag = 0;
			}
			
			RTCclkFlag++;
		}
		
		
    }
}

