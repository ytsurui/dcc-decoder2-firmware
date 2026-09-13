/*
 * analog_poller.h
 *
 * Created: 2021/02/14 16:21:25
 *  Author: Y.Tsurui
 */ 

void analogPollerClkReceiver(void);
uint8_t analogDirection(void);

void initAnalogPoller(void);
/* ISR snapshot shared with the DCC receiver. */
void analogPollerEdgeReceiver(uint8_t pins, uint16_t count);
/* Last complete measurement window, 0..255; CV scaling belongs to train_ctrl. */
uint8_t analogDuty(void);

/* This Arduino sketch delivers RTC PIT ticks at 512 Hz. */
#define ANALOG_WINDOW_CLOCKS 256U
