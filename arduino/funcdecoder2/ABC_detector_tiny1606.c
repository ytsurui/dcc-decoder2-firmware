/*
 * ABC_detector_tiny1606.c
 *
 * Created: 2021/02/04 0:05:32
 *  Author: Y.Tsurui
 */ 

/*
#ifndef NO_ABC

#include <avr/io.h>
#include "ABC_detector.h"
#include "cv_value.h"
#include "func_ctrl.h"

uint8_t ABCcountHigh_Right = 0;
uint8_t ABCcountLow_Right = 0;

uint8_t ABCcountHigh_Left = 0;
uint8_t ABCcountLow_Left = 0;

uint8_t gCount = 0;

uint8_t ABCRightCount = 0;
uint8_t ABCLeftCount = 0;

uint8_t ABCRightCountTH = 0;
uint8_t ABCLeftCountTH = 0;

uint8_t ABCsumRight[4];
uint8_t ABCsumLeft[4];

uint8_t ABCsumRightPos = 0;
uint8_t ABCsumLeftPos = 0;

uint8_t ABCstatus = 0;
uint8_t ABCstatus2 = 0;

uint8_t ABCleftBackCount = 0;
uint8_t ABCrightBackCount = 0;

#define ABC_ENABLE_THRESHOLD	10

void initABCpoller(void) {
	// ABC+: PA5 / ABC-: PC2
	
	PORTA.DIRCLR = PIN5_bm;	// ABC+
	PORTC.DIRCLR = PIN2_bm;	// ABC-
	PORTA.PIN5CTRL |= PORT_PULLUPEN_bm;
	PORTC.PIN2CTRL |= PORT_PULLUPEN_bm;
}

uint8_t getABCrightCount(void) {
	return ABCRightCountTH;
}

uint8_t getABCleftCount(void) {
	return ABCLeftCountTH;
}


uint8_t setABCsumRight(uint8_t value) {
	uint16_t backValue = 0;
	uint8_t i;
	
	ABCsumRight[ABCsumRightPos] = value;
	ABCsumRightPos++;
	if (ABCsumRightPos >= 4) ABCsumRightPos = 0;
	
	for (i = 0; i < 4; i++) {
		backValue += ABCsumRight[i];
	}
	
	return (uint8_t)(backValue >> 4);
}

uint8_t setABCsumLeft(uint8_t value) {
	uint16_t backValue = 0;
	uint8_t i;
	
	ABCsumLeft[ABCsumLeftPos] = value;
	ABCsumLeftPos++;
	if (ABCsumLeftPos >= 4) ABCsumLeftPos = 0;
	
	for (i = 0; i < 4; i++) {
		backValue += ABCsumLeft[i];
	}
	
	return (uint8_t)(backValue >> 4);
}


/*
void ABCcheckRight(void) {
	if (PORTA.IN & PIN5_bm) {	// ABC+
		ABCcountHigh_Right++;
	} else {
		ABCcountLow_Right++;
	}
	ABCRightCount++;
	
	if (ABCRightCount >= 250) {
		//ABCRightCountTH = ABCcountLow_Right;
		ABCRightCountTH = setABCsumRight(ABCcountLow_Right);
		ABCRightCount = 0;
		
		if (CV27 & 0x01) {
			//if (ABCcountHigh_Right > ABCcountLow_Right) {
			//if (ABCcountLow_Right < ABC_ENABLE_THRESHOLD) {
			//if (ABCRightCountTH < ABC_ENABLE_THRESHOLD) {
			if (ABCRightCountTH < CV48) {
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
*/
/*
void ABCcheckLeft(void) {
	if (PORTC.IN & PIN2_bm) {	// ABC-
		ABCcountHigh_Left++;	
	} else {
		ABCcountLow_Left++;
	}
	ABCLeftCount++;
	
	if (ABCLeftCount >= 250) {
		//ABCLeftCountTH = ABCcountLow_Left;
		ABCLeftCountTH = setABCsumLeft(ABCcountLow_Left);
		ABCLeftCount = 0;
		
		if (CV27 & 0x02) {
			//if (ABCcountHigh_Left > ABCcountLow_Left) {
			//if (ABCcountLow_Left < ABC_ENABLE_THRESHOLD) {
			//if (ABCLeftCountTH < ABC_ENABLE_THRESHOLD) {
			if (ABCLeftCountTH < CV48) {
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
*/

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

uint8_t getABCstatus(void)
{
	if (ignoreABCstat()) {
		return (0);
	}
	return (ABCstatus2);
}

void ABCmsEvent()
{
	if (ABCstatus2 & ABC_STAT_EN_FOR) {
		// right
		if (~ABCstatus & ABC_STAT_EN_FOR) {
			ABCrightBackCount++;
			if (ABCrightBackCount > CV148) {
				ABCrightBackCount = 0;
				ABCstatus2 &= ~ABC_STAT_EN_FOR;
			}
		} else {
			ABCrightBackCount = 0;
		}
	} else {
		if (ABCstatus & ABC_STAT_EN_FOR) {
			// �����ɔ��f
			ABCstatus2 |= ABC_STAT_EN_FOR;
		}
		ABCrightBackCount = 0;
	}
	
	if (ABCstatus2 & ABC_STAT_EN_REV) {
		// left
		if (~ABCstatus & ABC_STAT_EN_REV) {
			ABCleftBackCount++;
			if (ABCleftBackCount > CV148) {
				ABCleftBackCount = 0;
				ABCstatus2 &= ~ABC_STAT_EN_REV;
			}
		} else {
			ABCleftBackCount = 0;
		}
	} else {
		if (ABCstatus & ABC_STAT_EN_REV) {
			ABCstatus2 |= ABC_STAT_EN_REV;
		}
		ABCleftBackCount = 0;
	}
}

#endif
*/
