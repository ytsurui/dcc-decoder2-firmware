/*
 * train_ctrl.c
 *
 * Created: 2017/02/04 19:44:18
 * Project: motordecoder_simple_v1
 * Version: 1.00
 *	Target:	ATtiny45
 *	Author: Y.Tsurui
 */ 

#ifndef ATTINY806_FUNC

#include <avr/io.h>
#include "analog_poller.h"

#include "cv_value.h"
//#include "pwm_motor_ctrl.h"
#include "motor.h"
#include "ABC_detector.h"
#include "func_ctrl.h"
#include "funcport.h"

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

/* DCC operator reversal. ABC automatic reversal and analog keep their own paths. */
#define REV_IDLE 0
#define REV_DECEL 1
#define REV_WAIT 2
#define REV_ACCEL 3
#define REV_MIN_RATE 1
#define REV_STOP_TICKS 255 /* approximately 250 ms at 1024 Hz */
static uint8_t reversalState;
static uint8_t requestedDirection;
static uint8_t reversalABC;
static uint8_t reversalWait;

uint8_t readMotorDirection(void) { return nowDirection; }

static void resetRampClock(void)
{
	clock_recv_counter = Rate_counter = 0;
}

static void beginReversal(void)
{
	reversalState = REV_DECEL;
	reversalABC = (ABCworkedFlag & 1) || getABCstatus() == nowDirection;
	pwm_cutout_timer = motorStartDelayCount = motorStartDelaySpd = 0;
	resetRampClock();
}

/* Returning to the bridge's current direction cancels only the operator reversal. */
static void cancelReversal(void)
{
	if (reversalABC) ABCworkedFlag |= 1;
	reversalState = REV_IDLE;
	reversalABC = reversalWait = 0;
	resetRampClock();
}

/* Manual reduction is an upper bound, never an acceleration during braking. */
static void limitReversalSpeed(uint8_t speed)
{
	if (reversalState != REV_DECEL || speed >= now_spd) return;
	now_spd = speed;
	pwmSetSpeed(now_spd);
	if (now_spd == 0) {
		reversalState = REV_WAIT;
		reversalWait = REV_STOP_TICKS;
		resetRampClock();
	}
}

static void emergencyStop(void)
{
	reversalState = REV_IDLE;
	reversalABC = reversalWait = 0;
	motorStartDelayCount = motorStartDelaySpd = pwm_cutout_timer = 0;
	now_spd = target_spd = 0;
	resetRampClock();
	pwmSetSpeed(0);
}

/* Return true while this state machine owns speed and direction. */
static uint8_t clockReversal(void)
{
	uint8_t rate;
	if (reversalState == REV_IDLE) return 0;
	if (reversalState == REV_WAIT) {
		pwmSetSpeed(0);
		if (--reversalWait) return 1;
		pwmSetDirection(requestedDirection);
		nowDirection = requestedDirection;
		resetRampClock();
		if (reversalABC) {
			/* Let the existing ABC recovery select CV53 (or CV3 fallback). */
			ABCworkedFlag |= 1;
			reversalState = REV_IDLE;
		} else {
			reversalState = REV_ACCEL;
		}
		return 1;
	}
	if (reversalState == REV_DECEL) {
		if (getABCstatus() == nowDirection) reversalABC = 1;
		rate = CV1_6[3];
	} else {
		/* A newly entered ABC section must still be able to stop the train. */
		if (getABCstatus() == nowDirection) {
			reversalState = REV_IDLE;
			return 0;
		}
		rate = now_spd > target_spd ? CV1_6[3] : CV1_6[2];
	}
	if (rate < REV_MIN_RATE) rate = REV_MIN_RATE;
	if (++clock_recv_counter < 16) return 1;
	clock_recv_counter = 0;
	if (++Rate_counter < rate) return 1;
	Rate_counter = 0;
	if (reversalState == REV_DECEL) {
		if (now_spd) --now_spd;
		pwmSetSpeed(now_spd);
		if (now_spd == 0) {
			reversalState = REV_WAIT;
			reversalWait = REV_STOP_TICKS;
		}
	} else {
		if (now_spd < target_spd) ++now_spd;
		else if (now_spd > target_spd) --now_spd;
		pwmSetSpeed(now_spd);
		if (now_spd == target_spd) reversalState = REV_IDLE;
	}
	return 1;
}

uint8_t getSpdCache2(void) {
	return (spdCache2);
}

//void setspeed(uint8_t direction, uint8_t speed, uint8_t *acceleRate, uint8_t *deacceleRate)
static void setspeedCommand(uint8_t direction, uint8_t speed, uint8_t emergency)
{
	//static uint8_t old_direction = 0xFF;
	spdCache2 = speed;
	if (emergency) {
		emergencyStop();
		return;
	}
	
	//if (getYardModeStat()) speed = speed >> 1;	
	if (getYardModeStat()) {
		speed = (uint8_t)(speed * CV131 / 256) & 0x00FF;
	}
	
	if (readDirectionReverse()) {
		if (direction == 2) {
			direction = 1;
		} else {
			direction = 2;
		}
	}

	if (!spdAnalogFlag && CV33_43[10] != 1) {
		requestedDirection = direction;
		if (reversalState != REV_IDLE) {
			if ((reversalState == REV_DECEL || reversalState == REV_WAIT) &&
			    direction == nowDirection) {
				cancelReversal();
				/* Continue below with the existing normal/ABC speed control. */
			} else {
				target_spd = speed;
				if (reversalState == REV_ACCEL && direction != nowDirection) beginReversal();
				limitReversalSpeed(speed);
				return;
			}
		}
		if (direction != nowDirection && now_spd != 0 &&
		    (nowDirection == 1 || nowDirection == 2)) {
			target_spd = speed;
			beginReversal();
			limitReversalSpeed(speed);
			return;
		}
	}
	if (direction != nowDirection) {
		pwmSetDirection(direction);
		nowDirection = direction;
		pwm_cutout_timer = 255;
	}

	if ((target_spd == 0) && (CV140 != 0)) {
		// Motor Start Delay Mode
		if (speed != 0) {
			if (motorStartDelayCount == 0) {
				motorStartDelayCount = CV140;
				clock_recv_counter = 0;
			}
			motorStartDelaySpd = speed;
			return;
		} else {
			motorStartDelaySpd = 0;
		}
	}

	target_spd = speed;


	if ((speed < CV1_6[1]) && (speed != 0)) {
		speed = CV1_6[1];
	}

	if ((now_spd < CV1_6[1]) && (speed != 0)) {
		now_spd = CV1_6[1];
	}

	if (pwm_cutout_timer == 0) {
		if (spdAnalogFlag) {
			
		} else {
			if (getABCstatus() != direction) {
				if ((ABCworkedFlag != 0) && (CV53 != 0)) {
					
				} else {
					if (ABCworkedFlag & 0x01) {
						ABCworkedFlag &= ~0x01;
					}
					if ((target_spd < now_spd) && (CV1_6[3] == 0)) {
						//if ((target_spd < now_spd) && (CV4 == 0)) {
						//if ((target_spd < now_spd) && (*deacceleRate == 0)) {
						//Deacceleration
						now_spd = speed;
						pwmSetSpeed(speed);
					} else if ((target_spd > now_spd) && (CV1_6[2] == 0)) {
						//} else if ((target_spd > now_spd) && (CV3 == 0)) {
						//} else if ((target_spd > now_spd) && (*acceleRate == 0)) {
						//Acceleration
						now_spd = speed;
						pwmSetSpeed(speed);
					} else if (target_spd == now_spd) {
						now_spd = speed;
						pwmSetSpeed(speed);
					}
				}
			} else {
				ABCworkedFlag |= 1;
				if ((CV54 == 0) && (CV1_6[3] == 0)) {
					// Automatic Brake
					now_spd = 0;
					pwmSetSpeed(0);
				}
			}
			
		}
	} else {
		//pwm_set_spd(0);
		pwmSetSpeed(0);
	}
}

void setspeed(uint8_t direction, uint8_t speed)
{
	setspeedCommand(direction, speed, !spdAnalogFlag && speed == 1);
}

void setspeed_28step(uint8_t direction, uint8_t speed)
{
	uint8_t tableindex;
	uint8_t spd2;
	
	//if ((CV37 & 0x80) == 0) return;
	
	spdAnalogFlag = 0;
	
	tableindex = (speed & 0x0F) << 1;
	
	if (tableindex == 0x00) {
		setspeed(direction, 0);
		return;
	}
	
	if (tableindex == 0x02) {
		//Emergency Stop
		setspeed(direction, 1);
		return;
	}
	
	if (speed & 0x10) {
		tableindex++;
	}
	
	spd2 = CV67_94[tableindex - 4];
	
	setspeedCommand(direction, spd2, 0);
}

void setspeed_128step(uint8_t direction, uint8_t speed)
{
	uint8_t spd_table_tmp;
	uint8_t scaled_spd;
	
	spdAnalogFlag = 0;
	if (speed <= 1) {
		setspeedCommand(direction, 0, speed == 1);
		return;
	}
	
	/*
	if ((CV2 < CV6) && (CV6 < CV5)) {
		if (speed < 64) {
			spd_table_tmp = CV6 - CV2;
			scaled_spd = CV2 + ((spd_table_tmp * (speed)) / 64);
		} else {
			spd_table_tmp = CV5 - CV6;
			scaled_spd = CV6 + ((spd_table_tmp * (speed - 64)) / 64);;
		}
	} else if (CV2 < CV5) {
		spd_table_tmp = CV5 - CV2;
		scaled_spd = CV2 + ((spd_table_tmp * speed) / 128);
	} else {
		scaled_spd = speed << 1;
	}
	*/
	if ((CV1_6[1] < CV1_6[5]) && (CV1_6[5] < CV1_6[4])) {
		if (speed < 64) {
			spd_table_tmp = CV1_6[5] - CV1_6[1];
			scaled_spd = CV1_6[1] + ((spd_table_tmp * (speed)) / 64);
		} else {
			spd_table_tmp = CV1_6[4] - CV1_6[5];
			scaled_spd = CV1_6[5] + ((spd_table_tmp * (speed - 64)) / 64);;
		}
	} else if (CV1_6[1] < CV1_6[4]) {
		spd_table_tmp = CV1_6[4] - CV1_6[1];
		scaled_spd = CV1_6[1] + ((spd_table_tmp * speed) / 128);
	} else {
		scaled_spd = speed << 1;
	}
	
	setspeedCommand(direction, scaled_spd, 0);
}

void setspeed_analog(uint8_t direction)
{
	/* Discard a pending DCC reversal when entering analog operation. */
	reversalState = REV_IDLE;
	spdAnalogFlag = 1;
	if (readDirectionReverse()) {
		if (direction == 2) {
			direction = 1;
		} else {
			direction = 2;
		}
	}
	/* CV58 is the full-duty ceiling; CV59 is applied by the clock receiver. */
	uint8_t speed = (uint8_t)(((uint16_t)CV58 * analogDuty() + 127U) / 255U);
	/* No valid polarity yet: keep the motor stopped without inventing one. */
	if (analogDirection() == 0) {
		target_spd = now_spd = 0;
		pwmSetSpeed(0);
		return;
	}
	setspeed(direction, speed);
}


void clock_receiver_train_ctrl(void)
{
	if (!spdAnalogFlag && CV33_43[10] != 1 && clockReversal()) return;
	if (pwm_cutout_timer) {
		pwm_cutout_timer--;

		if (pwm_cutout_timer == 0) {
			//if ((target_spd < now_spd) && (CV4 == 0)) {
			if ((target_spd < now_spd) && (CV1_6[3] == 0)) {
				//Deacceleration
				pwmSetSpeed(now_spd);
			//} else if ((target_spd > now_spd) && (CV3 == 0)) {
			} else if ((target_spd > now_spd) && (CV1_6[2] == 0)) {
				//Acceleration
				pwmSetSpeed(now_spd);
			} else if (target_spd == now_spd) {
				pwmSetSpeed(now_spd);
			}
		}

		return;	
	}

	if (motorStartDelayCount) {
		clock_recv_counter++;
		if (clock_recv_counter >= 100) {
			clock_recv_counter = 0;
			motorStartDelayCount--;

			if (motorStartDelayCount == 0) {
				target_spd = motorStartDelaySpd;
				now_spd = 0;
			}
		}
		return;
	}
	

	clock_recv_counter++;

	if (clock_recv_counter >= 16) {
		clock_recv_counter = 0;
		Rate_counter++;
		
		
		if (spdAnalogFlag) {
			if (now_spd < target_spd) {
				if (Rate_counter >= CV59) {
					now_spd++;
					pwmSetSpeed(now_spd);
					Rate_counter = 0;
				}
			} else if (now_spd > target_spd) {
				if (Rate_counter >= CV59) {
					now_spd--;
					pwmSetSpeed(now_spd);
					Rate_counter = 0;
				}
			} else {
				Rate_counter = 0;
			}
		} else {
		
			if (getABCstatus() != nowDirection) {
				// ABC Status Disable (ABC Acceleration / Normal Operation)
				if (ABCworkedFlag & 0x01) {
					// ABC worked Acceleration
					if (CV53 == 0) {
						if (CV1_6[2] == 0) {
							now_spd = target_spd;
							pwmSetSpeed(now_spd);
							Rate_counter = 0;
						} else if ((Rate_counter >= CV1_6[2]) && (now_spd < target_spd)) {
							now_spd++;
							pwmSetSpeed(now_spd);
							Rate_counter = 0;
						}
					} else {
						if ((Rate_counter >= CV53) && (now_spd < target_spd)) {
							now_spd++;
							pwmSetSpeed(now_spd);
							Rate_counter = 0;
						}
					}
					if (now_spd >= target_spd) {
						ABCworkedFlag &= ~0x01;
					}
				} else {
					// Normal Operation
					if (now_spd < target_spd) {
						//Acceleration
						//if (Rate_counter == CV3) {
						if (Rate_counter >= CV1_6[2]) {
							now_spd++;
							pwmSetSpeed(now_spd);
							Rate_counter = 0;
						}
					} else if (now_spd > target_spd) {
						//Deacceleration
						//if (Rate_counter == CV4) {
						if (Rate_counter >= CV1_6[3]) {
							now_spd--;
							pwmSetSpeed(now_spd);
							Rate_counter = 0;
						}
					} else {
						Rate_counter = 0;
					}
				
				}
			} else if (now_spd != 0) {
				// ABC Enable (Stopping)
				ABCworkedFlag |= 0x01;
				
				if (CV54 == 0) {
					if (CV1_6[3] == 0) {
						now_spd = 0;
						pwmSetSpeed(0);
						Rate_counter = 0;
					} else {
						if (Rate_counter >= CV1_6[3]) {
							now_spd--;
							pwmSetSpeed(now_spd);
							Rate_counter = 0;
						}
					}
				} else if (Rate_counter >= CV54) {
					
					now_spd--;
					
					if (target_spd < now_spd) {
						now_spd = target_spd;
					}
					
					pwmSetSpeed(now_spd);
					Rate_counter = 0;

				} 
			}
		}
		
	}

}


/*
uint8_t get_speed_8bit(void)
{
	return (now_spd);
}
*/



uint8_t checkABCreverseDirection(void) {
	if (ABCdirectionReverseFlag & 0x01) {
		return (1);
	}
	return (0);
}


void clockReceiverABCctrl(void) {
	/* External reversal owns the bridge until its stop/turn sequence finishes. */
	if (reversalState != REV_IDLE) return;
	if (CV52 == 0) return;
	if (getABCstatus() != nowDirection) {
		ABCautoReverseCount1 = 0;
		ABCautoReverseCount2 = 0;
		return;
	}
	
	ABCautoReverseCount1++;
	if (ABCautoReverseCount1 >= 1000) {
		ABCautoReverseCount1 = 0;
		ABCautoReverseCount2++;
		
		if (ABCautoReverseCount2 >= CV52) {
			if (ABCdirectionReverseFlag & 0x01) {
				ABCdirectionReverseFlag &= ~0x01;
			} else {
				ABCdirectionReverseFlag |= 0x01;
			}
			if (nowDirection == 1) nowDirection = 2;
			else if (nowDirection == 2) nowDirection = 1;
			pwmSetDirection(nowDirection);
			pwm_cutout_timer = 255;
			
			ABCautoReverseCount1 = 0;
			ABCautoReverseCount2 = 0;
		}
	}
}

#endif
