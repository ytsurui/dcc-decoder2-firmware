/*
 * cv.h
 *
 * Created: 2021/02/06 21:52:29
 *  Author: Y.Tsurui
 */ 


uint8_t read_cv(uint16_t);
void load_cv(void);
void write_cv_byte(uint16_t, uint8_t);
void write_cv_bit(uint16_t, uint8_t);
