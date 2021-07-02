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
	/*  48 */	0x00,
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

};
