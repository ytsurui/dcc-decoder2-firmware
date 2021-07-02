/*
 * funcport.h
 *
 * Created: 2021/02/06 19:03:08
 *  Author: Y.Tsurui
 */ 


void initFuncPort(void);
void funcPortCtrl(uint8_t funcNum, uint8_t funcStat);

uint8_t readDirectionReverse(void);
uint8_t readFuncProgMode(void);
void funcPortProgACK(uint8_t stat);
#define FUNC_PROG_ACK_ON	1
#define FUNC_PROG_ACK_OFF	2
