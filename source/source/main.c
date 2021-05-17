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
#include "peripheral/cpu_wdt.h"
#include "peripheral/rtc.h"
#include "peripheral/dcc_poller.h"
#include "peripheral/analog_poller.h"
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


int main(void)
{
	
	uint8_t dccRecvPacketCache[48];
	uint8_t dccRecvPacketLength;
	
	uint8_t changeFreqCount = 0;
	uint8_t changeFreqCount2 = 0;
	
	uint8_t RTCclkFlag = 0;
	
	uint8_t ABCpollerFlag = 0;
	
	uint8_t BEMFintervalCounter = 0;
	
	//uint16_t testcount = 0;
	
    setCLK();	// configure as 10MHz
	initRTC();
	initMotorModule();
	initFuncPort();
	initDCCpoller();
	initUART();
	initABCpoller();
	
	load_cv();
	
	startWDT();
	
	sei();
	
		
	while (1) 
    {
		clearWDT();
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
		
		if (checkServiceMode()) {
			if (chkRTCflag()) {
				dccRouterClockReceiver();
			}
		} else {
			
			ABCpollerFlag = ABCpollerReaderFlag();
			if (ABCpollerFlag == 1) {
				ABCcheckRight();
				ABCcheckLeft();
			} else if (ABCpollerFlag == 2) {
				ABCcheckRight();
				ABCcheckLeft();
			}
		
			if (checkRailcomSendChannel1()) {
				railcomChannel1AddrSend();
			}
		
			if (chkHighSpeedRTCflag()) {
				// 16kHz Event Timer
				HSclockReceiverMotorCtrl();
				HSclockReceiverFuncCtrl();
			}
			
			if (chkRTCflag()) {
				if (RTCclkFlag == 0) RTCclkFlag = 1;
				
				BEMFintervalCounter++;
				if (BEMFintervalCounter == 10) { // 100Hz BEMF Poller
					captureBEMF();
					BEMFintervalCounter = 0;
				}
			}
						
			
			if (RTCclkFlag != 0) {
				switch (RTCclkFlag) {
					case 1:
						DCCpollerClkReceiver();
						analogPollerClkReceiver();
						break;
					case 2:
						//ABCclkReceiver();
						clockReceiverABCctrl();
						break;
					case 3:
						clock_receiver_train_ctrl();
						break;
					case 4:
						clockReceiverFuncCtrl();
						break;
					case 5:
					case 6:
					case 7:
					case 8:
					case 9:
					case 10:
					case 11:
					case 12:
						clockReceiverFuncCtrlSub(RTCclkFlag - 5);
						break;
					case 13:
						dccRouterClockReceiver();
						break;
					case 14:
						if (CV29 & 0x04) {
							// Enable Analog Mode
							/*
							if (readAnalogStat()) {
								setspeed_analog(readAnalogStat());
								funcCtrlAnalog(readAnalogStat());
							}
							*/
							if (readAnalogStat()) {
								setspeed_analog(analogDirection());
								funcCtrlAnalog(analogDirection());
							}
						}
						break;
					case 15:
						if (CV47 & 0x80) {
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
						break;
				}
				RTCclkFlag++;
				if (RTCclkFlag >= 16) RTCclkFlag = 0;
			}
		
		}
		
		
    }
}

