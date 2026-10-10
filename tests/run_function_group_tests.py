#!/usr/bin/env python3
"""Test function-group CV storage using production CV code and mocked EEPROM."""
import os
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
HARNESS = r'''
#include <assert.h>
/* Host tests use ordinary RAM for the AVR EEPROM section. */
#define __attribute__(attributes)
#include CV_SOURCE
#undef __attribute__
int main(void) {
    assert(read_cv(123) == 0x87 && read_cv(124) == 0x38);
    load_cv(); /* Fresh EEPROM initializes and loads defaults. */
    assert(CV123 == 0x87 && CV124 == 0x38);
    write_cv_byte(123, 0x07);
    write_cv_byte(124, 0x80);
    assert(CV123 == 0x07 && read_cv(123) == 0x07);
    assert(CV124 == 0x80 && read_cv(124) == 0x80);
    write_cv_bit(123, 0x0F); /* Set bit7. */
    write_cv_bit(124, 0x07); /* Clear bit7. */
    assert(CV123 == 0x87 && read_cv(123) == 0x87);
    assert(CV124 == 0 && read_cv(124) == 0);
    CV123 = CV124 = 0xFF;
    load_cv();
    assert(CV123 == 0x87 && CV124 == 0); /* Zero is valid. */
    write_cv_byte(15, 1); /* Lock must protect EEPROM and RAM. */
    write_cv_byte(123, 0);
    write_cv_byte(124, 0xFF);
    assert(CV123 == 0x87 && read_cv(123) == 0x87);
    assert(CV124 == 0 && read_cv(124) == 0);
    write_cv_byte(15, 0);
    write_cv_byte(123, 0);
    write_cv_byte(124, 0xFF);
    write_cv_byte(8, 8); /* User-visible factory reset. */
    assert(CV123 == 0x87 && read_cv(123) == 0x87);
    assert(CV124 == 0x38 && read_cv(124) == 0x38);
    /* Check both ends and every element of the unified CV33-CV45 range. */
    for (uint8_t cv = 33; cv <= 45; ++cv) {
        assert(CV33_45[cv - 33] == read_cv(cv));
        write_cv_byte(cv, cv - 32);
        assert(CV33_45[cv - 33] == cv - 32);
        assert(read_cv(cv) == cv - 32);
        CV33_45[cv - 33] = 0;
    }
    load_cv();
    for (uint8_t cv = 33; cv <= 45; ++cv)
        assert(CV33_45[cv - 33] == cv - 32);
    for (uint8_t i = 0; i < 11; ++i)
        assert(CV112_CV122[i] == read_cv(112 + i));
    assert(CV123 == 0x87 && CV124 == 0x38);
    write_cv_bit(44, 0x0F);
    assert(CV33_45[11] == (12 | 0x80));
    assert(read_cv(44) == (12 | 0x80));
    write_cv_bit(45, 0x00);
    assert(CV33_45[12] == 12 && read_cv(45) == 12);
    write_cv_byte(8, 8);
    assert(CV33_45[0] == CV33_DEFAULT);
    assert(CV33_45[10] == CV43_DEFAULT);
    assert(CV33_45[11] == CV44_DEFAULT);
    assert(CV33_45[12] == CV45_DEFAULT);
    return 0;
}
'''

with tempfile.TemporaryDirectory(prefix='dcc-group-test-') as directory:
    tmp = Path(directory)
    (tmp / 'avr').mkdir()
    (tmp / 'avr/io.h').write_text('#include <stdint.h>\n')
    (tmp / 'avr/eeprom.h').write_text('''#include <stdint.h>
static inline void eeprom_busy_wait(void) {}
static inline uint8_t eeprom_read_byte(const uint8_t *p) { return *p; }
static inline void eeprom_update_byte(uint8_t *p, uint8_t v) { *p = v; }
''')
    harness = tmp / 'test.c'
    harness.write_text(HARNESS)
    for name, source, function_only in [
        ('studio-motor', 'source/source/dcc/cv.c', False),
        ('studio-func', 'source/source/dcc/cv.c', True),
        ('arduino-motor', 'arduino/motordecoder2/cv.c', False),
        ('arduino-func', 'arduino/funcdecoder2/cv.c', True),
    ]:
        binary = tmp / name
        subprocess.run([
            os.environ.get('CC', 'cc'), '-std=c99', '-O2',
            '-fsanitize=address,undefined', '-I' + str(tmp),
            '-DCV_SOURCE="' + str(ROOT / source) + '"',
        ] + (['-DATTINY806_FUNC'] if function_only else []) + [
            str(harness), '-o', str(binary),
        ], check=True)
        subprocess.run([str(binary)], check=True)
        print(name + ': passed', flush=True)
