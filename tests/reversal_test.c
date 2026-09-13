#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <avr/io.h>
Port PORTA, PORTB, PORTC; Timer TCA0; Adc ADC0; struct Vref VREF;
/* Observe bridge changes while keeping the real PWM implementation. */
#define pwmSetDirection hardwareSetDirection
#include MOTOR_SOURCE
#undef pwmSetDirection
static uint8_t outputDirection, enforceStoppedBridge;
void pwmSetDirection(uint8_t direction) {
    if (enforceStoppedBridge && direction != outputDirection) {
        assert(nowSPDvalue == 0 && TCA0.SINGLE.CMP0 == 0);
        assert(bemfSPDvalue == 0 && bemfReadFlag == 0);
    }
    hardwareSetDirection(direction);
    outputDirection = direction;
}
#include TRAIN_SOURCE
#include FUNC_SOURCE
static uint8_t analogMode, abcStatus, reverseBoard, packetDirection;
uint8_t readAnalogStat(void) { return analogMode; }
uint8_t analogDirection(void) { return 1; }
uint8_t analogDuty(void) { return 128; }
uint8_t getABCstatus(void) { return abcStatus; }
uint8_t readDirectionReverse(void) { return reverseBoard; }
uint8_t readDirectionFlag(void) { return packetDirection; }
uint8_t readFuncProgMode(void) { return 0; }
void funcPortCtrl(uint8_t port, uint8_t value) { (void)port; (void)value; }
uint8_t funcEffect(uint8_t stat, uint8_t type, uint8_t counter,
                   uint16_t *effect, uint8_t *override, uint8_t *hs) {
    (void)type; (void)counter; (void)effect; (void)override; (void)hs;
    return stat ? 255 : 0;
}
static void tick(unsigned count) {
    while (count--) {
        clockReceiverABCctrl();
        clock_receiver_train_ctrl();
        clockReceiverFuncCtrl();
        HSclockReceiverMotorCtrl();
    }
}
static void reset(unsigned speed) {
    analogMode=abcStatus=reverseBoard=0; packetDirection=1;
    memset(CV1_6,0,sizeof(CV1_6));
    memset(CV33_43,30,sizeof(CV33_43)); CV33_43[10]=0;
    memset(CV60_64,0,sizeof(CV60_64)); CV60_64[1]=95; CV60_64[2]=192;
    CV52=CV53=CV54=CV140=0; CV131=255;
    CV44=CV45=30; CV60_64[3]=30;
    CV55_57[0]=30; CV55_57[1]=10; CV55_57[2]=40; CV138=175;
    CV58=255; CV59=3;
    ABCworkedFlag=ABCdirectionReverseFlag=0;
    ABCautoReverseCount1=ABCautoReverseCount2=0;
    reversalState=REV_IDLE; reversalWait=reversalABC=0;
    requestedDirection=nowDirection=1; now_spd=target_spd=speed;
    spdAnalogFlag=0; pwm_cutout_timer=motorStartDelayCount=motorStartDelaySpd=0;
    resetRampClock();
    funcVirtualStat=funcPortStat=oldFuncPortStat=requestedFuncPorts=0;
    funcSetDirection=1; lastMotorLightDirection=1; funcCount=0; funcTable1=0x10;
    HSclkFuncUseFlag=0;
    memset(funcStatusCount,0,sizeof(funcStatusCount));
    pwmProgModeFlag=0; pwmSetSpeed(0); pwmSetSpeed(speed);
    enforceStoppedBridge=0; pwmSetDirection(1); enforceStoppedBridge=1;
    HSclockReceiverMotorCtrl();
}
static void command(uint8_t direction, uint8_t speed) {
    spdAnalogFlag=0;
    setspeedCommand(direction,speed,0);
}
static void testNormal(void) {
    reset(8); command(2,12);
    assert(outputDirection==1 && now_spd==8);
    for (unsigned spd=8;spd>0;--spd) {
        /* Packet retransmission cannot reset the ramp clock. */
        for (unsigned t=0;t<16;++t) { command(2,12); tick(1); }
        assert(now_spd==spd-1 && outputDirection==1);
    }
    assert(reversalState==REV_WAIT && TCA0.SINGLE.CMP0==0);
    tick(254); assert(outputDirection==1 && now_spd==0);
    tick(1); assert(outputDirection==2 && now_spd==0);
    tick(15); assert(now_spd==0);
    tick(1); assert(now_spd==1);
    tick(16*11); assert(now_spd==12 && reversalState==REV_IDLE);
    reset(4); CV1_6[2]=2; CV1_6[3]=3; command(2,6);
    tick(47); assert(now_spd==4); tick(1); assert(now_spd==3);
    tick(48*3+255+31); assert(now_spd==0 && outputDirection==2);
    tick(1); assert(now_spd==1);
    /* CV2 must not pull the deceleration back up on incoming packets. */
    reset(6); CV1_6[1]=20; command(2,30);
    for (unsigned i=0;i<96;++i) { command(2,30); tick(1); }
    assert(now_spd==0); tick(255+16); assert(now_spd==1);
    reset(2); CV1_6[3]=255; command(2,4);
    tick(16*255-1); assert(now_spd==2); tick(1); assert(now_spd==1);
}
static void testUpdates(void) {
    reset(8); command(2,12); tick(32); command(1,4);
    tick(16*6+255); assert(now_spd==0 && outputDirection==1);
    tick(16*4); assert(now_spd==4);
    reset(4); command(2,8); tick(64); command(1,6);
    tick(255+96); assert(now_spd==6 && outputDirection==1);
    reset(4); command(2,8); tick(64+255+32);
    assert(now_spd==2); command(1,5);
    tick(32+255+80); assert(now_spd==5 && outputDirection==1);
    for (unsigned phase=0;phase<3;++phase) {
        reset(4); command(2,8);
        tick(phase==0 ? 16 : phase==1 ? 64 : 64+255+32);
        command(2,0); tick(2000);
        assert(now_spd==0 && target_spd==0 && TCA0.SINGLE.CMP0==0);
    }
    for (unsigned phase=0;phase<3;++phase) {
        reset(4); command(2,8);
        tick(phase==0 ? 16 : phase==1 ? 64 : 64+255+32);
        setspeed_128step(1,1); tick(2000);
        assert(now_spd==0 && target_spd==0 && reversalState==REV_IDLE);
    }
    reset(0); CV140=5; command(1,30); assert(motorStartDelayCount==5);
    setspeed_128step(2,1); tick(1000);
    assert(now_spd==0 && motorStartDelayCount==0 && motorStartDelaySpd==0);
    reset(4); CV140=5; command(2,8); tick(64+255+16);
    assert(now_spd==1 && motorStartDelayCount==0);
}
static void testABC(void) {
    reset(4); abcStatus=1; ABCworkedFlag=1; CV53=4; CV52=1;
    ABCautoReverseCount1=999;
    command(2,8); tick(64+255);
    assert(now_spd==0 && outputDirection==2 && reversalState==REV_IDLE);
    assert(ABCdirectionReverseFlag==0);
    command(2,8); tick(63); assert(now_spd==0);
    tick(1); assert(now_spd==1);
    /* Already stopped by ABC: existing cutout and CV53 recovery, no new state. */
    reset(0); target_spd=8; abcStatus=1; ABCworkedFlag=1; CV53=4;
    command(2,8); assert(outputDirection==2 && reversalState==REV_IDLE);
    tick(255+63); assert(now_spd==0); tick(1); assert(now_spd==1);
    /* CV53=0 continues to use the existing CV3 fallback. */
    reset(4); abcStatus=1; ABCworkedFlag=1; CV1_6[2]=2;
    command(2,8); tick(64+255+31); assert(now_spd==0);
    tick(1); assert(now_spd==1);
    /* Autonomous ABC reversal retains its pre-existing path. */
    reset(0); target_spd=8; abcStatus=1; ABCworkedFlag=1; CV52=1;
    ABCautoReverseCount1=999;
    tick(1); assert(outputDirection==2 && ABCdirectionReverseFlag==1);
    command(2,8); assert(reversalState==REV_IDLE);
    /* A newly encountered ABC section can interrupt reverse acceleration. */
    reset(4); command(2,8); tick(64+255+32);
    abcStatus=2; tick(16); assert(now_spd==0);
}
static void testLightsAndModes(void) {
    reset(4); CV33_43[2]=0x40; CV33_43[3]=0x80;
    funcCtrl(1,0x10,1); assert((funcPortStat & 3)==1);
    command(2,8); funcCtrl(1,0x10,2);
    tick(64+254); assert((funcPortStat & 3)==1);
    tick(1); assert((funcPortStat & 3)==2 && funcSetDirection==2);
    /* A different function group arriving at the switch must not suppress F0 refresh. */
    reset(4); CV33_43[2]=0x40; CV33_43[3]=0x80;
    funcTable1=0x11;
    funcCtrl(1,0x10,1); command(2,8); tick(64+254);
    clock_receiver_train_ctrl();
    funcTable2=1; funcCtrl(2,1,2);
    assert(funcSetDirection==2);
    clockReceiverFuncCtrl(); assert((funcPortStat & 3)==2);
    /* Function OFF while slowing remains OFF after reversal. */
    reset(4); CV33_43[2]=0x40; CV33_43[3]=0x80;
    funcCtrl(1,0x10,1); command(2,8); funcCtrl(1,0,2);
    tick(64+255); assert((funcPortStat & 3)==0);
    /* Always-on direction-dependent lights follow the same motor direction. */
    reset(4); CV33_43[2]=0x5F; CV33_43[3]=0x9F;
    packetDirection=2; command(2,8);
    clockReceiverFuncCtrlSub(0); clockReceiverFuncCtrlSub(1);
    assert((funcPortStat & 3)==1);
    tick(64+255); clockReceiverFuncCtrlSub(0); clockReceiverFuncCtrlSub(1);
    assert((funcPortStat & 3)==2);
    /* Board direction mapping is applied once, not twice. */
    reset(4); reverseBoard=1; CV33_43[2]=0x40; CV33_43[3]=0x80;
    command(1,8); funcCtrl(1,0x10,1); assert((funcPortStat & 3)==1);
    tick(64+255); assert((funcPortStat & 3)==2);
    reset(4); CV33_43[10]=1; enforceStoppedBridge=0;
    command(2,8); funcCtrl(1,0x10,2);
    assert(reversalState==REV_IDLE && funcSetDirection==2);
    reset(4); command(2,8); analogMode=1; enforceStoppedBridge=0;
    setspeed_analog(1); assert(reversalState==REV_IDLE);
    /* Pure analog direction switching remains immediate. */
    reset(4); analogMode=1; enforceStoppedBridge=0;
    setspeed_analog(2); assert(outputDirection==2 && reversalState==REV_IDLE);
    funcCtrlAnalog(1); assert(funcSetDirection==1);
}
static void testSpeedDecoding(void) {
    reset(4); CV1_6[1]=20; CV1_6[4]=255; CV1_6[5]=127;
    setspeed_128step(1,0); assert(now_spd==0 && target_spd==0);
    reset(4); funcVirtualStat=0x80; CV131=20;
    setspeed_128step(2,1); assert(now_spd==0 && target_spd==0);
    reset(4); funcVirtualStat=0x80; CV131=20;
    setspeed_28step(2,1); assert(now_spd==0 && target_spd==0);
    reset(4); CV67_94[0]=9; setspeed_28step(2,2);
    tick(64+255+9*16); assert(now_spd==9 && outputDirection==2);
}
static void testPWMStop(void) {
    for (unsigned low=0;low<4;++low) {
        for (unsigned stage=1;stage<=101;stage+=10) {
            reset(10); CV60_64[0]=0x80|low;
            bemfReadFlag=stage; bemfSaveSpdValue=180; bemfSPDvalue=200;
            bemfIntegral=123; bemfPreviousError=40; TCA0.SINGLE.CMP0=200;
            pwmSetSpeed(0);
            assert(TCA0.SINGLE.CMP0==0 && bemfReadFlag==0 && bemfSPDvalue==0);
            assert(bemfIntegral==0 && bemfPreviousError==0);
            captureBEMF(); HSclockReceiverMotorCtrl();
            assert(TCA0.SINGLE.CMP0==0 && bemfReadFlag==0);
        }
    }
    reset(10); pwmProgMode(PWM_PROG_MODE_ON);
    pwmSetSpeed(0); assert(pwmProgModeFlag==1);
    pwmProgMode(PWM_PROG_MODE_OFF); assert(pwmProgModeFlag==0);
}
int main(void) {
    testNormal(); testUpdates(); testABC(); testLightsAndModes();
    testSpeedDecoding(); testPWMStop();
    puts("PASS: reversal timing, CV rates, command updates/stops, CV140, ABC recovery/auto, lights, CV43, analog isolation, speed decoding, BEMF stop");
}
