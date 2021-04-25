/*
 * func_effect.c
 *
 * Created: 2021/04/25 14:01:56
 *  Author: Y.Tsurui
 */ 

#include <avr/io.h>

/*
 *	return: illuminateValue
 */
uint8_t funcEffect(uint8_t stat, uint8_t cvType, uint8_t counter, uint16_t *effectCounter, uint8_t *ctrlOverride, uint8_t *HSfuncValue) {
	uint8_t iValue;
	uint8_t iValue2;
	
	iValue = (cvType & 0xF0) >> 4;
	if ((cvType & 0x0F) == 0x01) {
		// Headlight
		
		if (stat == 0) {
			*effectCounter = 0;
			*HSfuncValue = 0;
			*ctrlOverride = 0;
			return (0);
		}
		if (*effectCounter != 0) {
			iValue2 = (*effectCounter >> 2);
			if ((iValue2 >> 4) >= iValue) {
				*effectCounter = 0;
				*HSfuncValue = (iValue2 & 0xF0) + 0x0F;
				*ctrlOverride = 1;
				return (iValue);
			}
			*HSfuncValue = iValue2;
			*ctrlOverride = 1;
			return (iValue2);
		} else {
			*HSfuncValue = (cvType & 0xF0) + 0x0F;
			*ctrlOverride = 1;
		}
		
		
		
	}
	
	
	
	
	//return (stat);
	//iValue = (cvType & 0xF0) >> 4;
	*effectCounter = 0;
	*ctrlOverride = 0;
	return (iValue);
	
	
	
}
