#!/usr/bin/env python3
"""Host regression tests: python3 tests/run_analog_tests.py (requires a C compiler)."""
import os
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
MOCK_IO = r'''
#ifndef MOCK_AVR_IO_H
#define MOCK_AVR_IO_H
#include <stdint.h>
typedef struct { uint8_t IN, INTFLAGS, DIRCLR, PIN2CTRL, PIN4CTRL; } MockPort;
typedef struct { uint16_t CNT, CCMP; uint8_t INTFLAGS, INTCTRL, CTRLA; } MockTimer;
typedef struct { struct { uint16_t CNT, CMP2; uint8_t INTFLAGS, INTCTRL; } SINGLE; } MockTca;
extern MockPort PORTA;
extern MockTimer TCB0;
extern MockTca TCA0;
#define PIN2_bm 4
#define PIN4_bm 16
#define TCB_CAPT_bm 1
#define TCB_CLKSEL_CLKDIV2_gc 2
#define TCB_ENABLE_bm 1
#define PORT_ISC_BOTHEDGES_gc 1
#define TCA_SINGLE_CMP2_bm 64
#endif
'''
with tempfile.TemporaryDirectory(prefix='analog-pwm-test-') as tmp:
    tmp = Path(tmp)
    (tmp/'avr').mkdir(); (tmp/'util').mkdir()
    (tmp/'avr/io.h').write_text(MOCK_IO)
    (tmp/'avr/interrupt.h').write_text('#define ISR(vector) void vector(void)\n#define cli() ((void)0)\n#define sei() ((void)0)\n')
    (tmp/'util/atomic.h').write_text('#define ATOMIC_RESTORESTATE 0\n#define ATOMIC_BLOCK(x) for (int atomic_once=1; atomic_once; atomic_once=0)\n')
    for name, analog, train, dcc, flags in [
        ('studio', 'source/source/peripheral', 'source/source/app', 'source/source/peripheral', ['-DEXPECT_WINDOW_CLOCKS=512']),
        ('arduino', 'arduino/motordecoder2', 'arduino/motordecoder2', 'arduino/motordecoder2', ['-DEXPECT_WINDOW_CLOCKS=512']),
        ('studio-func', 'source/source/peripheral', 'source/source/app', 'source/source/peripheral', ['-DATTINY806_FUNC', '-DNO_RAILCOM', '-DTEST_FUNCTION_DECODER', '-DEXPECT_WINDOW_CLOCKS=256']),
        ('arduino-func', 'arduino/funcdecoder2', 'arduino/funcdecoder2', 'arduino/funcdecoder2', ['-DTEST_FUNCTION_DECODER', '-DEXPECT_WINDOW_CLOCKS=256']),
    ]:
        sources = {'ANALOG_SOURCE': ROOT/analog/'analog_poller_tiny1606.c',
                   'TRAIN_SOURCE': ROOT/train/'train_ctrl.c',
                   'DCC_SOURCE': ROOT/dcc/'dcc_poller_tiny1606.c'}
        binary = tmp/name
        subprocess.run([os.environ.get('CC','cc'), '-std=c99', '-O2',
                        '-fsanitize=undefined,address', '-I'+str(tmp)] + flags +
                       ['-D'+key+'="'+str(path)+'"' for key,path in sources.items()] +
                       [str(ROOT/'tests/analog_pwm_test.c'), '-o',str(binary)],check=True)
        print(name, flush=True)
        subprocess.run([str(binary)], check=True)
