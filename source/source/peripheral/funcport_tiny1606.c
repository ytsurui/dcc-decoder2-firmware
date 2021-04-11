/*
 * funcport_tiny1606.c
 *
 * Created: 2021/02/06 19:02:56
 *  Author: Y.Tsurui
 */ 

#include <avr/io.h>
#include "funcport.h"

uint8_t funcProgModeFlag = 0;

void initFuncPort(void)
{
	//PORTA.DIRSET |= (PIN3_bm | PIN7_bm);			// PA3,PA7 (Func1,Func4)
	//PORTB.DIRSET |= (PIN3_bm | PIN4_bm | PIN5_bm);	// PB3,PB4,PB5 (Func2,Func7,Func3)
	//PORTC.DIRSET |= (PIN0_bm | PIN1_bm);			// PC0,PC1 (Func5,Func6)
	PORTA.DIRSET |= (PIN3_bm | PIN7_bm);			// PA3,PA7 (Func1,Func3)
	PORTB.DIRSET |= (PIN3_bm | PIN4_bm | PIN5_bm);	// PB3,PB4,PB5 (Func4,Func7,Func5)
	PORTC.DIRSET |= (PIN0_bm | PIN1_bm);			// PC0,PC1 (Func6,Func2)
	
	PORTB.DIRSET &= ~(PIN1_bm);						// PB1 (Direction Jumper)
	PORTB.PIN1CTRL = PORT_PULLUPEN_bm;	// Pullup & Invert Input (Direction Jumper)
}

//	Func2 -> Func4
//	Func3 -> Func5
//	Func4 -> Func3
//	Func5 -> Func6
//	Func6 -> Func2

void funcPortCtrl(uint8_t funcNum, uint8_t funcStat)
{
	switch (funcNum) {
		case 0:	// PA3 (Func1)
			if (funcStat) PORTA.OUTSET = PIN3_bm;
			else PORTA.OUTCLR = PIN3_bm;
			break;
		case 1: // Func2: PB3 -> PC1
			//if (funcStat) PORTB.OUTSET = PIN3_bm;
			//else PORTB.OUTCLR = PIN3_bm;
			if (funcStat) PORTC.OUTSET = PIN1_bm;
			else PORTC.OUTCLR = PIN1_bm;
			break;
		case 2: // Func3: PB5 -> PA7
			//if (funcStat) PORTB.OUTSET = PIN5_bm;
			//else PORTB.OUTCLR = PIN5_bm;
			if (funcStat) PORTA.OUTSET = PIN7_bm;
			else PORTA.OUTCLR = PIN7_bm;
			break;
		case 3:	// Func4: PA7 -> PB3
			//if (funcStat) PORTA.OUTSET = PIN7_bm;
			//else PORTA.OUTCLR = PIN7_bm;
			if (funcStat) PORTB.OUTSET = PIN3_bm;
			else PORTB.OUTCLR = PIN3_bm;
			break;
		case 4:	// Func5: PC0 -> PB5
			//if (funcStat) PORTC.OUTSET = PIN0_bm;
			//else PORTC.OUTCLR = PIN0_bm;
			if (funcStat) PORTB.OUTSET = PIN5_bm;
			else PORTB.OUTCLR = PIN5_bm;
			break;
		case 5:	// Func6: PC1 -> PC0
			//if (funcStat) PORTC.OUTSET = PIN1_bm;
			//else PORTC.OUTCLR = PIN1_bm;
			if (funcStat) PORTC.OUTSET = PIN0_bm;
			else PORTC.OUTCLR = PIN0_bm;
			break;
		case 6: // PB4 (Func7)
			if (funcStat) PORTB.OUTSET = PIN4_bm;
			else PORTB.OUTCLR = PIN4_bm;
			break;
		case 7: // MotorDriver (Func8)
			
			break;
	}
}

uint8_t readDirectionReverse(void)
{
	if (~PORTB.IN & PIN1_bm) {
		return (1);
	}
	return (0);
}

uint8_t readFuncProgMode(void)
{
	return (funcProgModeFlag);
}

void funcPortProgACK(uint8_t stat) {
	uint8_t i;
	
	if (stat == FUNC_PROG_ACK_ON) {
		funcProgModeFlag = 1;
	} else {
		funcProgModeFlag = 0;
	}
	
	for (i = 0; i < 7; i++) {
		funcPortCtrl(i, funcProgModeFlag);
	}
}