/* Analog rail PWM measurement. TCB0 remains the DCC time base (5 MHz). */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "analog_poller.h"

#define ANALOG_TIMER_PERIOD 0xD000UL

static volatile uint32_t timerBase;
static volatile uint16_t lastCount;
static volatile uint32_t rightTime;
static volatile uint32_t leftTime;
static volatile uint8_t lastPins;
static uint32_t windowStart;
static uint16_t windowClocks;
static uint8_t direction;
static uint8_t duty;

/* Called only with interrupts disabled. Keep the frequent edge path 16-bit;
 * the 32-bit totals are extended at every timer wrap, even with no edges.
 */
static inline __attribute__((always_inline)) void accumulateTo(uint16_t count)
{
    uint16_t elapsed = count - lastCount;
    if (lastPins == PIN2_bm) rightTime += elapsed;
    else if (lastPins == PIN4_bm) leftTime += elapsed;
    lastCount = count;
}

static void finishTimerPeriod(void)
{
    TCB0.INTFLAGS = TCB_CAPT_bm;
    accumulateTo((uint16_t)ANALOG_TIMER_PERIOD);
    lastCount = 0;
    timerBase += ANALOG_TIMER_PERIOD;
}

ISR(TCB0_INT_vect)
{
    /* PORTA can service a pending wrap first. Do not account for it twice. */
    if (TCB0.INTFLAGS & TCB_CAPT_bm) finishTimerPeriod();
}

static inline __attribute__((always_inline)) void captureTime(uint16_t count)
{
    /* Also covers a wrap between reading CNT and reading INTFLAGS.
     * Interrupt masking must stay below one period (10.65 ms).
     */
    if (TCB0.INTFLAGS & TCB_CAPT_bm) {
        finishTimerPeriod();
        count = TCB0.CNT;
    }
    accumulateTo(count);
}

/* Called after initDCCpoller has configured TCB0, before input IRQs resume. */
void initAnalogPoller(void)
{
    timerBase = 0;
    TCB0.INTFLAGS = TCB_CAPT_bm;
    lastCount = TCB0.CNT;
    windowStart = lastCount;
    lastPins = PORTA.IN & (PIN2_bm | PIN4_bm);
    rightTime = leftTime = 0;
    windowClocks = 0;
    direction = duty = 0;
    TCB0.INTCTRL |= TCB_CAPT_bm;
}

/* PORTA ISR calls this before DCC decoding. No division in interrupt context. */
void analogPollerEdgeReceiver(uint8_t pins, uint16_t count)
{
    captureTime(count);
    lastPins = pins & (PIN2_bm | PIN4_bm);
}

void analogPollerClkReceiver(void)
{
    uint32_t right, left, elapsed;
    if (++windowClocks < ANALOG_WINDOW_CLOCKS) return;
    windowClocks = 0;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        /* Finish the open interval, including constant DC and 0% PWM. */
        captureTime(TCB0.CNT);
        uint32_t now = timerBase + lastCount;
        right = rightTime;
        left = leftTime;
        elapsed = now - windowStart;
        rightTime = leftTime = 0;
        windowStart = now;
    }

    duty = 0;
    /* Reject mixed polarity (DCC or a direction change within this window).
     * Keep the last valid direction when stopped or when the window is invalid.
     * Both-low and both-high time remains in the denominator.
     */
    if (elapsed == 0 || (right != 0 && left != 0)) return;
    if (right != 0) {
        direction = 1;
    } else if (left != 0) {
        direction = 2;
    } else {
        return;
    }
    /* Bound multiplication even if main-loop clock delivery was delayed. */
    uint32_t on = right + left;
    while (elapsed > 0xFFFFFFUL) {
        elapsed >>= 1;
        on >>= 1;
    }
    duty = (uint8_t)((on * 255UL + elapsed / 2) / elapsed);
}

uint8_t analogDirection(void) { return direction; }
uint8_t analogDuty(void) { return duty; }
