/*
 * train_ctrl.h
 *
 * Created: 2017/02/04 19:44:26
 * Project: motordecoder_simple_v1
 * Version: 1.00
 *	Target:	ATtiny45
 *	Author: Y.Tsurui
 */ 


void setspeed_28step(uint8_t direction, uint8_t speed);
void setspeed_128step(uint8_t direction, uint8_t speed);
void setspeed_analog(uint8_t direction);

void clock_receiver_train_ctrl(void);
void clockReceiverABCctrl(void);

uint8_t checkABCreverseDirection(void);
uint8_t get_speed_8bit(void);
