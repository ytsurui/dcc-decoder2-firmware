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

#include "cv_value.h"
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

uint8_t getSpdCache2(void) {
	return (spdCache2);
}

//void setspeed(uint8_t direction, uint8_t speed, uint8_t *acceleRate, uint8_t *deacceleRate)
void setspeed(uint8_t direction, uint8_t speed)
{
	//static uint8_t old_direction = 0xFF;
	spdCache2 = speed;
	
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

	if (direction != nowDirection) {
		pwmSetDirection(direction);
		nowDirection = direction;
		pwm_cutout_timer = 255;
	}
	
	if (speed == 1) {
		//Emergency Stop
		now_spd = 0;
		target_spd = 0;
		pwmSetSpeed(0);
		return;
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
	
	setspeed(direction, spd2);
}

void setspeed_128step(uint8_t direction, uint8_t speed)
{
	uint8_t spd_table_tmp;
	uint8_t scaled_spd;
	
	spdAnalogFlag = 0;
	
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
	
	setspeed(direction, scaled_spd);
}

void setspeed_analog(uint8_t direction)
{
	spdAnalogFlag = 1;
	if (readDirectionReverse()) {
		if (direction == 2) {
			direction = 1;
		} else {
			direction = 2;
		}
	}
	setspeed(direction, CV58);
}


void clock_receiver_train_ctrl(void)
{
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
