/*
 * pwm_tiny1606.c
 *
 * Created: 2021/02/06 17:31:19
 *  Author: Y.Tsurui
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "motor.h"
#include "../dcc/cv_value.h"


uint8_t pwmProgModeFlag = 0;

#ifndef ATTINY806_FUNC

uint8_t pwmfreqChangeFlag = 0;

uint8_t superslowBaseCounter = 0;
uint8_t superslowCounter = 0;
uint8_t superslowValue = 0;

uint8_t throttleSPDvalue = 0;
uint8_t bemfSPDvalue = 0;
uint8_t nowSPDvalue = 0;
uint8_t fixedSPDvalue = 0;

uint8_t bemfReadFlag = 0;
uint8_t bemfSaveSpdValue = 0;
uint16_t bemfAdcValue = 0;

//uint8_t currentReadFlag = 0;
uint16_t currentAdcValue = 0;

void pwmChangeFrequency(uint8_t freqCfg);
uint8_t calcSuperSlowDutyValue(void);

void calcMotorPID(void);


ISR(TCA0_OVF_vect)
{
	TCA0.SINGLE.INTFLAGS |= TCA_SINGLE_OVF_bm;	// Clear Interrupt Flag
		
	if (pwmfreqChangeFlag) {
		if (pwmfreqChangeFlag == PWM_FREQ_39KHZ) {
			// Set Freq Default
			pwmChangeFrequency(PWM_FREQ_39KHZ);
			pwmfreqChangeFlag = 0;
			TCA0.SINGLE.INTCTRL &= ~TCA_SINGLE_OVF_bm;
		} else {
			// Set Frequency Low (38Hz / 152Hz)
			pwmChangeFrequency(pwmfreqChangeFlag);
			pwmfreqChangeFlag = 1;
		}
	}
}

#endif

void initMotorModule(void)
{
	// PORT Configuration
	PORTA.DIRCLR = PIN1_bm;	// BEMF (AIN1)
	PORTA.DIRSET = PIN6_bm;	// FWD
	PORTB.DIRSET = PIN0_bm;	// PWM
	PORTC.DIRSET = PIN3_bm;	// REV
	
	PORTA.OUTCLR = PIN6_bm;
	PORTC.OUTCLR = PIN3_bm;
#ifndef ATTINY806_FUNC
	// Timer0 Configuration
	TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1_gc | TCA_SINGLE_ENABLE_bm;
	TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_SINGLESLOPE_gc | TCA_SINGLE_CMP0_bm;
	
	TCA0.SINGLE.PER = 0xFF;
	//TCA0.SINGLE.CMP0 = 0x2F;
	TCA0.SINGLE.CMP0 = 0x00;
	
	PORTA.OUTSET |= PIN6_bm;
	
#ifdef AVR2
	ADC0.CTRLA = ADC_ENABLE_bm;
	ADC0.CTRLB = ADC_PRESC_DIV32_gc;
	ADC0.CTRLC = ADC_REFSEL_4096MV_gc;
	ADC0.CTRLD = 0;
	
	ADC0.CTRLF = ADC_SAMPNUM_ACC2_gc;
	//ADC0.CTRLF = ADC_SAMPNUM_NONE_gc;
	ADC0.COMMAND = ADC_MODE_SINGLE_8BIT_gc;
#else
	// VREF Configuration
	VREF.CTRLA = VREF_ADC0REFSEL_4V34_gc;
	
	// ADC Configuration
	ADC0.CTRLA = ADC_ENABLE_bm | ADC_RESSEL_8BIT_gc;
	ADC0.CTRLB = ADC_SAMPNUM_ACC2_gc;
	//ADC0.CTRLC = ADC_PRESC_DIV16_gc;	// Internal Reference Voltage, CLK div 16
	//ADC0.CTRLC = 0x40 | ADC_REFSEL_VDDREF_gc | ADC_PRESC_DIV32_gc;
	ADC0.CTRLC = 0x40 | ADC_REFSEL_INTREF_gc | ADC_PRESC_DIV32_gc;
	ADC0.CTRLD = ADC_INITDLY_DLY16_gc | ADC_ASDV_ASVOFF_gc;
	ADC0.CTRLE = 0;
#endif

#endif
}

#ifndef ATTINY806_FUNC

void pwmSetDirection(uint8_t dir)
{
	if (pwmProgModeFlag) return;
	
	if (CV33_43[10] == 1) return;
	
	if (dir == PWM_DIRECTION_FOR) {
		// PWD
		PORTC.OUTCLR = PIN3_bm;
		PORTA.OUTSET = PIN6_bm;
	} else if (dir == PWM_DIRECTION_REV) {
		// REV
		PORTA.OUTCLR = PIN6_bm;
		PORTC.OUTSET = PIN3_bm;
	} else {
		PORTA.OUTCLR = PIN6_bm;
		PORTC.OUTCLR = PIN3_bm;
	}
	
}

void pwmSetSpeed(uint8_t spd)
{
	uint16_t calcTemp;
	//TCA0.SINGLE.CMP0 = spd;
	
	if (CV33_43[10] == 1) {
		nowSPDvalue = spd;
		return;
	}
		
	if (spd != nowSPDvalue) {
		throttleSPDvalue = spd;
		nowSPDvalue = spd;
		superslowValue = calcSuperSlowDutyValue();
		calcTemp = 0xFF * nowSPDvalue;
		
		fixedSPDvalue = (uint8_t)(calcTemp / superslowValue) & 0xFF;
	}
}

#endif

void pwmProgMode(uint8_t stat)
{
	if (stat == PWM_PROG_MODE_ON) {
		// Disable PWM Output
		#ifndef ATTINY806_FUNC
		TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_SINGLESLOPE_gc;
		#endif
		// Forward Output
		PORTC.OUTCLR = PIN3_bm;
		PORTA.OUTSET = PIN6_bm;
		PORTB.OUTSET = PIN0_bm;
		
		pwmProgModeFlag = 1;
	} else {
		// Output Stop
		PORTC.OUTCLR = PIN3_bm;
		PORTA.OUTCLR = PIN6_bm;
		PORTB.OUTCLR = PIN0_bm;
		
		// Enable PWM Output
		#ifndef ATTINY806_FUNC
		TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_SINGLESLOPE_gc | TCA_SINGLE_CMP0_bm;
		#endif
		pwmProgModeFlag = 0;
	}
}

#ifndef ATTINY806_FUNC

void pwmChangeFrequency(uint8_t freqCfg)
{
	if (CV33_43[10] == 1) return;
	
	if (freqCfg == PWM_FREQ_39KHZ) {
		TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1_gc | TCA_SINGLE_ENABLE_bm;
	} else if (freqCfg == PWM_FREQ_19KHZ) {
		TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV2_gc | TCA_SINGLE_ENABLE_bm;
	} else if (freqCfg == PWM_FREQ_152HZ) {
		TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV256_gc | TCA_SINGLE_ENABLE_bm;
	} else if (freqCfg == PWM_FREQ_38HZ) {
		TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1024_gc | TCA_SINGLE_ENABLE_bm;
	}
}

void pwmChangeFreqOperation(uint8_t freqCfg)
{
	if (pwmProgModeFlag) return;
	if (CV33_43[10] == 1) return;
	
	pwmfreqChangeFlag = freqCfg;
	TCA0.SINGLE.INTCTRL |= TCA_SINGLE_OVF_bm;
}

uint8_t calcSuperSlowDutyValue(void)
{
	uint16_t calcTemp;
	
	//nowSpdValue = TCA0.SINGLE.CMP0 & 0x00FF;
	if (nowSPDvalue >= CV60_64[2]) return (0xFF);
	
	//dutyBaseValue = CV62 - CV61;
	//calcTemp = nowSPDvalue * dutyBaseValue;
	//calcTemp = calcTemp / CV62;
	//calcTemp += CV61;
	
	calcTemp = (nowSPDvalue * (0xFF - CV60_64[1]) / CV60_64[2]) + CV60_64[1];
	
	
	return (calcTemp & 0x00FF);
}

// 16kHz Signal Receiver
void HSclockReceiverMotorCtrl(void)
{
	if (CV33_43[10] == 1) return;
	
	if (pwmProgModeFlag) return;
	
	if (bemfReadFlag) {
		if (bemfReadFlag < 100) {
			if (bemfReadFlag == 1) {
				// Disable PWM Output
				bemfSaveSpdValue = TCA0.SINGLE.CMP0;
				TCA0.SINGLE.CMP0 = 0;
			} else if (bemfReadFlag == 10) {
				// Enable ADC
#ifdef AVR2
				ADC0.MUXPOS = ADC_MUXPOS_AIN1_gc;
				ADC0.COMMAND |= ADC_START_IMMEDIATE_gc;
#else
				ADC0.MUXPOS = ADC_MUXPOS_AIN1_gc;
				ADC0.COMMAND = ADC_STCONV_bm;
#endif
				bemfReadFlag = 100;
			}
			bemfReadFlag++;
		/*
		} else if (bemfReadFlag == 49) {
			// Enable ADC
			ADC0.MUXPOS = ADC_MUXPOS_AIN1_gc;
			ADC0.COMMAND = ADC_STCONV_bm;
			bemfReadFlag = 51;
		*/
		} else if (bemfReadFlag == 101) {
#ifdef AVR2
			if ((ADC0_COMMAND & 0x07) == 0) {
				// ADC Completed
				bemfAdcValue = ADC0.SAMPLE;
				bemfReadFlag = 0;
				TCA0.SINGLE.CMP0 = bemfSaveSpdValue;
				//calcBEMFoffsetValue();
				calcMotorPID();
			}
#else
			if ((ADC0.COMMAND & ADC_STCONV_bm) == 0) {
				// ADC Completed
				bemfAdcValue = ADC0.RES;
				bemfReadFlag = 0;
				TCA0.SINGLE.CMP0 = bemfSaveSpdValue;
				//calcBEMFoffsetValue();
				calcMotorPID();
			}
#endif
		}
		
		
		return;
	}
	
	/*
	if (currentReadFlag) {
		if (currentReadFlag == 1) {
			// Enable ADC
			ADC0.MUXPOS = ADC_MUXPOS_AIN1_gc;
			ADC0.COMMAND = ADC_STCONV_bm;
			currentReadFlag = 2;
		} else if (currentReadFlag == 2) {
			if ((ADC0.COMMAND & ADC_STCONV_bm) == 0) {
				// ADC Completed
				currentAdcValue = ADC0.RES;
				currentReadFlag = 0;
			}
		} else {
			currentReadFlag = 0;
		}
	}
	*/
	
	if ((CV60_64[0] & 0x7F) == 0) {
		// Enable PWM Output
		//TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_SINGLESLOPE_gc | TCA_SINGLE_CMP0_bm;
		if (CV60_64[0] & 0x80) {
			TCA0.SINGLE.CMP0 = bemfSPDvalue;
		} else {
			TCA0.SINGLE.CMP0 = nowSPDvalue;
		}
		return;
	}
	
	if ((CV60_64[0] & 0x7F) == 3) {
		superslowBaseCounter++;
		if (superslowBaseCounter >= 1) {
			superslowBaseCounter = 0;
			superslowCounter++;
		} else {
			return;
		}
	} else if (CV60_64[0] & 0x02) {
		// 120Hz
		superslowCounter += 2;
	} else {
		// 60Hz
		superslowCounter++;
	}
	if (superslowValue < superslowCounter) {
		// Disable PWM Output
		//TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_SINGLESLOPE_gc;
		TCA0.SINGLE.CMP0 = 0;
		TCA0.SINGLE.CNT = 0xFE;
		//PORTB.OUTCLR = PIN0_bm;
	} else {
		// Enable PWM Output
		if (CV60_64[0] & 0x80) {
			TCA0.SINGLE.CMP0 = bemfSPDvalue;	
		} else {
			TCA0.SINGLE.CMP0 = fixedSPDvalue;
		}
		//TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_SINGLESLOPE_gc | TCA_SINGLE_CMP0_bm;
	}
	
}

uint8_t get_speed_8bit(void)
{
	//return (TCA0.SINGLE.CMP0 & 0x00FF);
	return (nowSPDvalue);
}

void captureCurrent(void)
{
	//if (bemfReadFlag != 0) return;
	//if (currentReadFlag == 0) currentReadFlag = 1;
}

uint16_t getCurrentValue(void) {
	return (currentAdcValue);
}

void captureBEMF(void)
{
	if (~CV60_64[0] & 0x80) return;
	//if (currentReadFlag != 0) return;
	if (bemfReadFlag == 0) bemfReadFlag = 1;
}

uint16_t getBEMFvalue(void)
{
	return (bemfAdcValue);
}

void calcMotorPID(void) {
	
	int16_t bemfADCfixedValue;
	int16_t P, D, dt;
	static int16_t I;
	static int16_t preP;
	
	int16_t Kp, Ki, Kd;
	int16_t calcValue;
	
	if (throttleSPDvalue == 0) {
		bemfSPDvalue = 0;
		return;
	}
	
	dt = 10;
	
	Kp = CV55_57[0];	// CV55
	Ki = CV55_57[1];	// CV56
	Kd = CV55_57[2];	// CV57

#ifdef AVR2
	bemfADCfixedValue = bemfAdcValue * 255 / CV138;	// CV138 = 175: 2.96V ADC MAX to Scale 0-255
#else
	bemfADCfixedValue = (bemfAdcValue >> 1) * 255 / CV138;	// CV138 = 175: 2.96V ADC MAX to Scale 0-255
#endif
	if (bemfADCfixedValue > 255) bemfADCfixedValue = 255;
	
	P = (int16_t)throttleSPDvalue - bemfADCfixedValue;
	I += P / dt;
	D = (P - preP) / (dt * 10);
	preP = P;
	
	calcValue = (int16_t)throttleSPDvalue + ((Kp * P / 10) + (Ki * I / 10) + ((Kd * D) / 10));
	//calcValue = (Kp * P / 10) + (Ki * I / 10) + ((Kd * D) / 10);
	//bemfSPDvalue = (uint8_t)((Kp * P) + (Ki * I));
	if (calcValue > 255) {
		bemfSPDvalue = 255;
	} else if (calcValue < 0) {
		bemfSPDvalue = 0;
	} else {
		bemfSPDvalue = (uint8_t)calcValue;
	}
	
	/*
	if (bemfSPDvalue > throttleSPDvalue) {
		if ((bemfSPDvalue - throttleSPDvalue) > 160) {
			bemfSPDvalue = throttleSPDvalue + 160;
		}
	} else {
		if ((throttleSPDvalue - bemfSPDvalue) > 160) {
			bemfSPDvalue = throttleSPDvalue - 160;
		} 
	}
	*/
	
}

uint16_t getBEMFFixedSpdValue(void) {
	return (bemfSPDvalue);
}

#endif


void motorFuncDriver(uint8_t stat, uint8_t direction) {
	if (CV33_43[10] != 1) return;
	
	// Disable PWM Output
	#ifndef ATTINY806_FUNC
	TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_SINGLESLOPE_gc;
	#endif
		
	
	if (direction == PWM_DIRECTION_FOR) {
		// PWD
		PORTC.OUTCLR = PIN3_bm;
		PORTA.OUTSET = PIN6_bm;
	} else if (direction == PWM_DIRECTION_REV) {
		// REV
		PORTA.OUTCLR = PIN6_bm;
		PORTC.OUTSET = PIN3_bm;
	} else {
		PORTA.OUTCLR = PIN6_bm;
		PORTC.OUTCLR = PIN3_bm;
	}
	
	if (stat) {
		PORTB.OUTSET = PIN0_bm;
	} else {
		PORTB.OUTCLR = PIN0_bm;
	}
}
