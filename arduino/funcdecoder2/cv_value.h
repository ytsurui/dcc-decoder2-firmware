/*
 * cv_value.h
 *
 * Created: 2021/02/06 21:54:36
 *  Author: Y.Tsurui
 */ 


#define CV_COUNT	255

#define HARDWARE_NUM	0x05	// Version 1.0.5
#define MANUFACTURENUM	0x0D

#define CV1_DEFAULT		3		//7-Bit Addr
#define CV2_DEFAULT		0		//Vstart
#define CV3_DEFAULT		0		//Acceleration Rate
#define CV4_DEFAULT		0		//Deaceleration Rate
#define CV5_DEFAULT		255		//Vmax
#define CV6_DEFAULT		127		//Vmid

#define CV15_DEFAULT	0		// Decoder Lock
#define CV16_DEFAULT	0		// Decoder Lock
#define CV17_DEFAULT	192		//14-Bit Addr
#define CV18_DEFAULT	0		//14-Bit Addr

#define CV27_DEFAULT	0b00000011		// Decoder Automatic Stopping Configuration
/*
 *	// CV27 Configuration
 *		bit0: Enable Automatic Brake Function On Right Rail (ABC+/MOTOR+)
 *		bit0: Enable Automatic Brake Function On Left Rail (ABC-/MOTOR-)
 */

#define CV29_DEFAULT	0x0E	//Basic Configuration

/* CV29 Configuration
 *		bit0: Swap Direction Forward/Backward
 *		bit1: 7-bit Addr Use
 *		bit2: Enable Analog Operation Mode
 *		bit3: Enable Railcom Address Feedback
 *		bit5: 14-bit Addr Use
 */

#define CV33_DEFAULT	4				// F4 Headlight Dimming
#define CV34_DEFAULT	7				// F7 Taillight Yard Operation
#define CV35_DEFAULT	0b01000000		// F0 Forward	(Func1)
#define CV36_DEFAULT	0b10000001		// F1 Backward	(Func2)
#define CV37_DEFAULT	0b10000001		// F1 Backward	(Func3)
#define CV38_DEFAULT	0b10000000		// F0 Backward	(Func4)
#define CV39_DEFAULT	0b01000001		// F1 Forward	(Func5)
#define CV40_DEFAULT	0b01000001		// F1 Forward	(Func6)
#define CV41_DEFAULT	0b00000011		// F3 Roomlight	(Func7)
#define CV42_DEFAULT	0b00000000		// F0 (Func8, MotorDriver)
//#ifdef ATTINY806_FUNC
#define CV43_DEFAULT	0b00000001		// MotorDriver Function Enable (0: Disabled / 1: Enabled)
//#else
//#define CV43_DEFAULT	0b00000000		// MotorDriver Function Enable (0: Disabled / 1: Enabled)
//#endif
#define CV44_DEFAULT	0b00111110		// Headlight On Backward (Disabled)
#define CV45_DEFAULT	0b00111110		// Headlight On Forward (Disabled)
//#define CV50_DEFAULT	0b11001001		// Analog Function Enable Flag (bit7-0: Func8-Func1 Enable)
#define CV50_DEFAULT	0b11111111		// Analog Function Enable Flag (bit7-0: Func8-Func1 Enable)

//#define CV47_DEFAULT	0xCF	//PWM KickStart Configuration (Bit0-5: Timing, Bit6: Freq (0: 120Hz / 1: 60Hz), Bit7: Enable
#define CV47_DEFAULT	0x4F	//PWM KickStart Configuration (Bit0-5: Timing, Bit6: Freq (0: 120Hz / 1: 60Hz), Bit7: Enable

#define CV51_DEFAULT	255		// Automatic Brake Ignore Function (Default: Disable)
#define CV52_DEFAULT	0		// Automatic Brake Auto-Reverse (0: Disabled / 1-255: Stop seconds)
#define CV53_DEFAULT	10		// Automatic Brake Acceleration Rate
#define CV54_DEFAULT	3		// Automatic Brake Deacceleration Rate
#define CV48_DEFAULT	7		// ABC Detector Threshold Value
#define CV148_DEFAULT	3		// ABC status-back wait count

#define CV55_DEFAULT	30		// BEMF PID Feedback Kp Raw Value
#define CV56_DEFAULT	10		// BEMF PID Feedback Ki Raw Value
#define CV57_DEFAULT	40		// BEMF PID Feedback Kd Raw Value
//#define CV55_DEFAULT	17		// BEMF PID Feedback Kp Raw Value
//#define CV56_DEFAULT	10		// BEMF PID Feedback Ki Raw Value
//#define CV57_DEFAULT	2		// BEMF PID Feedback Kd Raw Value
#define CV138_DEFAULT	100		// BEMF PID Feedback ADC Scaler Max Value

#define CV58_DEFAULT	0x7F	// Analog Max Speed
#define CV59_DEFAULT	0x03	// Analog Acceleration Rate

#define CV60_DEFAULT	0x83	// Super Slow Configuration (0: Disable / 1: 60Hz / 2: 120Hz / 3: 30Hz) / Bit8: BEMF On/Off
#define CV61_DEFAULT	0x5F	// Super Slow Initialize Duty (0-255)
#define CV62_DEFAULT	0xC0	// Super Slow End Speed (0-255)

#define CV63_DEFAULT	7				// F7 Slow Speed Mode for yard operation
#define CV64_DEFAULT	0b00000001		// Force Railcom Enabled (CV29 bit3 overwrite)


#define CV131_DEFAULT	0b00111111		// Slow Speed Mode Max Speed
#define CV112_DEFAULT	0b01000000		// Headlight Dimming illuminate value
//#define CV114_DEFAULT	0b11110001		// Func1 Type (Headlight)
//#define CV115_DEFAULT	0b11110001		// Func2 Type (Headlight)
//#define CV116_DEFAULT	0b11110011		// Func3 Type (Taillight left)
//#define CV117_DEFAULT	0b11110010		// Func4 Type (Taillight right)
//#define CV118_DEFAULT	0b11110011		// Func5 Type (Taillight left)
//#define CV119_DEFAULT	0b11110010		// Func6 Type (Taillight right)
//#define CV120_DEFAULT	0b11110000		// Func7 Type (Roomlight/Normal)
#define CV114_DEFAULT	0b11110001		// Func1 Type (Headlight)
#define CV115_DEFAULT	0b11110011		// Func2 Type (Taillight left)
#define CV116_DEFAULT	0b11110010		// Func3 Type (Taillight right)
#define CV117_DEFAULT	0b11110001		// Func4 Type (Headlight)
#define CV118_DEFAULT	0b11110011		// Func5 Type (Taillight left)
#define CV119_DEFAULT	0b11110010		// Func6 Type (Taillight right)
#define CV120_DEFAULT	0b11110100		// Func7 Type (Roomlight/Roomlight Type1)
#define CV121_DEFAULT	0b11110001		// Func8 Forward Type (Headlight)
#define CV122_DEFAULT	0b11110010		// Func8 Backward Type (Taillight)

#define CV140_DEFAULT   0               // Motor Start Delay Time (1/10sec, 0=Disable)

#define CV244_DEFAULT   0x00            // Railcom Debug-Output (0: Normal / 1: BEMF ADC)


uint8_t CV1_6[6];

//uint8_t CV1;		// 8-bit Addr
//uint8_t CV2;
//uint8_t CV3;
//uint8_t CV4;
//uint8_t CV5;
//uint8_t CV6;
uint8_t CV17;		// 14-Bit Addr MSB
uint8_t CV18;		// 14-Bit Addr LSB
uint8_t CV19;		// Consist Addr
uint8_t CV27;		// Decoder Automatic Stopping Configuration
uint8_t CV29;		// Decoder Configuration
//uint8_t CV33;		// Headlight Dimming
//uint8_t CV34;		// Taillight Configuration
//uint8_t CV35;		// Func1 Address Configuration		
//uint8_t CV36;		// Func2 Address Configuration

uint8_t CV33_43[11];

uint8_t CV44;		// Headlight Only Function Backward
uint8_t CV45;		// Headlight Only Function Forward

/*
#ifndef ATTINY806_FUNC
uint8_t CV47;		// PWM KickStart Configuration
#endif
*/

uint8_t CV50;		// Analog Function Enable Flag (bit7-0: Func8-Func1 Enable)

/*
#ifndef ATTINY806_FUNC
uint8_t CV51;		// Automatic Brake Ignore Function
uint8_t CV52;		// Automatic Brake Auto-Reverse (0: Disabled / 1-255: Stop seconds)
uint8_t CV53;		// Automatic Brake Acceleration Rate
uint8_t CV54;		// Automatic Brake Deacceleration Rate
uint8_t CV48;		// ABC Detector Threshold Value
uint8_t CV148;		// ABC status-back wait count

//uint8_t CV55;		// BEMF PID Feedback Kp Raw Value
//uint8_t CV56;		// BEMF PID Feedback Ki Raw Value
//uint8_t CV57;		// BEMF PID Feedback Kd Raw Value
uint8_t CV55_57[3];	// BEMF PID Feedback Values;
uint8_t CV138;		// BEMF PID Feedback ADC Scaler Max Value;

uint8_t CV58;		// Analog Max Speed
uint8_t CV59;		// Analog Acceleration Rate
#endif
*/
//uint8_t CV60;		// Super Slow Configuration (0: Disable / 1: 60Hz / 2: 120Hz / 3: 30Hz)
//uint8_t CV61;		// Super Slow Initialize Duty (0-255)
//uint8_t CV62;		// Super Slow End Speed (0-255)
//uint8_t CV63;		// F5 Half Speed Mode for yard operation
//uint8_t CV64;		// Force Railcom Enabled (CV29 bit3 Overwrite)
uint8_t CV60_64[5];

/*
#ifndef ATTINY806_FUNC
uint8_t CV67_94[28];	// Speed Table
#endif
*/

//uint8_t CV112;		// Headlight illuminate Value (bit7-bit4)
//uint8_t CV114;		// Func1 Type Configuration
//uint8_t CV115;		// Func2 Type Configuration

uint8_t CV112_CV122[11];

/*
#ifndef ATTINY806_FUNC
uint8_t CV131;				// Yard Mode Max Speed

uint8_t CV140;      // Motor Start Delay Time (1/10sec)


uint8_t CV244;      // Railcom Debug-Output (0: Normal / 1: BEMF ADC)
#endif
*/

uint8_t funcTable1;
uint8_t funcTable2;
uint8_t funcTable3;
uint8_t funcTable4;
uint8_t funcTable5;


/*	
 *	Function Configurations
 *		
 *		bit7-6: Direction Control
 *					00:	  Always
 *					01:	  Forward Only
 *					10:	  Backward Only
 *		bit5: Analog Enabled
 *					0: Analog Enabled
 *					1: Analog Disabled
 *		bit4-bit0:	Function Number
 *					0-28: Function Num
 *					30:	  Function Disabled
 *					31:   Always Enabled
 *
 */

/*
 *	Function Type Configurations
 *
 *	Lower-bit (bit3-bit0)
 *		0000:	Normal Function
 *		0001:	Headlight
 *		0010:	Taillight right
 *		0011:	Taillight left
 *      0100:   Roomlight Type1
 *      0101:	Roomlight Type2
 *		0110:	Roomlight Type3
 *
 *	Higher-bit (bit7-bit4)
 *		Function illuminate Value (0x0-0xF)
 *
 */

