/*
 * func_ctrl.h
 *
 * Created: 2020/10/02 22:07:48
 *  Author: Y.Tsurui
 */ 

void HSclockReceiverFuncCtrl(void);
void clockReceiverFuncCtrlSub(uint8_t i);
void clockReceiverFuncCtrl(void);
void funcCtrlAnalog(uint8_t direction);
void funcCtrl1(uint8_t funcGroup, uint8_t funcData, uint8_t direction);
void funcCtrl2(uint8_t funcGroup, uint8_t funcData, uint8_t direction);
void funcCtrl3(uint8_t funcGroup, uint8_t funcData, uint8_t direction);
void funcCtrl4(uint8_t funcGroup, uint8_t funcData, uint8_t direction);
void funcCtrl5(uint8_t funcGroup, uint8_t funcData, uint8_t direction);

void funcCtrl(uint8_t group, uint8_t funcData, uint8_t direction);

uint8_t getYardModeStat(void);
uint8_t ignoreABCstat(void);