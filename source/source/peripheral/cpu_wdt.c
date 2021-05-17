/*
 * cpu_wdt.c
 *
 * Created: 2021/05/17 20:50:35
 *  Author: Y.Tsurui
 */ 

#include <avr/io.h>
#include <avr/cpufunc.h>
#include <avr/wdt.h>

void startWDT(void)
{
	ccp_write_io((void *) & (WDT.CTRLA), WDT_PERIOD_256CLK_gc | WDT_WINDOW_OFF_gc);
}

void clearWDT(void)
{
	wdt_reset();
}