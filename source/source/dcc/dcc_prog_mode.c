/*
 * dcc_prog_mode.c
 *
 * Created: 2021/02/07 12:22:27
 *  Author: Y.Tsurui
 */ 

#include <avr/io.h>

#include "cv.h"
#include "../peripheral/motor.h"
#include "../peripheral/funcport.h"

uint8_t packetProgCache[8];
uint8_t basicACKflag = 0;
uint8_t basicACKcounter = 0;
#define BASIC_ACK_COUNT_MAX	12


// Paged Mode Programming Variables
uint8_t page_r = 0;
uint8_t page_verify_r, page_verify_r_flag = 0;
uint8_t service_page_reg_flag;

void basicACK() {
	basicACKcounter = 1;
	basicACKflag = 1;
	
	funcPortProgACK(FUNC_PROG_ACK_ON);
	pwmProgMode(PWM_PROG_MODE_ON);
}

void dccProgPageMode(uint8_t packetLength, uint8_t packetData[])
{
	uint16_t cvAddr;
	
	if ((packetData[0] == packetProgCache[0]) && (packetData[1] == packetProgCache[1]) && (packetData[2] == packetProgCache[2])) {
		
		if ((packetProgCache[0] & 0x04) == 0) {
			cvAddr = ((page_r - 1) * 4 + (packetProgCache[0] & 0x03) + 1);
			
			if (packetProgCache[0] & 0x08) {
				// Write CV
				write_cv_byte(cvAddr, packetProgCache[1]);
				service_page_reg_flag |= 0x04;
				basicACK();
			} else {
				// Veryfy CV
				if (packetProgCache[1] == read_cv(cvAddr)) {
					service_page_reg_flag |= 0x02;
					basicACK();
				}
			}
			return;
		}
		
		switch (packetProgCache[0] & 0x07) {
			case 0x05:
				// Paged Mode, Write Page Register
				if (~service_page_reg_flag & 0x01) {
					page_r = packetProgCache[1];
					service_page_reg_flag |= 0x01;
					basicACK();
				}
				return;
			case 0x04:
				// Phys Mode, CV29 (Basic Configuration)
				cvAddr = 29;
				break;
			case 0x06:
				// Phys Mode, CV7 (Version Number)
				cvAddr = 7;
				break;
			case 0x07:
				// Phys Mode, CV8 (Manufacturer ID Number)
				cvAddr = 8;
				break;
			default:
				return;
		}
		
		// Phys Mode
		if (packetProgCache[0] & 0x08) {
			// Write
			write_cv_byte(cvAddr, packetProgCache[1]);
			if (~service_page_reg_flag & 0x04) {
				service_page_reg_flag |= 0x04;
				basicACK();
			}
		} else {
			// Verify
			if (packetProgCache[1] == read_cv(cvAddr)) {
				if (~service_page_reg_flag & 0x02) {
					service_page_reg_flag |= 0x02;
					basicACK();
				}
			}
		}
		
	} else {
		packetProgCache[0] = packetData[0];
		packetProgCache[1] = packetData[1];
		packetProgCache[2] = packetData[2];
		basicACKflag = 0;
	}
}

void dccProgDirectMode(uint8_t packetLength, uint8_t packetData[])
{
	uint16_t cvAddr;
	
	if ((packetData[0] == packetProgCache[0]) && (packetData[1] == packetProgCache[1]) && (packetData[2] == packetProgCache[2]) && (packetData[3] == packetProgCache[3])) {
		if (basicACKflag) return;
		
		cvAddr = (packetProgCache[0] & 0x03) + packetProgCache[1] + 1;
		
		switch (packetProgCache[0] & 0x0C) {
			case 0x08:
				// Bit Manipulation
				break;
			case 0x04:
				// Verify Byte
				if (packetProgCache[2] == read_cv(cvAddr)) {
					basicACK();
				}
				break;
			case 0x0C:
				// Write Byte
				write_cv_byte(cvAddr, packetProgCache[2]);
				basicACK();
				break;
			case 0x00:
				// Reserved
				break;
		}
	} else {
		packetProgCache[0] = packetData[0];
		packetProgCache[1] = packetData[1];
		packetProgCache[2] = packetData[2];
		packetProgCache[3] = packetData[3];
		basicACKflag = 0;
	}
	
}


void dccProgModeClockReceiver()
{
	if (basicACKcounter != 0) {
		basicACKcounter++;
		if (basicACKcounter > BASIC_ACK_COUNT_MAX) {
			basicACKcounter = 0;
			basicACKflag = 1;
			funcPortProgACK(FUNC_PROG_ACK_OFF);
			pwmProgMode(PWM_PROG_MODE_OFF);
		}
	}
}