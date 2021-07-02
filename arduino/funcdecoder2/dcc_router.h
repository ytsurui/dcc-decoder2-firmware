/*
 * dcc_router.h
 *
 * Created: 2021/01/25 19:50:01
 *  Author: Y.Tsurui
 */ 


void dccPacketRouter(uint8_t packetLength, uint8_t packetData[]);
void dccRouterClockReceiver();
uint8_t readDirectionFlag(void);
uint8_t checkServiceMode(void);