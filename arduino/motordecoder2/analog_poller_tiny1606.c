/*
 * analog_poller_tiny1606.c
 *
 * Created: 2021/02/14 16:18:11
 *  Author: Y.Tsurui
 */ 

#include <avr/io.h>


uint8_t analogDirectionFlag = 0;

uint8_t analogDirectionRightThCount = 0;
uint8_t analogDirectionLeftThCount = 0;

#define ANALOG_DIR_COUNT_MAX	30
#define ANALOG_DIR_COUNT_TH		15

void analogPollerClkReceiver(void)
{
	if (PORTA.IN & PIN2_bm) {
		if (PORTA.IN & PIN4_bm) {
			// Don't care
		} else {
			if (analogDirectionLeftThCount > 0) analogDirectionLeftThCount--;
			if (analogDirectionRightThCount < ANALOG_DIR_COUNT_MAX) analogDirectionRightThCount += 2;
		}
	} else {
		if (PORTA.IN & PIN4_bm) {
			if (analogDirectionRightThCount > 0) analogDirectionRightThCount--;
			if (analogDirectionLeftThCount < ANALOG_DIR_COUNT_MAX) analogDirectionLeftThCount += 2;
		}
	}
	
	if (analogDirectionRightThCount > ANALOG_DIR_COUNT_TH) {
		analogDirectionFlag = 1;
	} else if (analogDirectionLeftThCount > ANALOG_DIR_COUNT_TH) {
		analogDirectionFlag = 2;
	} else {
		analogDirectionFlag = 0;
	}
}

uint8_t analogDirection(void)
{
	return (analogDirectionFlag);
}
