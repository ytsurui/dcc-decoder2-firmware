/*
 * dcc_prog_mode.h
 *
 * Created: 2021/02/07 12:22:43
 *  Author: Y.Tsurui
 */ 

void dccProgPageMode(uint8_t packetLength, uint8_t packetData[]);
void dccProgDirectMode(uint8_t packetLength, uint8_t packetData[]);
void dccProgModeClockReceiver();