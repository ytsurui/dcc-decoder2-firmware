#!/usr/bin/env python3
"""DCC reversal integration tests with production train, PWM, and light code.

Run: python3 tests/run_reversal_tests.py (host C compiler and sanitizers required).
Registers are mocked; this does not model motor inertia or AVR interrupt latency.
"""
import os
from pathlib import Path
import re
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
source = (ROOT/'source/source/peripheral/motor_tiny1606.c').read_text()
macros = set(re.findall(r'\b[A-Z][A-Z0-9_]*_(?:bm|gc)\b', source))
io = '''#ifndef MOCK_IO_H
#define MOCK_IO_H
#include <stdint.h>
typedef struct { uint8_t DIRCLR, DIRSET, OUTCLR, OUTSET; } Port;
extern Port PORTA, PORTB, PORTC;
typedef struct { struct { uint16_t CTRLA, CTRLB, PER, CMP0, CNT, INTCTRL, INTFLAGS; } SINGLE; } Timer;
extern Timer TCA0;
typedef struct { uint16_t CTRLA, CTRLB, CTRLC, CTRLD, CTRLE, CTRLF, COMMAND, MUXPOS, RES, SAMPLE; } Adc;
extern Adc ADC0;
extern struct Vref { uint16_t CTRLA; } VREF;
#define ADC0_COMMAND ADC0.COMMAND
'''
for macro in sorted(macros):
    value = 1 << int(macro[3]) if re.fullmatch(r'PIN\d_bm', macro) else 1
    io += f'#define {macro} {value}\n'
io += '#endif\n'
with tempfile.TemporaryDirectory(prefix='dcc-reversal-test-') as tmp:
    tmp = Path(tmp)
    (tmp/'avr').mkdir()
    (tmp/'avr/io.h').write_text(io)
    (tmp/'avr/interrupt.h').write_text('#define ISR(vector) void vector(void)\n')
    for name, train, motor, func in [
        ('studio', 'source/source/app/train_ctrl.c', 'source/source/peripheral/motor_tiny1606.c', 'source/source/app/func_ctrl.c'),
        ('arduino', 'arduino/motordecoder2/train_ctrl.c', 'arduino/motordecoder2/motor_tiny1606.c', 'arduino/motordecoder2/func_ctrl.c'),
    ]:
        for avr2 in ((False, True) if name == 'studio' else (False,)):
            binary = tmp/(name + ('-1626' if avr2 else '-1606'))
            defines = [f'-D{key}_SOURCE="{ROOT/path}"' for key, path in
                       [('TRAIN', train), ('MOTOR', motor), ('FUNC', func)]]
            subprocess.run([os.environ.get('CC', 'cc'), '-std=c99', '-O2',
                            '-fsanitize=undefined,address', '-I'+str(tmp)] +
                           (['-DAVR2'] if avr2 else []) + defines +
                           [str(ROOT/'tests/reversal_test.c'), '-o', str(binary)], check=True)
            print(binary.name, flush=True)
            subprocess.run([str(binary)], check=True)
