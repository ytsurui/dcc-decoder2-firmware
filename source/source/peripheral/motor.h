/*
 * motor.h
 *
 * Created: 2021/02/06 17:31:46
 *  Author: Y.Tsurui
 */ 

void initMotorModule(void);

void pwmSetDirection(uint8_t dir);
void pwmSetSpeed(uint8_t spd);

#define PWM_DIRECTION_FOR	1
#define PWM_DIRECTION_REV	2

void pwmProgMode(uint8_t stat);
#define PWM_PROG_MODE_ON	1
#define PWM_PROG_MODE_OFF	2

//void pwmChangeFrequency(uint8_t freqCfg);
void pwmChangeFreqOperation(uint8_t freqCfg);
#define PWM_FREQ_39KHZ				1
#define PWM_FREQ_19KHZ				2
#define PWM_FREQ_38HZ				11
#define PWM_FREQ_152HZ				12

void HSclockReceiverMotorCtrl(void);

uint8_t get_speed_8bit(void);

void captureCurrent(void);
uint16_t getCurrentValue(void);
void captureBEMF(void);
uint16_t getBEMFvalue(void);
uint16_t getBEMFFixedSpdValue(void);

void motorFuncDriver(uint8_t stat, uint8_t direction);
