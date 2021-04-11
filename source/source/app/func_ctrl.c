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
uint8_t funcPortStat;

uint8_t funcSetDirection;


uint8_t funcCount = 0;
uint8_t funcCount2 = 0;
#define FUNC_COUNT_MAX	0x0F

void clockReceiverFuncCtrl(void)
{
	uint8_t i, mask, dirFlag;
	
	if (readFuncProgMode()) return;
	
	funcCount++;
	if (funcCount > FUNC_COUNT_MAX) funcCount = 0;
	
	mask = 0x01;
	for (i = 0; i < 8; i++) {
		if (i == 7) {
			if (CV33_43[10] != 0x01) {
				// MotorDriver Function
				continue;
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
		}
		/*
		if (funcVirtualStat & 0x02) {
			// Taillight Yard Operation
			if ((CV112_CV122[i + 2] & 0x0F) == 0x02) {
				// Taillight Right, Always Off
				funcPortCtrl(i, 0);
			} else if ((CV112_CV122[i + 2] & 0x0F) == 0x03) {
				// Taillight Left, Always On
				
			}
		}
		*/
		
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
					funcPortCtrl(i, 0);
				} else if ((CV112_CV122[i + 2] & 0x0F) == 0x03) {
					// Taillight Left, Always On
					funcSetPort2(i, funcCount);
				} else {
					funcPortCtrl(i, 0);
				}
			//	funcSetPort2(i, funcCount);
			} else {
				if (i == 7) {
					motorFuncDriver(0, 0);
				} else {
					funcPortCtrl(i, 0);
				}
			}
		}
		mask = mask << 1;
	}
	
	
	
}

void funcCtrlAnalog(uint8_t direction) {
	/*
	if (direction == 1) {
		funcPortStat = 0b00000001;
	} else {
		funcPortStat = 0b00000010;
	}
	*/
	
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
	funcSetVirtual(7, CV63, funcNum, stat);					// Slow Speed Mode for Yard Operation
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
		} else {
			funcPortStat &= ~(1 << funcPort);
		}
	}
}

void funcSetPortMotor(uint8_t cv, uint8_t funcNum, uint8_t stat, uint8_t direction)
{
	if (CV33_43[10] != 1) return;
	
	if ((cv & 0x1F) == funcNum) {
		if (stat) {
			funcPortStat |= 0x80;
		} else {
			funcPortStat &= 0x7F;
		}
	}
}

void funcSetPort2(uint8_t funcPort, uint8_t count)
{
	uint8_t cvType;
	uint8_t illuminateValue;
	uint8_t direction;
	
	if (funcPort == 7) {
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
	
	
	if ((cvType & 0x0F) == 0x01) {
		// Headlight
		if (funcVirtualStat & 0x04) {
			// Headlight Only (Forward)
			illuminateValue = (CV112_CV122[0] & 0xF0) >> 4;
		} else if (funcVirtualStat & 0x08) {
			// Headlight Only (Backward)
			illuminateValue = (CV112_CV122[0] & 0xF0) >> 4;
		} else if (funcVirtualStat & 0x01) {
			// Headlight Dimming
			illuminateValue = (CV112_CV122[0] & 0xF0) >> 4;
		} else {
			//if (readSpdFlag()) {
			if (get_speed_8bit()) {
				illuminateValue = (cvType & 0xF0) >> 4;
			} else {
				illuminateValue = (CV112_CV122[0] & 0xF0) >> 4;
			}
		}
	} else if ((cvType & 0x0F) == 0x02) {
		// Taillight Right
		if (funcVirtualStat & 0x04) {
			// Headlight Only, always off
			funcPortCtrlBridge(funcPort, 0, direction);
			return;
		} else if (funcVirtualStat & 0x08) {
			// Headlight Only, always off
			funcPortCtrlBridge(funcPort, 0, direction);
			return;	
		} else if (funcVirtualStat & 0x02) {
			// Taillight Yard Operation, always off
			funcPortCtrlBridge(funcPort, 0, direction);
			return;
		}
		illuminateValue = (cvType & 0xF0) >> 4;
	} else if ((cvType & 0x0F) == 0x03) {
		// Taillight left
		if (funcVirtualStat & 0x04) {
			// Headlight Only, always off
			funcPortCtrlBridge(funcPort, 0, direction);
			return;
		} else if (funcVirtualStat & 0x08) {
			// Headlight Only, always off
			funcPortCtrlBridge(funcPort, 0, direction);
			return;
		} else if (funcVirtualStat & 0x03) {
			// Taillight Yard Operation, always on
			//funcPortCtrl(funcPort, 1);
			illuminateValue = (cvType & 0xF0) >> 4;
		} else {
			illuminateValue = (cvType & 0xF0) >> 4;
		}
	} else {
		illuminateValue = (cvType & 0xF0) >> 4;
		
	}
	
	
	if (count > illuminateValue) {
		funcPortCtrlBridge(funcPort, 0, direction);
	} else {
		funcPortCtrlBridge(funcPort, 1, direction);
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