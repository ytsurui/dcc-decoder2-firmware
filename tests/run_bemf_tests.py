#!/usr/bin/env python3
"""Test production BEMF selection with mock registers; no hardware timing simulation.

Run: python3 tests/run_bemf_tests.py (requires cc and sanitizers).
"""
from pathlib import Path
import re, subprocess
root=Path(__file__).resolve().parents[1]
import tempfile
_tmp=tempfile.TemporaryDirectory(prefix="bemf-test-")
w=Path(_tmp.name)
(w/'avr').mkdir(exist_ok=True)
s=(root/'source/source/peripheral/motor_tiny1606.c').read_text()
macros=set(re.findall(r'\b(?:[A-Z][A-Z0-9_]*_(?:bm|gc))\b',s))
io='''#include <stdint.h>
typedef struct { uint8_t DIRCLR,DIRSET,OUTCLR,OUTSET; } Port;
extern Port PORTA,PORTB,PORTC;
typedef struct { struct {uint16_t CTRLA,CTRLB,PER,CMP0,CNT,INTCTRL,INTFLAGS;} SINGLE;} Timer;
extern Timer TCA0;
typedef struct {uint16_t CTRLA,CTRLB,CTRLC,CTRLD,CTRLE,CTRLF,COMMAND,MUXPOS,RES,SAMPLE;} Adc;
extern Adc ADC0;
extern struct Vref {uint16_t CTRLA;} VREF;
#define ADC0_COMMAND ADC0.COMMAND
'''
for m in sorted(macros):
 v=1<<int(m[3]) if re.fullmatch(r'PIN\d_bm',m) else 1
 io+=f'#define {m} {v}\n'
(w/'avr/io.h').write_text(io)
(w/'avr/interrupt.h').write_text('#define ISR(x) void x(void)\n')

for variant, source in [('studio', root/'source/source/peripheral/motor_tiny1606.c'), ('arduino', root/'arduino/motordecoder2/motor_tiny1606.c')]:
 (w/'check.c').write_text('#include <assert.h>\n#include <stdio.h>\n#include "'+str(source)+'"\n'+r''' 
Port PORTA,PORTB,PORTC; Timer TCA0; Adc ADC0; struct Vref VREF;
static uint8_t analogMode;
uint8_t readAnalogStat(void) { return analogMode; }
int main(void) {
 assert(CV60_DEFAULT == 0x83);
 for(unsigned mode=0; mode<2; ++mode) {
  analogMode=mode;
  for(unsigned flags=0; flags<4; ++flags) {
   for(unsigned low=0; low<4; ++low) {
    CV60_64[0]=(flags<<6)|low;
    unsigned enabled=!!(CV60_64[0] & (mode ? 0x40 : 0x80));
    bemfReadFlag=0; pwmProgModeFlag=0; CV33_43[10]=0;
    nowSPDvalue=100; fixedSPDvalue=150; bemfSPDvalue=200;
    superslowValue=128; superslowCounter=0; superslowBaseCounter=0;
    HSclockReceiverMotorCtrl();
    assert(TCA0.SINGLE.CMP0==(enabled ? 200 : (low ? 150 : 100)));
    assert(superslowCounter==(low==2 ? 2 : (low ? 1 : 0)));
    captureBEMF();
    assert(bemfReadFlag==(enabled ? 1 : 0));
    if(enabled) {
     HSclockReceiverMotorCtrl();
     assert(TCA0.SINGLE.CMP0==0); /* measurement briefly cuts motor PWM */
    }
   }
  }
 }
 puts("PASS: CV60 default, both travel modes, all four BEMF combinations and all four low-speed modes; capture gating and PWM selection");
}
''')
 subprocess.run(['cc','-O2','-I'+str(w),'-fsanitize=undefined,address',str(w/'check.c'),'-o',str(w/'check')],check=True)
 print(variant,flush=True)
 subprocess.run([str(w/'check')],check=True)
