/*
 * ABC_detector_tiny1606.c
 *
 * Created: 2021/02/04 0:05:32
 *  Author: Y.Tsurui
 */ 

#include <avr/io.h>
#include "ABC_detector.h"
#include "../dcc/cv_value.h"
#include "../app/func_ctrl.h"

uint8_t ABCcountHigh_Right = 0;
uint8_t ABCcountLow_Right = 0;

uint8_t ABCcountHigh_Left = 0;
uint8_t ABCcountLow_Left = 0;

uint8_t gCount = 0;

uint8_t ABCRightCount = 0;
uint8_t ABCLeftCount = 0;

uint8_t ABCstatus = 0;

#define ABC_ENABLE_THRESHOLD	10

void initABCpoller(void) {
	// ABC+: PA5 / ABC-: PC2
	
	PORTA.DIRCLR = PIN5_bm;	// ABC+
	PORTC.DIRCLR = PIN2_bm;	// ABC-
	PORTA.PIN5CTRL |= PORT_PULLUPEN_bm;
	PORTC.PIN2CTRL |= PORT_PULLUPEN_bm;
}


void ABCcheckRight(void) {
	if (PORTA.IN & PIN5_bm) {	// ABC+
		ABCcountHigh_Right++;
	} else {
		ABCcountLow_Right++;
	}
	ABCRightCount++;
	
	if (ABCRightCount >= 250) {
		ABCRightCount = 0;
		
		if (CV27 & 0x01) {
			//if (ABCcountHigh_Right > ABCcountLow_Right) {
			if (ABCcountLow_Right < ABC_ENABLE_THRESHOLD) {
				ABCstatus |= ABC_STAT_EN_FOR;
			} else {
				ABCstatus &= ~ABC_STAT_EN_FOR;
			}
		} else {
			ABCstatus &= ~ABC_STAT_EN_FOR;
		}
		
		ABCcountHigh_Right = 0;
		ABCcountLow_Right = 0;
	}
	
}

void ABCcheckLeft(void) {
	if (PORTC.IN & PIN2_bm) {	// ABC-
		ABCcountHigh_Left++;	
	} else {
		ABCcountLow_Left++;
	}
	ABCLeftCount++;
	
	if (ABCLeftCount >= 250) {
		ABCLeftCount = 0;
		
		if (CV27 & 0x02) {
			//if (ABCcountHigh_Left > ABCcountLow_Left) {
			if (ABCcountLow_Left < ABC_ENABLE_THRESHOLD) {
				ABCstatus |= ABC_STAT_EN_REV;
			} else {
				ABCstatus &= ~ABC_STAT_EN_REV;
			}
		} else {
			ABCstatus &= ~ABC_STAT_EN_REV;
		}
		
		ABCcountHigh_Left = 0;
		ABCcountLow_Left = 0;
	}
	
}


/*
void ABCclkReceiver(void) {
	
	if (PORTA.IN & PIN5_bm) {	// ABC+
		ABCcountHigh_Right++;
	} else {
		ABCcountLow_Right++;
	}
	
	if (PORTC.IN & PIN2_bm) {	// ABC-
		ABCcountHigh_Left++;
	} else {
		ABCcountLow_Left++;
	}
	
	gCount++;
	
	if (gCount >= 250) {
		gCount = 0;
		
		if (CV27 & 0x01) {
			//if (ABCcountHigh_Right > ABCcountLow_Right) {
			//if (ABCcountHigh_Right > ABC_ENABLE_THRESHOLD) {
			if (ABCcountLow_Right < ABC_ENABLE_THRESHOLD) {
				ABCstatus |= ABC_STAT_EN_FOR;
			} else {
				ABCstatus &= ~ABC_STAT_EN_FOR;
			}
		} else {
			ABCstatus &= ~ABC_STAT_EN_FOR;
		}
		
		if (CV27 & 0x02) {
			//if (ABCcountHigh_Left > ABCcountLow_Left) {
			//if (ABCcountHigh_Left > ABC_ENABLE_THRESHOLD) {
			if (ABCcountLow_Left < ABC_ENABLE_THRESHOLD) {
				ABCstatus |= ABC_STAT_EN_REV;
			} else {
				ABCstatus &= ~ABC_STAT_EN_REV;
			}
		} else {
			ABCstatus &= ~ABC_STAT_EN_REV;
		}
		
		ABCcountHigh_Left = 0;
		ABCcountLow_Left = 0;
		ABCcountHigh_Right = 0;
		ABCcountLow_Right = 0;
	}
	
	
}
*/

uint8_t getABCstatus(void)
{
	if (ignoreABCstat()) {
		return (0);
	}
	return (ABCstatus);
}
