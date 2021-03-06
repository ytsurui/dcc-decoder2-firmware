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
	static uint8_t effectInternalCounter;
	
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
				//return (iValue);
				return ((iValue << 4) | 0x0F);
			}
			*HSfuncValue = iValue2;
			*ctrlOverride = 1;
			return (iValue2);
		} else {
			*HSfuncValue = (cvType & 0xF0) + 0x0F;
			*ctrlOverride = 1;
		}
	} else if ((cvType & 0x0F) == 0x04) {
		// Roomlight Type1
		
		if (stat == 0) {
			*effectCounter = 0;
			*HSfuncValue = 0;
			*ctrlOverride = 0;
			return (0);
		}
		
		/*
		if (*effectCounter != 0) {
			if (*effectCounter < 60) {
				*ctrlOverride = 0;
				*HSfuncValue = 0;
				//return (iValue);
				return ((iValue << 4) | 0x0F);
			}
			if (*effectCounter < 120) {
				*ctrlOverride = 1;
				*HSfuncValue = 0;
				return (1);
			}
			if (*effectCounter < 180) {
				*ctrlOverride = 0;
				*HSfuncValue = 0;
				//return (iValue);
				return ((iValue << 4) | 0x0F);
			}
			if (*effectCounter < 400) {
				*ctrlOverride = 1;
				*HSfuncValue = 0;
				return (1);
			}
			
			*effectCounter = 0;
			*ctrlOverride = 0;
			*HSfuncValue = 0;
			//return (iValue);
			return ((iValue << 4) | 0x0F);
		}
		*/
		if (*effectCounter != 0) {
			if (*effectCounter < 40) {
				*ctrlOverride = 1;
				*HSfuncValue = 0;
				effectInternalCounter = 0;
				return ((iValue << 2) | 0x03);	// 50% -> 25%
			}
			if (*effectCounter < 130) {
				*ctrlOverride = 1;
				*HSfuncValue = 0;
				return (iValue);	// 20% -> 8%
			}
			if (*effectCounter < 180) {
				*ctrlOverride = 1;
				*HSfuncValue = 0;
				//return ((iValue << 4) | 0x0F) * 0.7;	// 70% -> 50%
				return ((iValue << 3) | 0x07);
			}
			if (*effectCounter < 210) {
				*ctrlOverride = 1;
				*HSfuncValue = 0;
				//return ((iValue << 4) | 0x0F) * 0.7;	// 70% -> 50%
				return ((iValue << 2) | 0x03);	// 50% -> 25%
			}
			
			if (*effectCounter < 380) {
				if (*effectCounter & 0x08) {
					*ctrlOverride = 1;
					*HSfuncValue = 0;
					effectInternalCounter++;
					return (0x02 * effectInternalCounter);
				} else {
					*ctrlOverride = 1;
					*HSfuncValue = 0;
					//return ((iValue << 4) | 0x0F) * 0.4;	// 40%
					//return ((iValue << 1) | 0x01);	// 50% -> 25%
					return (iValue);
				}
			}
			/*
			if (*effectCounter < 375) {
				*ctrlOverride = 1;
				*HSfuncValue = 0;
				//return ((iValue << 4) | 0x0F) * 0.4;	// 40%
				return ((iValue << 1) | 0x01);	// 50% -> 25%
			}
			*/
			
			*effectCounter = 0;
			*ctrlOverride = 0;
			*HSfuncValue = 0;
			//return (iValue);
			return ((iValue << 4) | 0x0F);
			
			
		}
		
		
	} else if ((cvType & 0x0F) == 0x05) {
		// Roomlight Type2
		if (stat == 0) {
			*effectCounter = 0;
			*HSfuncValue = 0;
			*ctrlOverride = 0;
			return (0);
		}
		
		if (*effectCounter != 0) {
			if (*effectCounter >= 400) {
				*effectCounter = 0;
				*ctrlOverride = 0;
				*HSfuncValue = 0;
				//return (iValue);
				return ((iValue << 4) | 0x0F);
			}
			
			iValue2 = 0;
			*ctrlOverride = 0;
			*HSfuncValue = 0;
			return (iValue2);
			
		}
	} else if ((cvType & 0x0F) == 0x06) {
		// Roomlight Type3
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
				//return (iValue);
				return ((iValue << 4) | 0x0F);
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
	//return (iValue);
	return ((iValue << 4) | 0x0F);
	
	
	
}
