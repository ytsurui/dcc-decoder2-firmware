/*
 * cv_eeprom.h
 *
 * Created: 2021/02/06 21:53:52
 *  Author: Y.Tsurui
 */ 



#include <avr/eeprom.h>
uint8_t __attribute__((section(".eeprom"))) val[] =
/*  CV */ {
	/*	 0 */	0xFF,			// Configure Area
	/*   1 */	CV1_DEFAULT,	//7-bit Addr (1-127)
	/*   2 */	CV2_DEFAULT,	//Vstart (0-255)
	/*   3 */	CV3_DEFAULT,	//Acceleration Rate
	/*   4 */	CV4_DEFAULT,	//Deaceleration Rate
	/*   5 */	CV5_DEFAULT,	//Vhigh (0-255)
	/*   6 */	CV6_DEFAULT,	//Vmid	(0-255)
	/*   7 */	HARDWARE_NUM,	//Version Number
	/*   8 */	MANUFACTURENUM,	//Manufacture Number (Public Domain & Do-It-Yourself Decoders: 0x0D)
	/*   9 */	0x00,
	/*  10 */	0x00,
	/*  11 */	0x00,
	/*  12 */	0x00,
	/*  13 */	0x00,
	/*  14 */	0x00,
	/*  15 */	0x00,			//Decoder Lock Value
	/*  16 */	0x00,			//Decoder Lock Value
	/*  17 */	CV17_DEFAULT,	//14-bit Addr MSB
	/*  18 */	CV18_DEFAULT,	//14-bit Addr LSB
	/*  19 */	0x00,			//Consist Addr
	/*  20 */	0x00,
	/*  21 */	0x00,
	/*  22 */	0x00,
	/*  23 */	0x00,
	/*  24 */	0x00,
	/*  25 */	0x00,
	/*  26 */	0x00,
	/*  27 */	CV27_DEFAULT,	// Decoder Automatic Stopping Configuration
	/*  28 */	0x00,
	/*  29 */	CV29_DEFAULT,	//Bit0: Swap Forward/Reverse / bit1: 7-bit Addr / bit2: Analog Convert /bit3: Railcom Addr Feedback / bit5: 14-bit Addr
	/*  30 */	0x00,
	/*  31 */	0x00,
	/*  32 */	0x00,
	/*  33 */	CV33_DEFAULT,	// Headlight Dimming Configuration (bit7-bit0)
	/*  34 */	CV34_DEFAULT,	//  Configuration (bit7-bit0)
	/*  35 */	CV35_DEFAULT,	// Func1 Configuration (bit7-bit0)
	/*  36 */	CV36_DEFAULT,	// Func2 Configuration (bit7-bit0)
	/*  37 */	CV37_DEFAULT,	// Func3 Configuration (bit7-bit0)
	/*  38 */	CV38_DEFAULT,	// Func4 Configuration (bit7-bit0)
	/*  39 */	CV39_DEFAULT,	// Func5 Configuration (bit7-bit0)
	/*  40 */	CV40_DEFAULT,	// Func6 Configuration (bit7-bit0)
	/*  41 */	CV41_DEFAULT,	// Func7 Configuration (bit7-bit0)
	/*  42 */	CV42_DEFAULT,	// Func8 Configuration (bit7-bit0)
	/*  43 */	CV43_DEFAULT,	// MotorDriver Function Enable (0: Disabled / 1: Enabled)
	/*  44 */	CV44_DEFAULT,
	/*  45 */	CV45_DEFAULT,	// Headlight Always On Function
	/*  46 */	0x00,
	/*  47 */	CV47_DEFAULT,	// PWM KickStart Configuration
	/*  48 */	CV48_DEFAULT,	// ABC Threshold
	/*  49 */	0x00,
	/*  50 */	CV50_DEFAULT,	// Analog Function Enable Flag (bit7-0: Func8-Func1 Enable)
	/*  51 */	CV51_DEFAULT,	// Automatic Brake Ignore Function (Default: F5)
	/*  52 */	CV52_DEFAULT,	// Automatic Brake Auto-Reverse (0: Disabled / 1-255: Stop seconds)
	/*  53 */	CV53_DEFAULT,	// Automatic Brake Acceleration Rate
	/*  54 */	CV54_DEFAULT,	// Automatic Brake Deacceleration Rate
	/*  55 */	CV55_DEFAULT,	// BEMF PID Feedback Kp Raw Value
	/*	56 */	CV56_DEFAULT,	// BEMF PID Feedback Ki Raw Value
	/*  57 */	CV57_DEFAULT,	// BEMF PID Feedback Kd Raw Value
	/*  58 */	CV58_DEFAULT,	// Analog Max Speed
	/*  59 */	CV59_DEFAULT,	// Analog Acceleration Rate
	/*  60 */	CV60_DEFAULT,	// Super Slow Configuration
	/*  61 */	CV61_DEFAULT,	// Super Slow Initialize Duty (0-255)
	/*  62 */	CV62_DEFAULT,	// Super Slow End Speed (0-255)
	/*  63 */	CV63_DEFAULT,	// F7 Slow Speed Mode for yard operation
	/*  64 */	CV64_DEFAULT,	// bit0: Force Railcom Enabled (CV29 bit3 overwrite, CV64 bit0=1)
	/*  65 */	0x00,
	/*  66 */	0x00,
	/*  67 */	9,		//Speed Table
	/*  68 */	18,		//Speed Table
	/*  69 */	27,		//Speed Table
	/*  70 */	36,		//Speed Table
	/*  71 */	46,		//Speed Table
	/*  72 */	55,		//Speed Table
	/*  73 */	64,		//Speed Table
	/*  74 */	73,		//Speed Table
	/*  75 */	82,		//Speed Table
	/*  76 */	91,		//Speed Table
	/*  77 */	100,	//Speed Table
	/*  78 */	109,	//Speed Table
	/*  79 */	118,	//Speed Table
	/*  80 */	128,	//Speed Table
	/*  81 */	137,	//Speed Table
	/*  82 */	146,	//Speed Table
	/*  83 */	155,	//Speed Table
	/*  84 */	164,	//Speed Table
	/*  85 */	173,	//Speed Table
	/*  86 */	182,	//Speed Table
	/*  87 */	191,	//Speed Table
	/*  88 */	200,	//Speed Table
	/*  89 */	209,	//Speed Table
	/*  90 */	219,	//Speed Table
	/*  91 */	228,	//Speed Table
	/*  92 */	237,	//Speed Table
	/*  93 */	246,	//Speed Table
	/*  94 */	255,	//Speed Table
	/*  95 */	0x00,
	/*  96 */	0x00,
	/*  97 */	0x00,
	/*  98 */	0x00,
	/*  99 */	0x00,
	/* 100 */	0x00,
	/* 101 */	0x00,
	/* 102 */	0x00,
	/* 103 */	0x00,
	/* 104 */	0x00,
	/* 105 */	0x00,
	/* 106 */	0x00,
	/* 107 */	0x00,
	/* 108 */	0x00,
	/* 109 */	0x00,
	/* 110 */	0x00,
	/* 111 */	0x00,
	/* 112 */	CV112_DEFAULT,	// Headlight Dimming illuminate value
	/* 113 */	0x00,
	/* 114 */	CV114_DEFAULT,	// Func1 Type (Headlight)
	/* 115 */	CV115_DEFAULT,	// Func2 Type (Taillight left)
	/* 116 */	CV116_DEFAULT,	// Func3 Type (Taillight right)
	/* 117 */	CV117_DEFAULT,	// Func4 Type (Headlight)
	/* 118 */	CV118_DEFAULT,	// Func5 Type (Taillight left)
	/* 119 */	CV119_DEFAULT,	// Func6 Type (Taillight right)
	/* 120 */	CV120_DEFAULT,	// Func7 Type (Roomlight/Normal)
	/* 121 */	CV121_DEFAULT,	// Func8 Forward Type (Headlight)
	/* 122 */	CV122_DEFAULT,	// Func8 Backward Type (Taillight)
	/* 123 */	0x00,
	/* 124 */	0x00,
	/* 125 */	0x00,
	/* 126 */	0x00,
	/* 127 */	0x00,
#ifndef ATTINY806_FUNC
	/* 128 */	0x00,
	/* 129 */	0x00,
	/* 130 */	0x00,
	/* 131 */	CV131_DEFAULT,	// Slow Speed Mode Max Speed
	/* 132 */	0x00,
	/* 133 */	0x00,
	/* 134 */	0x00,
	/* 135 */	0x00,
	/* 136 */	0x00,
	/* 137 */	0x00,
	/* 138 */	CV138_DEFAULT,	// BEMF PID Feedback ADC Scaler Max Value
	/* 139 */	0x00,
	/* 140 */	CV140_DEFAULT,	// Motor Start Delay Time (1/10sec)
	/* 141 */	0x00,
	/* 142 */	0x00,
	/* 143 */	0x00,
	/* 144 */	0x00,
	/* 145 */	0x00,
	/* 146 */	0x00,
	/* 147 */	0x00,
	/* 148 */	CV148_DEFAULT,	// ABC Status-Back wait count
	/* 149 */	0x00,
	/* 150 */	0x00,
	/* 151 */	0x00,
	/* 152 */	0x00,
	/* 153 */	0x00,
	/* 154 */	0x00,
	/* 155 */	0x00,
	/* 156 */	0x00,
	/* 157 */	0x00,
	/* 158 */	0x00,
	/* 159 */	0x00,
	/* 160 */	0x00,
	/* 161 */	0x00,
	/* 162 */	0x00,
	/* 163 */	0x00,
	/* 164 */	0x00,
	/* 165 */	0x00,
	/* 166 */	0x00,
	/* 167 */	0x00,
	/* 168 */	0x00,
	/* 169 */	0x00,
	/* 170 */	0x00,
	/* 171 */	0x00,
	/* 172 */	0x00,
	/* 173 */	0x00,
	/* 174 */	0x00,
	/* 175 */	0x00,
	/* 176 */	0x00,
	/* 177 */	0x00,
	/* 178 */	0x00,
	/* 179 */	0x00,
	/* 180 */	0x00,
	/* 181 */	0x00,
	/* 182 */	0x00,
	/* 183 */	0x00,
	/* 184 */	0x00,
	/* 185 */	0x00,
	/* 186 */	0x00,
	/* 187 */	0x00,
	/* 188 */	0x00,
	/* 189 */	0x00,
	/* 190 */	0x00,
	/* 191 */	0x00,
	/* 192 */	0x00,
	/* 193 */	0x00,
	/* 194 */	0x00,
	/* 195 */	0x00,
	/* 196 */	0x00,
	/* 197 */	0x00,
	/* 198 */	0x00,
	/* 199 */	0x00,
	/* 200 */	0x00,
	/* 201 */	0x00,
	/* 202 */	0x00,
	/* 203 */	0x00,
	/* 204 */	0x00,
	/* 205 */	0x00,
	/* 206 */	0x00,
	/* 207 */	0x00,
	/* 208 */	0x00,
	/* 209 */	0x00,
	/* 210 */	0x00,
	/* 211 */	0x00,
	/* 212 */	0x00,
	/* 213 */	0x00,
	/* 214 */	0x00,
	/* 215 */	0x00,
	/* 216 */	0x00,
	/* 217 */	0x00,
	/* 218 */	0x00,
	/* 219 */	0x00,
	/* 220 */	0x00,
	/* 221 */	0x00,
	/* 222 */	0x00,
	/* 223 */	0x00,
	/* 224 */	0x00,
	/* 225 */	0x00,
	/* 226 */	0x00,
	/* 227 */	0x00,
	/* 228 */	0x00,
	/* 229 */	0x00,
	/* 230 */	0x00,
	/* 231 */	0x00,
	/* 232 */	0x00,
	/* 233 */	0x00,
	/* 234 */	0x00,
	/* 235 */	0x00,
	/* 236 */	0x00,
	/* 237 */	0x00,
	/* 238 */	0x00,
	/* 239 */	0x00,
	/* 240 */	0x00,
	/* 241 */	0x00,
	/* 242 */	0x00,
	/* 243 */	0x00,
	/* 244 */	CV244_DEFAULT,	// Railcom Debug-Output (0: Normal / 1: BEMF ADC / 2: PID SPD Value)
	/* 245 */	0x00,
	/* 246 */	0x00,
	/* 247 */	0x00,
	/* 248 */	0x00,
	/* 249 */	0x00,
	/* 250 */	0x00,
	/* 251 */	0x00,
	/* 252 */	0x00,
	/* 253 */	0x00,
	/* 254 */	0x00,
	/* 255 */	0x00
#endif
};
