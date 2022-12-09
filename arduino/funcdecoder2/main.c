/*
 * main.c
 *
 * Created: 2021/02/03 22:07:31
 * Author : Y.Tsurui
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "cpuclk.h"
#include "cpu_wdt.h"
#include "rtc.h"
#include "dcc_poller.h"
#include "analog_poller.h"
#include "uart.h"
#include "ABC_detector.h"
#include "motor.h"
#include "funcport.h"

#include "cv.h"

#include "dcc_router.h"

#include "func_ctrl.h"
#include "train_ctrl.h"

//#ifndef NO_RAILCOM
//#include "railcom.h"
//#endif

#include "cv_value.h"


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
/*
#ifndef ATTINY806_FUNC
	//uint8_t changeFreqCount = 0;
	//uint8_t changeFreqCount2 = 0;
	
	uint8_t BEMFintervalCounter = 0;
	
	uint8_t ABCpollerFlag = 0;
	
	uint8_t funcRunFlag;
#endif
*/
	
	uint8_t RTCclkFlag = 0;
	
	
	
	
	//uint16_t testcount = 0;
	
    setCLK();	// configure as 10MHz
	initRTC();
	initMotorModule();
	initFuncPort();
	initDCCpoller();
	initUART();
//#ifndef ATTINY806_FUNC
//	initABCpoller();
//#endif
	
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

/*			
#ifndef NO_ABC
			ABCpollerFlag = ABCpollerReaderFlag();
			if (ABCpollerFlag == 1) {
				ABCcheckRight();
				ABCcheckLeft();
			} else if (ABCpollerFlag == 2) {
				ABCcheckRight();
				ABCcheckLeft();
			}
#endif
*/	

			if (chkHighSpeedRTCflag()) {
				// 16kHz Event Timer
//#ifndef ATTINY806_FUNC
//				HSclockReceiverMotorCtrl();
//#endif
				HSclockReceiverFuncCtrl();
			}
			
			if (chkRTCflag()) {
				if (RTCclkFlag == 0) RTCclkFlag = 1;

/*
#ifndef ATTINY806_FUNC
				BEMFintervalCounter++;
				if (BEMFintervalCounter == 5) {	// 100Hz MOSFET Current Poller
					captureCurrent();
				} else if (BEMFintervalCounter == 10) { // 100Hz BEMF Poller
					captureBEMF();
					BEMFintervalCounter = 0;
				}
#endif
*/
			}
						
			
			if (RTCclkFlag != 0) {
				switch (RTCclkFlag) {
					case 1:
						DCCpollerClkReceiver();
						analogPollerClkReceiver();
						break;
/*
#ifndef ATTINY806_FUNC
					case 2:
						//ABCclkReceiver();
						clockReceiverABCctrl();
						break;
					case 3:
						clock_receiver_train_ctrl();
						break;
#endif
*/
					case 4:
//#ifndef ATTINY806_FUNC
//						if (funcRunFlag) {
//							funcRunFlag = 0;
//						} else {
//							clockReceiverFuncCtrl();
//							funcRunFlag = 1;
//						}
//#else
						clockReceiverFuncCtrl();
//#endif
						break;
					case 5:
					case 6:
					case 7:
					case 8:
					case 9:
					case 10:
					case 11:
					case 12:
//#ifndef ATTINY806_FUNC
//						if (funcRunFlag) {
//							clockReceiverFuncCtrlSub(RTCclkFlag - 5);
//						}
//#else
						clockReceiverFuncCtrlSub(RTCclkFlag - 5);
//#endif
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
								//#ifndef ATTINY806_FUNC
								//setspeed_analog(analogDirection());
								//#endif
								funcCtrlAnalog(analogDirection());
							}
						}
						break;
//#ifndef NO_ABC
//					case 15:
//						ABCmsEvent();
//						break;
//#endif
					/*
					#ifndef ATTINY806_FUNC
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
					#endif
					*/
				}
				RTCclkFlag++;
				if (RTCclkFlag >= 16) RTCclkFlag = 0;
			}
		
		}
		
		
    }
}

