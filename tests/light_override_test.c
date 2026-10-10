#include <assert.h>
#include <string.h>
#include FUNC_SOURCE
#include EFFECT_SOURCE
static uint8_t speed, motorDirection, motorOn, ports[8];
uint8_t readDirectionReverse(void) { return 0; }
uint8_t readDirectionFlag(void) { return funcSetDirection; }
uint8_t readFuncProgMode(void) { return 0; }
uint8_t readAnalogStat(void) { return 0; }
uint8_t readMotorDirection(void) { return 0; }
uint8_t get_speed_8bit(void) { return speed; }
void funcPortCtrl(uint8_t p, uint8_t v) { ports[p] = v; }
void motorFuncDriver(uint8_t s, uint8_t d) { motorOn=s; motorDirection=d; }
static void reset(void) {
 memset(CV33_43,30,sizeof(CV33_43));
 memset(CV112_CV122,0xf0,sizeof(CV112_CV122));
 memset(HSfuncValue,0,sizeof(HSfuncValue));
 memset(funcStatusCount,0,sizeof(funcStatusCount));
 funcVirtualStat=funcPortStat=oldFuncPortStat=HSclkFuncUseFlag=0;
 CV44=5;CV45=6;CV123=0x87;CV124=0x38;CV33_43[10]=1;
 CV112_CV122[0]=0x40;CV60_64[3]=30;
 funcSetDirection=1; speed=100;
 funcTable1=0x1f;funcTable2=0xf;funcTable3=0xf;funcTable4=funcTable5=0xff;
}
static void pwm(void) { HSfuncStageFlag=7;HSclkCounter=0;HSclockReceiverFuncCtrl(); }
int main(void) {
 for(int cv=0;cv<2;cv++) for(int port=0;port<7;port++)
 for(int dir=1;dir<=2;dir++) for(int on=0;on<2;on++) {
  reset();CV123=cv?0:1<<port;CV124=cv?1<<port:0;
  CV33_43[port+2]=0x80;CV112_CV122[port+2]=0xf1;
  funcSet(0,on,dir);funcCtrl(2,cv?2:1,dir);
  for(speed=0;speed<2;speed++) { clockReceiverFuncCtrlSub(port); assert(HSfuncValue[port]==0x4f); }
  funcSet(0,!on,dir); /* Changes while overridden must be retained. */
  clockReceiverFuncCtrlSub(port);assert(HSfuncValue[port]==0x4f);
  funcCtrl(2,0,dir);funcStatusCount[port]=0;clockReceiverFuncCtrlSub(port);
  assert(HSfuncValue[port]==((!on && dir==2)?0xff:0));
 }
 for(int type=0;type<=6;type++) {
  reset();CV123=1;CV112_CV122[2]=0xf0|type;funcSet(5,1,1);
  clockReceiverFuncCtrlSub(0);assert(HSfuncValue[0]==(type==1?0x4f:0));
  funcPortStat=1;funcStatusCount[0]=0;clockReceiverFuncCtrlSub(0);
  assert(HSfuncValue[0]==(type==1?0x4f:(type==2||type==3)?0:0xff));
 }
 reset();CV123=CV124=1;CV112_CV122[2]=0xf1;
 funcSet(5,1,1);funcSet(6,1,1);funcSet(5,0,1);clockReceiverFuncCtrlSub(0);assert(HSfuncValue[0]==0x4f);
 funcSet(6,0,1);clockReceiverFuncCtrlSub(0);assert(HSfuncValue[0]==0);
 reset();CV123=0;CV112_CV122[2]=0xf1;funcSet(5,1,1);clockReceiverFuncCtrlSub(0);assert(HSfuncValue[0]==0);
 for(int forward=1;forward<=3;forward++) for(int backward=1;backward<=3;backward++) {
  reset();CV112_CV122[9]=0xf0|forward;CV112_CV122[10]=0xf0|backward;
  funcSet(5,1,2);clockReceiverFuncCtrlSub(7);pwm();
  if(forward==1||backward==1) { assert(motorOn);assert(motorDirection==(forward==1?1:2));assert(HSfuncValue[7]==0x4f); }
  else { assert(!motorOn);assert(HSfuncValue[7]==0); }
  funcSet(5,0,2);clockReceiverFuncCtrlSub(7);pwm();assert(!motorOn);assert(HSfuncValue[7]==0);
 }
 reset();CV33_43[10]=0;CV112_CV122[9]=0xf1;funcSet(5,1,1);clockReceiverFuncCtrlSub(7);assert(HSfuncValue[7]==0);
 /* Every supported override F number, including F0 and F28. */
 for(int f=0;f<=28;f++) {
  reset();CV44=f;CV45=30;CV123=1;CV112_CV122[2]=0xf1;
  uint8_t group=f<=4?1:f<=8?2:f<=12?3:f<=20?4:5;
  uint8_t bit=f==0?4:f<=4?f-1:f<=8?f-5:f<=12?f-9:f<=20?f-13:f-21;
  funcCtrl(group,1<<bit,2);clockReceiverFuncCtrlSub(0);assert(HSfuncValue[0]==0x4f);
  funcCtrl(group,0,2);clockReceiverFuncCtrlSub(0);assert(HSfuncValue[0]==0);
 }
 /* Ports outside the active group retain their normal tail/head lighting. */
 for(int type=1;type<=3;type++) {
  reset();CV123=0;CV112_CV122[2]=0xf0|type;funcPortStat=1;
  funcSet(5,1,1);clockReceiverFuncCtrlSub(0);assert(HSfuncValue[0]==0xff);
 }
 /* Zero brightness must physically extinguish a previously lit output. */
 reset();CV112_CV122[2]=0xf1;CV112_CV122[0]=0;ports[0]=1;
 funcSet(5,1,1);clockReceiverFuncCtrlSub(0);assert(ports[0]==0);
 return 0;
}
