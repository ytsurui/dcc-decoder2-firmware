/*
 * train_ctrl_func.c
 *
 * Created: 2021/05/31 23:09:25
 *  Author: ytsurui
 */ 

//#ifdef ATTINY806_FUNC

/*
 * train_ctrl.c
 *
 * Created: 2017/02/04 19:44:18
 * Project: motordecoder_simple_v1
 * Version: 1.00
 *	Target:	ATtiny45
 *	Author: Y.Tsurui
 */ 

#include <avr/io.h>

#include "cv_value.h"
#include "motor.h"
#include "ABC_detector.h"
#include "func_ctrl.h"

uint8_t target_spd;
uint8_t now_spd;

uint8_t clock_recv_counter = 0;
uint8_t Rate_counter = 0;

uint8_t pwm_cutout_timer = 0;

uint8_t kickstart_timer = 0;
uint8_t kickstart_timer_prescaler = 0;

uint8_t spdAnalogFlag = 0;

uint8_t nowDirection = 0xFF;
/*
 *	Direction
 *		Value = 1: Forward
 *		Value = 2: Backward
 */

uint8_t ABCworkedFlag = 0;

uint8_t ABCdirectionReverseFlag = 0;
uint16_t ABCautoReverseCount1 = 0;
uint8_t ABCautoReverseCount2 = 0;

uint8_t spdCache2;

uint8_t motorStartDelayCount = 0;	// Motor Start Delay Counter (CV140)
uint8_t motorStartDelaySpd = 0;		// Motor Start Delay New SPD

void setspeed_28step(uint8_t direction, uint8_t speed)
{
	uint8_t tableindex;
	
	if (direction != nowDirection) {
		nowDirection = direction;
	}
	
	tableindex = (speed & 0x0F) << 1;
	if (tableindex == 0x00) {
		now_spd = 0;
	} else if (tableindex == 0x02) {
		// Emergency Stop
		now_spd = 0;
	} else {
		now_spd = 1;
	}

}

void setspeed_128step(uint8_t direction, uint8_t speed)
{
	if (direction != nowDirection) {
		nowDirection = direction;
	}
	
	if (speed == 1) {
		now_spd = 0;
	} else {
		now_spd = speed << 1;
	}
}


uint8_t get_speed_8bit(void)
{
	return (now_spd);
}




uint8_t checkABCreverseDirection(void) {
	if (ABCdirectionReverseFlag & 0x01) {
		return (1);
	}
	return (0);
}


//#endif