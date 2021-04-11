/*
 * cv.c
 *
 * Created: 2021/02/06 21:52:17
 *  Author: Y.Tsurui
 */ 

#include <avr/io.h>

#include "cv.h"
#include "cv_value.h"
#include "cv_eeprom.h"


uint8_t read_cv_raw(uint8_t CVnum)
{
	return (eeprom_read_byte(val + CVnum));
}

uint8_t read_cv(uint16_t CVnum)
{
	if (CVnum > CV_COUNT) return (0);
	
	return (read_cv_raw(CVnum & 0x00FF));
}

void write_eeprom(uint8_t addr, uint8_t data)
{
	eeprom_busy_wait();
	//eeprom_write_byte(val + addr, data);
	eeprom_update_byte(val + addr, data);
}

void initialize_speed_table(void)
{
	write_eeprom(67, 9);
	write_eeprom(68, 18);
	write_eeprom(69, 27);
	write_eeprom(70, 36);
	write_eeprom(71, 46);
	write_eeprom(72, 55);
	write_eeprom(73, 64);
	write_eeprom(74, 73);
	write_eeprom(75, 82);
	write_eeprom(76, 91);
	write_eeprom(77, 100);
	write_eeprom(78, 109);
	write_eeprom(79, 118);
	write_eeprom(80, 128);
	write_eeprom(81, 137);
	write_eeprom(82, 146);
	write_eeprom(83, 155);
	write_eeprom(84, 164);
	write_eeprom(85, 173);
	write_eeprom(86, 182);
	write_eeprom(87, 191);
	write_eeprom(88, 200);
	write_eeprom(89, 209);
	write_eeprom(90, 219);
	write_eeprom(91, 228);
	write_eeprom(92, 237);
	write_eeprom(93, 246);
	write_eeprom(94, 255);
}

void initialize_cv(void)
{
	write_eeprom(1, CV1_DEFAULT);
	write_eeprom(2, CV2_DEFAULT);
	write_eeprom(3, CV3_DEFAULT);
	write_eeprom(4, CV4_DEFAULT);
	write_eeprom(5, CV5_DEFAULT);
	write_eeprom(6, CV6_DEFAULT);
	write_eeprom(7, HARDWARE_NUM);
	write_eeprom(8, MANUFACTURENUM);
	write_eeprom(17, CV17_DEFAULT);
	write_eeprom(18, CV18_DEFAULT);
	write_eeprom(19, 0);
	write_eeprom(27, CV27_DEFAULT);
	write_eeprom(29, CV29_DEFAULT);
	write_eeprom(33, CV33_DEFAULT);
	write_eeprom(34, CV34_DEFAULT);
	write_eeprom(35, CV35_DEFAULT);
	write_eeprom(36, CV36_DEFAULT);
	write_eeprom(37, CV37_DEFAULT);
	write_eeprom(38, CV38_DEFAULT);
	write_eeprom(39, CV39_DEFAULT);
	write_eeprom(40, CV40_DEFAULT);
	write_eeprom(41, CV41_DEFAULT);
	write_eeprom(42, CV42_DEFAULT);
	write_eeprom(43, CV43_DEFAULT);
	write_eeprom(44, CV44_DEFAULT);
	write_eeprom(45, CV45_DEFAULT);
	write_eeprom(47, CV47_DEFAULT);
	write_eeprom(50, CV50_DEFAULT);
	write_eeprom(51, CV51_DEFAULT);
	write_eeprom(52, CV52_DEFAULT);
	write_eeprom(53, CV53_DEFAULT);
	write_eeprom(54, CV54_DEFAULT);
	write_eeprom(55, CV55_DEFAULT);
	write_eeprom(56, CV56_DEFAULT);
	write_eeprom(57, CV57_DEFAULT);
	write_eeprom(58, CV58_DEFAULT);
	write_eeprom(59, CV59_DEFAULT);
	write_eeprom(60, CV60_DEFAULT);
	write_eeprom(61, CV61_DEFAULT);
	write_eeprom(62, CV62_DEFAULT);
	write_eeprom(63, CV63_DEFAULT);
	write_eeprom(64, CV64_DEFAULT);
	write_eeprom(112, CV112_DEFAULT);
	write_eeprom(114, CV114_DEFAULT);
	write_eeprom(115, CV115_DEFAULT);
	write_eeprom(116, CV116_DEFAULT);
	write_eeprom(117, CV117_DEFAULT);
	write_eeprom(118, CV118_DEFAULT);
	write_eeprom(119, CV119_DEFAULT);
	write_eeprom(120, CV120_DEFAULT);
	write_eeprom(121, CV121_DEFAULT);
	write_eeprom(122, CV122_DEFAULT);
	write_eeprom(131, CV131_DEFAULT);
	write_eeprom(138, CV138_DEFAULT);
	write_eeprom(244, CV244_DEFAULT);
	
	initialize_speed_table();
	
	write_eeprom(0, 0);
	
	load_cv();
}

void initFuncTable(void) {
	funcTable1 = 0;
	funcTable2 = 0;
	funcTable3 = 0;
	funcTable4 = 0;
	funcTable5 = 0;
}

void setFuncTable(uint8_t funcNum) {
	if (funcNum <= 4) {
		if (funcNum == 0) {
			funcTable1 |= 0x10;
		} else {
			funcTable1 |= (1 << (funcNum - 1));
		}
	} else if (funcNum <= 8) {
		funcTable2 |= (1 << (funcNum - 5));
	} else if (funcNum <= 12) {
		funcTable3 |= (1 << (funcNum - 9));
	} else if (funcNum <= 20) {
		funcTable4 |= (1 << (funcNum - 13));
	} else if (funcNum <= 28) {
		funcTable5 |= (1 << (funcNum - 20));
	}
}


void load_cv(void)
{
	uint8_t CV0, i;

	eeprom_busy_wait();
	
	CV0 = read_cv_raw(0);
	
	if (CV0 == 0xFF) {
		//Initialize
		initialize_cv();
		return;
	}

	//CV1 = read_cv_raw(1);
	
	for (i = 0; i < 6; i++) {
		CV1_6[i] = read_cv_raw(1 + i);
	}
	//CV2 = read_cv_raw(2);
	//CV3 = read_cv_raw(3);
	//CV4 = read_cv_raw(4);
	//CV5 = read_cv_raw(5);
	//CV6 = read_cv_raw(6);

	/*
	if ((CV1 == 0xFF) && (CV2 == 0xFF) && (CV3 == 0xFF) && (CV4 == 0xFF) && (CV5 == 0xFF) & (CV6 == 0xFF)) {
		//Initialize
		initialize_cv();
		
		CV1 = read_cv_raw(1);
		CV2 = read_cv_raw(2);
		CV3 = read_cv_raw(3);
		CV4 = read_cv_raw(4);
		CV5 = read_cv_raw(5);
		CV6 = read_cv_raw(6);
	}
	*/
	CV17 = read_cv_raw(17);
	CV18 = read_cv_raw(18);
	CV19 = read_cv_raw(19);
	CV27 = read_cv_raw(27);		// Decoder Automatic Stopping Configuration
	CV29 = read_cv_raw(29);		// Decoder Basic Configuration
	/*
	CV33 = read_cv_raw(33);
	CV34 = read_cv_raw(34);
	CV35 = read_cv_raw(35);
	CV36 = read_cv_raw(36);
	CV37 = read_cv_raw(37);
	*/
	/*
	CV112 = read_cv_raw(112);
	CV114 = read_cv_raw(114);
	CV115 = read_cv_raw(115);
	CV116 = read_cv_raw(116);
	*/
	
	for (i = 0; i < 28; i++) {
		CV67_94[i] = read_cv_raw(67 + i);
	}
	
	for (i = 0; i < 11; i++) {
		CV33_43[i] = read_cv_raw(33 + i);
		setFuncTable(CV33_43[i] & 0x1F);
		CV112_CV122[i] = read_cv_raw(112 + i);
	}
	CV44 = read_cv_raw(44);
	CV45 = read_cv_raw(45);
	setFuncTable(CV44 & 0x1F);
	setFuncTable(CV45 & 0x1F);
	
	CV131 = read_cv_raw(131);
	
	
	CV47 = read_cv_raw(47);
	
	CV50 = read_cv_raw(50);
	
	CV51 = read_cv_raw(51);		// Automatic Brake Ignore Function
	setFuncTable(CV51 & 0x1F);	
	CV52 = read_cv_raw(52);
	CV53 = read_cv_raw(53);
	CV54 = read_cv_raw(54);
	
	for (i = 0; i < 3; i++) {
		CV55_57[i] = read_cv_raw(55 + i);
	}
	CV138 = read_cv_raw(138);
	
	
	CV58 = read_cv_raw(58);
	CV59 = read_cv_raw(59);
	
	CV60 = read_cv_raw(60);
	CV61 = read_cv_raw(61);
	CV62 = read_cv_raw(62);
	CV63 = read_cv_raw(63);		// Slow Speed Mode for yard Operation
	setFuncTable(CV63 & 0x1F);
	CV64 = read_cv_raw(64);		// Force Railcom Enabled (CV29 bit3 overwrite)

	CV244 = read_cv_raw(244);
	
	/*
	setFuncTable(CV33 & 0x1F);
	setFuncTable(CV34 & 0x1F);
	setFuncTable(CV35 & 0x1F);
	setFuncTable(CV36 & 0x1F);
	setFuncTable(CV37 & 0x1F);
	*/
}


void write_cv_byte(uint16_t CVnum, uint8_t data)
{
	uint8_t CV15, CV16, CVnum2;
	
	if (CVnum > CV_COUNT) return;
	
	CVnum2 = CVnum & 0x00FF;
	
	if ((CVnum2 != 15) && (CVnum2 != 16)) {
		CV15 = read_cv_raw(15);
		CV16 = read_cv_raw(16);
		
		if (CV15 != CV16) return;
	}

	switch (CVnum2) {
		//case 1:
		//	CV1 = data;
		//	break;
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
			CV1_6[CVnum2 - 1] = data;
			break;
		case 7: //CV7 Read-Only
			return;
		case 8: //CV8 Read-Only
			if ((data == MANUFACTURENUM) || (data == 8)) {
				write_eeprom(0, 0xFF);
				initialize_cv();
			}
			return;
		case 17:
			CV17 = data;
			break;
		case 18:
			CV18 = data;
			break;
		case 19:
			CV19 = data;
			break;
		case 27:
			CV27 = data;
			break;
		case 29:
			if (CV64 & 0x01) {
				data = data | 0x08;	// Force bit3 overwrite (Enable Railcom)
			}
			CV29 = data;
			break;
/*
		case 33:
			CV33 = data;
			setFuncTable(CV33 & 0x1F);
			break;
		case 34:
			CV34 = data;
			setFuncTable(CV34 & 0x1F);
			break;
		case 35:
			CV35 = data;
			setFuncTable(CV35 & 0x1F);
			break;
		case 36:
			CV36 = data;
			setFuncTable(CV36 & 0x1F);
			break;
		case 37:
			CV37 = data;
			setFuncTable(CV37 & 0x1F);
			break;
*/
		case 33:
		case 34:
		case 35:
		case 36:
		case 37:
		case 38:
		case 39:
		case 40:
		case 41:
		case 42:
		case 43:
			CV33_43[CVnum2 - 33] = data;
			setFuncTable(CV33_43[CVnum2 - 33] & 0x1F);
			break;
		case 44:
			CV44 = data;
			setFuncTable(CV44 & 0x1F);
			break;
		case 45:
			CV45 = data;
			setFuncTable(CV45 & 0x1F);
			break;
		case 47:
			CV47 = data;
			break;
		case 50:
			CV50 = data;
			break;
		case 51:
			CV51 = data;
			break;
		case 52:
			CV52 = data;
			break;
		case 53:
			CV53 = data;
			break;
		case 54:
			CV54 = data;
			break;
		case 55:
		case 56:
		case 57:
			CV55_57[CVnum2 - 55] = data;
			break;
		case 58:
			CV58 = data;
			break;
		case 59:
			CV59 = data;
			break;
		case 60:
			CV60 = data;
			break;
		case 61:
			CV61 = data;
			break;
		case 62:
			CV62 = data;
			break;
		case 63:	// F5 Half Speed Mode for yard operation
			CV63 = data;
			setFuncTable(CV63 & 0x1F);
			break;
		case 64:
			CV64 = data;
			break;
		case 67:
		case 68:
		case 69:
		case 70:
		case 71:
		case 72:
		case 73:
		case 74:
		case 75:
		case 76:
		case 77:
		case 78:
		case 79:
		case 80:
		case 81:
		case 82:
		case 83:
		case 84:
		case 85:
		case 86:
		case 87:
		case 88:
		case 89:
		case 90:
		case 91:
		case 92:
		case 93:
		case 94:
			CV67_94[CVnum2 - 67] = data;
			break;
		
		/*
		case 112:
			CV112 = data;
			break;
		case 114:
			CV114 = data;
			break;
		case 115:
			CV115 = data;
			break;
		case 116:
			CV116 = data;
			break;
		*/
		case 112:
		case 113:
		case 114:
		case 115:
		case 116:
		case 117:
		case 118:
		case 119:
		case 120:
		case 121:
		case 122:
			CV112_CV122[CVnum2 - 112] = data;
			break;
		case 131:
			CV131 = data;
			break;
		case 138:
			CV138 = data;
			break;
		case 244:
			CV244 = data;
			break;
	}
	/*
	eeprom_busy_wait();
	eeprom_write_byte(CVnum2 - 1, data);
	eeprom_busy_wait();
	*/
	write_eeprom(CVnum2, data);
}

void write_cv_bit(uint16_t CVnum, uint8_t data)
{
	uint8_t int_t1, int_t2;
	
	int_t1 = data & 0x07;
	int_t2 = 1 << int_t1;
	int_t1 = read_cv(CVnum);

	if (data & 0x08) {
		//Bit = 1
		int_t1 |= int_t2;
	} else {
		//Bit = 0
		int_t1 &= ~int_t2;
	}

	write_cv_byte(CVnum, int_t1);
}




