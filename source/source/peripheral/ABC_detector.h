/*
 * ABC_detector.h
 *
 * Created: 2021/02/04 0:05:41
 *  Author: Y.Tsurui
 */ 

#ifndef NO_ABC

void initABCpoller(void);
void ABCmsEvent(void);

uint8_t getABCrightCount(void);
uint8_t getABCleftCount(void);

void ABCcheckRight(void);
void ABCcheckLeft(void);

//void ABCclkReceiver(void);
uint8_t getABCstatus(void);

#define ABC_STAT_NONE 0x00
#define ABC_STAT_EN_FOR  0x01
#define ABC_STAT_EN_REV  0x02

#endif