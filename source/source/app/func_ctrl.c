/*
 * func_ctrl.c
 *
 * Created: 2020/10/02 22:07:39
 * Project: FuncDecoder_tiny45
 *  Author: Y.Tsurui
 */ 

#include <avr/io.h>
#include "../peripheral/funcport.h"
#include "../dcc/cv_value.h"
#include "../dcc/dcc_router.h"

#include "../peripheral/motor.h"

#include "func_effect.h"

void funcSet(uint8_t funcNum, uint8_t stat, uint8_t direction);
void funcSetVirtual(uint8_t funcID, uint8_t cv, uint8_t funcNum, uint8_t stat);
void funcSetPort(uint8_t funcPort, uint8_t cv, uint8_t funcNum, uint8_t stat, uint8_t direction);
void funcSetPort2(uint8_t funcPort, uint8_t count);
void funcSetPortMotor(uint8_t cv, uint8_t funcNum, uint8_t stat, uint8_t direction);
void funcPortCtrlBridge(uint8_t funcNum, uint8_t funcStat, uint8_t direction);

/*
 *	funcVirtualStat
 *		bit7: Slow Mode for yard Operation
 *		bit6: Ignore ABC Operation
 *		bit3: Headlight Only function (Backward)
 *		bit2: Headlight Only function (Forward)
 *		bit1: Taillight Yard Operation
 *		bit0: Headlight Dimming
 */
uint8_t funcVirtualStat;

/*
 *	funcPortStat
 *		bit7-bit0:	FMotor,F7,F6,...,F2,F1
 */
uint8_t oldFuncPortStat = 0;
uint8_t funcPortStat = 0;

uint8_t funcSetDirection;

uint16_t funcStatusCount[9];
uint8_t HSfuncValue[9];
uint8_t HSclkCounter;

uint8_t HSfuncStageFlag = 0;

uint8_t funcCount = 0;
uint8_t funcCount2 = 0;
#define FUNC_COUNT_MAX	0x0F

void HSclockReceiverFuncCtrl(void) {
	//uint8_t i;
	uint8_t direction;
	uint8_t HSclkCounterFiltered, funcValueDivided;
	
	if (HSclkFuncUseFlag == 0) return;
	
	if (HSfuncStageFlag < 7) {
		if (HSfuncValue[HSfuncStageFlag] != 0) {
			HSclkCounterFiltered = HSclkCounter & 0x0F;
			funcValueDivided = HSfuncValue[HSfuncStageFlag] >> 4;
			
			if (funcValueDivided > HSclkCounterFiltered) {
				funcPortCtrl(HSfuncStageFlag, 1);
			} else {
				funcPortCtrl(HSfuncStageFlag, 0);
			}
		}
	} else if (HSfuncStageFlag == 7) {
		if (HSfuncValue[7] != 0) {
			HSclkCounterFiltered = HSclkCounter & 0x0F;
			funcValueDivided = HSfuncValue[7] >> 4;
			
			if (funcVirtualStat & 0x04) {
				direction = 1;
			} else {
				direction = funcSetDirection;
			}
			
			if (funcValueDivided > HSclkCounterFiltered) {
				motorFuncDriver(1, direction);
			} else {
				motorFuncDriver(0, direction);
			}
		}
	}
	
	HSfuncStageFlag++;
	if (HSfuncStageFlag == 8) {
		HSclkCounter++;
		HSfuncStageFlag = 0;
	}
	
	/*
	HSclkCounter++;
	for (i = 0; i < 7; i++) {
		if (HSfuncValue[i] == 0) continue;
		
		if (HSfuncHighSpeedFlag & (1 << i)) {
			HSclkCounterFiltered = HSclkCounter & 0x1F;
			funcValueDivided = HSfuncValue[i] >> 3;
			
			if (funcValueDivided > HSclkCounterFiltered) {
				funcPortCtrl(i, 1);
			} else {
				funcPortCtrl(i, 0);
			}
		} else {
			if (HSfuncValue[i] > HSclkCounter) {
				//funcPortCtrlBridge(i, 1, direction);
				funcPortCtrl(i, 1);
			} else {
				//funcPortCtrlBridge(i, 0, direction);
				funcPortCtrl(i, 0);
			}
		}
	}
	
	if (HSfuncValue[7] != 0) {
		
		if (funcVirtualStat & 0x04) {
			direction = 1;
		} else {
			direction = funcSetDirection;
		}
		
		if (HSfuncHighSpeedFlag & (1 << i)) {
			HSclkCounterFiltered = HSclkCounter & 0x1F;
			funcValueDivided = HSfuncValue[i] >> 3;
			
			if (funcValueDivided > HSclkCounterFiltered) {
				motorFuncDriver(1, direction);
			} else {
				motorFuncDriver(0, direction);
			}
		} else {
			if (HSfuncValue[i] > HSclkCounter) {
				//funcPortCtrlBridge(i, 1, direction);
				motorFuncDriver(1, direction);
			} else {
				//funcPortCtrlBridge(i, 0, direction);
				motorFuncDriver(0, direction);
			}
		}
		
	}
	*/
	
}

void clockReceiverFuncCtrlSub(uint8_t i) {
	uint8_t mask, dirFlag;
	mask = 0x01 << i;
	
	if (i == 7) {
		if (CV33_43[10] != 0x01) {
			// MotorDriver Function
			return;
		}
	}
	if ((CV33_43[i + 2] & 0x1F) == 0x1F) {
		// Always Function Enabled
		if ((CV33_43[i + 2] & 0xC0) == 0) {
			// Direction Control Disabled
			funcPortStat |= (1 << i);
		} else {
			// Direction Control Enabled
			dirFlag = readDirectionFlag();
			if (readDirectionReverse()) {
				if (dirFlag == 2) {
					dirFlag = 1;
				} else {
					dirFlag = 2;
				}
			}
				
			// Function Enable Flag Control
			if (CV33_43[i + 2] & 0x80) {
				if (dirFlag == 2) {
					funcPortStat |= (1 << i);
				} else {
					funcPortStat &= ~(1 << i);
				}
			} else if (CV33_43[i + 2] & 0x40) {
				if (dirFlag == 1) {
					funcPortStat |= (1 << i);
				} else {
					funcPortStat &= ~(1 << i);
				}
			}
		}
			
		// Function Status Counter
		if ((funcPortStat & (1 << i)) != (oldFuncPortStat & (1 << i))) {
			funcStatusCount[i] = 1;
			if (funcPortStat & (1 << i)) {
				oldFuncPortStat |= (1 << i);
			} else {
				oldFuncPortStat &= ~(1 << i);
			}
		}
	}
	
	if (funcPortStat & mask) {
		// Function Enable
		funcSetPort2(i, funcCount);
	} else {
		// Function Disable
		if (funcVirtualStat & 0x04) {
			// Override Headlight Always On
			funcSetPort2(i, funcCount);
		} else if (funcVirtualStat & 0x02) {
			// Override Taillight
			if ((CV112_CV122[i + 2] & 0x0F) == 0x02) {
				// Taillight Right, Always Off
				HSfuncValue[i] = 0;
				HSclkFuncUseFlag &= ~(1 << i);
				funcPortCtrl(i, 0);
			} else if ((CV112_CV122[i + 2] & 0x0F) == 0x03) {
				// Taillight Left, Always On
				funcSetPort2(i, funcCount);
			} else {
				HSfuncValue[i] = 0;
				HSclkFuncUseFlag &= ~(1 << i);
				funcPortCtrl(i, 0);
			}
		//	funcSetPort2(i, funcCount);
		} else {
			if (i == 7) {
				motorFuncDriver(0, 0);
			} else {
				HSfuncValue[i] = 0;
				HSclkFuncUseFlag &= ~(1 << i);
				funcPortCtrl(i, 0);
			}
		}
	}
	
}

void clockReceiverFuncCtrl(void)
{
	//uint8_t i, mask, dirFlag;
	uint8_t i;
	
	if (readFuncProgMode()) return;
	
	funcCount++;
	if (funcCount > FUNC_COUNT_MAX) funcCount = 0;
	
	for (i = 0; i < 9; i++) {
		if (funcStatusCount[i] != 0) {
			funcStatusCount[i]++;
		}
	}
	
	
}

void funcCtrlAnalog(uint8_t direction) {
	
	uint8_t i, mask, newFuncPortStat;
	mask = 0x01;
	newFuncPortStat = 0;
	
	for (i = 0; i < 8; i++) {
		if ((CV33_43[i + 2] & 0xC0) != 0) {
			if (((CV33_43[i + 2] & 0xC0) == 0x40) && (direction == 1)) {
				newFuncPortStat |= CV50 & mask;
			} else if (((CV33_43[i + 2] & 0xC0) == 0x80) && (direction == 2)) {
				newFuncPortStat |= CV50 & mask;
			}
		} else {
			newFuncPortStat |= CV50 & mask;
		}
		mask = mask << 1;
	}
	
	funcSetDirection = direction;
	if ((CV50 & 0x80) && (CV33_43[10] == 0x01)) {
		newFuncPortStat |= 0x80;
	}
	
	
	/*
	if (CV33_43[10] == 0x01) {
		funcPortStat = 0x0180 + newFuncPortStat;
	} else {
		funcPortStat = newFuncPortStat;
	}
	*/
	funcPortStat = newFuncPortStat;
}

void funcCtrl(uint8_t group, uint8_t funcData, uint8_t direction)	// F13-F20, F21-F28
{
	uint8_t i, funcTable, min, max, mask;
	
	if (group == 1) {
		if (funcTable1 == 0) return;
		funcTable = funcTable1;
		min = 1;
		max = 4;
	} else if (group == 2) {
		if (funcTable2 == 0) return;
		funcTable = funcTable2;
		min = 5;
		max = 8;
	} else if (group == 3) {
		if (funcTable3 == 0) return;
		funcTable = funcTable3;
		min = 9;
		max = 12;
	} else if (group == 4) {
		if (funcTable4 == 0) return;
		funcTable = funcTable4;
		min = 13;
		max = 20;
	} else if (group == 5) {
		if (funcTable5 == 0) return;
		funcTable = funcTable5;
		min = 21;
		max = 28;
	} else {
		return;
	}
	
	if (readDirectionReverse()) {
		if (direction == 1) {
			direction = 2;
		} else {
			direction = 1;
		}
	}
	
	mask = 0x01;
	for (i = min; i <= max; i++) {
		if (funcTable & mask) funcSet(i, funcData & mask, direction);
		mask = mask << 1;
	}
	
	if (group == 1) {
		// F0
		if (funcTable1 & 0x10) funcSet(0, funcData & 0x10, direction);
	}
	
}

void funcSet(uint8_t funcNum, uint8_t stat, uint8_t direction)
{
	/*
	funcSetVirtual(0, CV33, funcNum, stat);
	funcSetVirtual(1, CV34, funcNum, stat);
	funcSetPort(0, CV35, funcNum, stat, direction);
	funcSetPort(1, CV36, funcNum, stat, direction);
	funcSetPort(2, CV37, funcNum, stat, direction);
	*/
	
	funcSetDirection = direction;
	
	funcSetVirtual(0, CV33_43[0], funcNum, stat);			// Headlight Dimming
	funcSetVirtual(1, CV33_43[1], funcNum, stat);			// Taillight Yard Operation
	funcSetPort(0, CV33_43[2], funcNum, stat, direction);	// Func1
	funcSetPort(1, CV33_43[3], funcNum, stat, direction);	// Func2
	funcSetPort(2, CV33_43[4], funcNum, stat, direction);	// Func3
	funcSetPort(3, CV33_43[5], funcNum, stat, direction);	// Func4
	funcSetPort(4, CV33_43[6], funcNum, stat, direction);	// Func5
	funcSetPort(5, CV33_43[7], funcNum, stat, direction);	// Func6
	funcSetPort(6, CV33_43[8], funcNum, stat, direction);	// Func7
	funcSetPortMotor(CV33_43[9], funcNum, stat, direction);	// Func8
	funcSetVirtual(3, CV44, funcNum, stat);					// Headlight Always On Backward
	funcSetVirtual(2, CV45, funcNum, stat);					// Headlight Always On Forward
	funcSetVirtual(6, CV51, funcNum, stat);					// Automatic Brake Ignore Function
	funcSetVirtual(7, CV60_64[3], funcNum, stat);			// Slow Speed Mode for Yard Operation (CV63)
}

void funcSetVirtual(uint8_t funcID, uint8_t cv, uint8_t funcNum, uint8_t stat)
{
	if ((cv & 0x1F) == funcNum) {
		if (stat) {
			funcVirtualStat |= (1 << funcID);
		} else {
			funcVirtualStat &= ~(1 << funcID);
		}
	}
}

void funcSetPort(uint8_t funcPort, uint8_t cv, uint8_t funcNum, uint8_t stat, uint8_t direction)
{
	uint8_t stat2;
	
	if ((cv & 0x1F) == funcNum) {
		// Check Direction
		stat2 = 0;
		if ((cv & 0xC0) == 0) {
			if (stat) stat2 = 1;
		} else if ((cv & 0xC0) == 0x40) {
			if ((direction == 1) && stat) stat2 = 1;
		} else if ((cv & 0xC0) == 0x80) {
			if ((direction == 2) && stat) stat2 = 1;
		}
		
		if (stat2) {
			funcPortStat |= (1 << funcPort);
			if (~oldFuncPortStat & (1 << funcPort)) {
				funcStatusCount[funcPort] = 1;
				HSfuncValue[funcPort] = 0;
				oldFuncPortStat |= (1 << funcPort);
			}
		} else {
			funcPortStat &= ~(1 << funcPort);
			if (oldFuncPortStat & (1 << funcPort)) {
				funcStatusCount[funcPort] = 1;
				HSfuncValue[funcPort] = 0;
				HSclkFuncUseFlag &= ~(1 << funcPort);
				oldFuncPortStat &= ~(1 << funcPort);
			}
		}
	}
}

void funcSetPortMotor(uint8_t cv, uint8_t funcNum, uint8_t stat, uint8_t direction)
{
	if (CV33_43[10] != 1) return;
	
	if ((cv & 0x1F) == funcNum) {
		if (stat) {
			funcPortStat |= 0x80;
			if (~oldFuncPortStat & 0x80) {
				funcStatusCount[7] = 1;
				HSfuncValue[7] = 0;
				oldFuncPortStat |= 0x80;
			}
		} else {
			funcPortStat &= 0x7F;
			if (oldFuncPortStat & 0x80) {
				funcStatusCount[7] = 1;
				HSfuncValue[7] = 0;
				HSclkFuncUseFlag &= 0x7F;
				oldFuncPortStat &= 0x7F;
			}
		}
	}
}

void funcSetPort2(uint8_t funcPort, uint8_t count)
{
	uint8_t cvType;
	uint8_t cvType2;
	uint8_t illuminateValue;
	uint8_t ctrlOverride;
	uint8_t direction;
	uint8_t funcStat;
	
	if (funcPort == 7) {
		/*
		if (readDirectionReverse()) {
			if (funcSetDirection == 1) {
				direction = 2;
			} else {
				direction = 1;
			}
		} else if (funcVirtualStat & 0x04) {
			direction = 1;
		} else {
			direction = funcSetDirection;
		}
		*/
		direction = funcSetDirection;
		
		if (direction == 2) {
			cvType = CV112_CV122[10];
		} else {
			cvType = CV112_CV122[9];
		}
	} else if ((funcPort >= 0) && (funcPort < 7)) {
		direction = funcSetDirection;
		cvType = CV112_CV122[funcPort + 2];
	} else {
		return;
	}
	
	funcStat = funcPortStat & (1 << funcPort);
	//if (funcStat == 0) return;
	
	
	if ((cvType & 0x0F) == 0x01) {
		// Headlight
		cvType2 = (CV112_CV122[0] & 0xF0) + 0x01;
		if (funcVirtualStat & 0x04) {
			// Headlight Only (Forward)
			illuminateValue = funcEffect(funcStat, cvType2, count, &funcStatusCount[funcPort], &ctrlOverride, &HSfuncValue[funcPort]);
		} else if (funcVirtualStat & 0x08) {
			// Headlight Only (Backward)
			illuminateValue = funcEffect(funcStat, cvType2, count, &funcStatusCount[funcPort], &ctrlOverride, &HSfuncValue[funcPort]);
		} else if (funcVirtualStat & 0x01) {
			// Headlight Dimming
			illuminateValue = funcEffect(funcStat, cvType2, count, &funcStatusCount[funcPort], &ctrlOverride, &HSfuncValue[funcPort]);
		} else {
			//if (readSpdFlag()) {
			if (get_speed_8bit()) {
				illuminateValue = funcEffect(funcStat, cvType, count, &funcStatusCount[funcPort], &ctrlOverride, &HSfuncValue[funcPort]);
			} else {
				illuminateValue = funcEffect(funcStat, cvType2, count, &funcStatusCount[funcPort], &ctrlOverride, &HSfuncValue[funcPort]);
			}
		}
	} else if ((cvType & 0x0F) == 0x02) {
		// Taillight Right
		if (funcVirtualStat & 0x04) {
			// Headlight Only, always off
			HSfuncValue[funcPort] = 0;
			HSclkFuncUseFlag &= ~(1 << funcPort);
			funcPortCtrlBridge(funcPort, 0, direction);
			return;
		} else if (funcVirtualStat & 0x08) {
			// Headlight Only, always off
			HSfuncValue[funcPort] = 0;
			HSclkFuncUseFlag &= ~(1 << funcPort);
			funcPortCtrlBridge(funcPort, 0, direction);
			return;	
		} else if (funcVirtualStat & 0x02) {
			// Taillight Yard Operation, always off
			HSfuncValue[funcPort] = 0;
			HSclkFuncUseFlag &= ~(1 << funcPort);
			funcPortCtrlBridge(funcPort, 0, direction);
			return;
		}
		illuminateValue = funcEffect(funcStat, cvType, count, &funcStatusCount[funcPort], &ctrlOverride, &HSfuncValue[funcPort]);
	} else if ((cvType & 0x0F) == 0x03) {
		// Taillight left
		if (funcVirtualStat & 0x04) {
			// Headlight Only, always off
			HSfuncValue[funcPort] = 0;
			HSclkFuncUseFlag &= ~(1 << funcPort);
			funcPortCtrlBridge(funcPort, 0, direction);
			return;
		} else if (funcVirtualStat & 0x08) {
			// Headlight Only, always off
			HSfuncValue[funcPort] = 0;
			HSclkFuncUseFlag &= ~(1 << funcPort);
			funcPortCtrlBridge(funcPort, 0, direction);
			return;
		} else if (funcVirtualStat & 0x03) {
			// Taillight Yard Operation, always on
			illuminateValue = funcEffect(funcStat, cvType, count, &funcStatusCount[funcPort], &ctrlOverride, &HSfuncValue[funcPort]);
		} else {
			illuminateValue = funcEffect(funcStat, cvType, count, &funcStatusCount[funcPort], &ctrlOverride, &HSfuncValue[funcPort]);
		}
	} else {
		illuminateValue = funcEffect(funcStat, cvType, count, &funcStatusCount[funcPort], &ctrlOverride, &HSfuncValue[funcPort]);
		
	}
	
	if (ctrlOverride) {
		/*
		if (illuminateValue) {
			funcPortCtrlBridge(funcPort, 1, direction);
		} else {
			funcPortCtrlBridge(funcPort, 0, direction);
		}
		*/
		HSfuncValue[funcPort] = illuminateValue;
		HSclkFuncUseFlag |= (1 << funcPort);
	} else {
		/*
		if (count > illuminateValue) {
			funcPortCtrlBridge(funcPort, 0, direction);
		} else {
			funcPortCtrlBridge(funcPort, 1, direction);
		}
		
		HSclkFuncUseFlag &= ~(1 << funcPort);
		*/
		HSfuncValue[funcPort] = illuminateValue;
		HSclkFuncUseFlag |= (1 << funcPort);
	}
}

void funcPortCtrlBridge(uint8_t funcNum, uint8_t funcStat, uint8_t direction) {
	if (funcNum < 7) {
		funcPortCtrl(funcNum, funcStat);
		return;
	}
	
	if (funcNum == 7) {
		// MotorDriver
		
		motorFuncDriver(funcStat, direction);
	}
}

uint8_t getYardModeStat(void)
{
	if (funcVirtualStat & 0x80) {
		return (1);
	}
	return (0);
}

uint8_t ignoreABCstat(void)
{
	if (funcVirtualStat & 0x40) {
		return (1);
	}
	return (0);
}