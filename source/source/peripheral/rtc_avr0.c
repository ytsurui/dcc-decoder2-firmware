/*
 * rtc_test.c
 *
 * Created: 2021/01/07 0:05:09
 *  Author: Y.Tsurui
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

uint8_t rtcFlag = 0;


ISR(RTC_PIT_vect) {
	rtcFlag |= 0x01;
	RTC.PITINTFLAGS = RTC_PI_bm;
	//PORTA.OUT = 0xFF;
}

ISR(RTC_CNT_vect) {
	RTC.INTFLAGS = RTC_CMP_bm | RTC_OVF_bm;
	rtcFlag |= 0x02;
	//RTC.CNT = 0;
}


// Configure 1kHz & 16kHz Timer
void initRTC(void)
{
	while (RTC.STATUS > 0);
	
	RTC.CMP = 0;
	RTC.PER = 1;
	
	RTC.CLKSEL = RTC_CLKSEL_INT32K_gc;
	RTC.CTRLA = RTC_RUNSTDBY_bm | RTC_PRESCALER_DIV1_gc | RTC_RTCEN_bm;
	RTC.PITINTCTRL |= RTC_PI_bm;
	RTC.PITCTRLA = RTC_PERIOD_CYC32_gc | RTC_PITEN_bm;
	
	RTC.INTCTRL |= RTC_CMP_bm | RTC_OVF_bm;
	
	sei();
}

uint8_t chkRTCflag(void)
{
	if (rtcFlag & 0x01) {
		rtcFlag &= ~0x01;
		return (1);
	}
	return (0);
}

uint8_t chkHighSpeedRTCflag(void)
{
	if (rtcFlag & 0x02) {
		rtcFlag &= ~0x02;
		return (1);
	}
	return (0);
}
