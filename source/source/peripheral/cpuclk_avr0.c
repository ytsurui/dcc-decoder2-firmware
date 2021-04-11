/*
 * cpuclk.c
 *
 * Created: 2021/01/25 10:04:40
 *  Author: Y.Tsurui
 */ 

#include <avr/io.h>

void setCLK(void)
{
	// Clock Configuration (10MHz)
	CCP = CCP_IOREG_gc;
	CLKCTRL.MCLKCTRLB = 0x01;	// Prescaler Enable, prescale=2 (10MHz)
	
	// BOD Enable
	BOD.CTRLA = BOD_ACTIVE_ENABLED_gc;
	BOD.CTRLB = BOD_LVL_BODLEVEL0_gc; 
}
