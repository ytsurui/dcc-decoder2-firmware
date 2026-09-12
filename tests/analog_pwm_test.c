/* Execute the production C with mock AVR registers. No ISR latency simulation. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <avr/io.h>
MockPort PORTA;
MockTimer TCB0;
MockTca TCA0;
#include ANALOG_SOURCE
#ifndef TEST_FUNCTION_DECODER
#include TRAIN_SOURCE
#endif
#include DCC_SOURCE

static uint8_t outputSpeed, outputDirection;
void railcomChannel1AddrSend(void) {}
void pwmSetSpeed(uint8_t speed) { outputSpeed = speed; }
void pwmSetDirection(uint8_t dir) { outputDirection = dir; }
uint8_t readDirectionReverse(void) { return 0; }
uint8_t getYardModeStat(void) { return 0; }
uint8_t getABCstatus(void) { return 0; }
uint8_t ignoreABCstat(void) { return 0; }

static void clearFlags(void) {
    /* Real AVR registers clear these bits on write-one, unlike the mock. */
    TCB0.INTFLAGS = PORTA.INTFLAGS = 0;
}
static void advance(uint32_t ticks) {
    while (ticks >= 0xD000U - TCB0.CNT) {
        ticks -= 0xD000U - TCB0.CNT;
        TCB0.CNT = 0;
        TCB0.INTFLAGS = TCB_CAPT_bm;
        TCB0_INT_vect();
        clearFlags();
    }
    TCB0.CNT += ticks;
}
static void edge(uint8_t pins) {
    PORTA.IN = pins;
    PORTA_PORT_vect();
    clearFlags();
}
static void resetInput(uint8_t pins) {
    memset(&TCB0, 0, sizeof(TCB0));
    PORTA.IN = pins;
    initDCCpoller();
    clearFlags();
    oldInputPortStat = oldPortStatFlag = 0;
    assert(analogDuty() == 0 && analogDirection() == 0);
}
static void publish(void) {
    for (unsigned i = 0; i < EXPECT_WINDOW_CLOCKS; ++i) analogPollerClkReceiver();
    clearFlags();
}
static void pwmWindow(uint8_t pins, unsigned period, unsigned on) {
    /* 500 ms at 5 MHz. The last partial PWM period is deliberately retained. */
    uint32_t remaining = 2500000;
    while (remaining) {
        uint32_t step;
        if (on) {
            edge(pins);
            step = remaining < on ? remaining : on;
            advance(step); remaining -= step;
        }
        if (on < period && remaining) {
            edge(0);
            step = remaining < period-on ? remaining : period-on;
            advance(step); remaining -= step;
        }
    }
    publish();
}
static void testInputs(void) {
    for (unsigned dir = 1; dir <= 2; ++dir) {
        uint8_t pins = dir == 1 ? PIN2_bm : PIN4_bm;
        for (unsigned width = 0; width <= 250; ++width) {
            resetInput(0);
            pwmWindow(pins, 250, width); /* 20 kHz; exhaustive 0.2 us widths */
            assert(analogDuty() == (width * 255U + 125U) / 250U);
            assert(analogDirection() == (width ? dir : 0));
        }
        for (unsigned period = 227; period <= 278; period += 17) {
            resetInput(0);
            pwmWindow(pins, period, period/3);
            int expected = (period/3 * 255U + period/2)/period;
            assert((int)analogDuty() >= expected-1 && (int)analogDuty() <= expected+1);
        }
        resetInput(pins); /* DC present before initialization: no edges at all */
        advance(2500000); publish();
        assert(analogDuty() == 255 && analogDirection() == dir);
        edge(0); advance(2500000); publish();
        assert(analogDuty() == 0 && analogDirection() == dir);
    }
    resetInput(PIN2_bm | PIN4_bm);
    advance(2500000); publish();
    assert(analogDuty() == 0 && analogDirection() == 0);
    resetInput(PIN2_bm);
    advance(1250000); edge(PIN4_bm); advance(1250000); publish();
    assert(analogDuty() == 0 && analogDirection() == 0);
    advance(2500000); publish();
    assert(analogDuty() == 255 && analogDirection() == 2);
    /* RTC trigger, using actual elapsed time even when notification is late. */
    resetInput(PIN2_bm); advance(3000000);
    for (unsigned i=0;i<EXPECT_WINDOW_CLOCKS-1;++i) analogPollerClkReceiver();
    assert(analogDuty() == 0);
    analogPollerClkReceiver(); clearFlags();
    assert(analogDuty() == 255);
    /* PORTA handles a pending wrap before TCB's ISR. */
    resetInput(PIN2_bm); advance(0xCFFF);
    TCB0.CNT=7; TCB0.INTFLAGS=TCB_CAPT_bm;
    edge(0);
    TCB0_INT_vect(); clearFlags(); /* no double counting when flag was cleared */
    advance(0xD007); publish();
    assert(analogDuty() == 128);
    /* Long gaps remain DC and unsigned time arithmetic crosses 32-bit wrap. */
    resetInput(PIN2_bm);
    timerBase = 0xFFFFF000UL; windowStart = timerBase;
    advance(2500000); publish(); assert(analogDuty()==255);
    resetInput(PIN2_bm);
    advance(20000000); publish(); assert(analogDuty()==255);
}
#ifndef TEST_FUNCTION_DECODER
static void resetTrain(void) {
    target_spd=now_spd=0; nowDirection=1; pwm_cutout_timer=0;
    motorStartDelayCount=0; CV140=0;
    clock_recv_counter=Rate_counter=0;
    memset(CV1_6,0,sizeof(CV1_6));
    CV58=255; CV59=3; outputSpeed=0;
}
static void testTrain(void) {
    resetInput(0); resetTrain();
    setspeed_analog(analogDirection()); assert(target_spd==0 && outputSpeed==0);
    pwmWindow(PIN2_bm,250,125);
    for (unsigned cv=0;cv<=255;++cv) {
        resetTrain(); CV58=cv;
        setspeed_analog(analogDirection());
        assert(target_spd == (cv*128U+127U)/255U);
    }
    resetTrain(); setspeed_analog(analogDirection());
    for (unsigned i=0;i<47;++i) clock_receiver_train_ctrl();
    assert(outputSpeed==0);
    clock_receiver_train_ctrl(); assert(outputSpeed==1);
    edge(0); advance(2500000); publish();
    setspeed_analog(analogDirection()); assert(target_spd==0);
    for (unsigned i=0;i<48;++i) clock_receiver_train_ctrl();
    assert(outputSpeed==0);
    /* Analog target 1 is a speed, digital target 1 remains emergency stop. */
    resetInput(PIN2_bm); advance(2500000); publish(); resetTrain();
    CV58=1; setspeed_analog(1); assert(target_spd==1);
    for (unsigned i=0;i<48;++i) clock_receiver_train_ctrl();
    assert(outputSpeed==1);
    spdAnalogFlag=0; setspeed(1,1); assert(target_spd==0 && outputSpeed==0);
}
#endif
static uint8_t received[48], receivedLen;
static void dccBit(unsigned bit) {
    /* Actual PORTA ISR and decoder, 58/100 us half-bits. */
    edge(PIN4_bm); advance(bit ? 290 : 500);
    edge(PIN2_bm);
    dccPacketShifter(&receivedLen, received);
    advance(bit ? 290 : 500);
}
static void testDcc(void) {
    resetInput(0);
    dccTimeoutCounter=800;
    for(unsigned i=0;i<12;++i) dccBit(1);
    dccBit(0);
    uint8_t packet[]={3,0x60,0x63};
    for(unsigned i=0;i<3;++i) {
        for(int bit=7;bit>=0;--bit) dccBit((packet[i]>>bit)&1);
        dccBit(i==2);
    }
    assert(receivedLen==3 && memcmp(packet,received,3)==0);
    assert(readAnalogStat()==0);
    publish(); assert(analogDuty()==0); /* mixed DCC polarity never becomes speed */

#ifndef TEST_FUNCTION_DECODER
    CV29=0x08;
    dccTimeoutCounter=800;
    railcomCutoutFlag=0; TCA0.SINGLE.INTCTRL=0;
    edge(0); assert((TCA0.SINGLE.INTCTRL & TCA_SINGLE_CMP2_bm)==0);
    dccTimeoutCounter=0;
    edge(PIN2_bm); edge(0);
    assert(TCA0.SINGLE.INTCTRL & TCA_SINGLE_CMP2_bm);
    CV29=0;
#endif
}
int main(void) {
    testInputs();
#ifndef TEST_FUNCTION_DECODER
    testTrain();
#endif
    testDcc();
#ifdef TEST_FUNCTION_DECODER
    puts("PASS: function decoder, 20 kHz duty sweep, nearby frequencies, DC/off, 256-clock trigger, wraps, polarity, DCC packet");
#else
    puts("PASS: motor decoder, 20 kHz duty sweep, nearby frequencies, DC/off, 512-clock trigger, wraps, polarity, CV58/CV59, DCC packet");
#endif
}
