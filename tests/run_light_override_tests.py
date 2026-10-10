#!/usr/bin/env python3
"""Test grouped light overrides with production control and effect code."""
import os
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
with tempfile.TemporaryDirectory(prefix='dcc-light-test-') as directory:
    tmp = Path(directory)
    (tmp / 'avr').mkdir()
    (tmp / 'avr/io.h').write_text('#include <stdint.h>\n')
    harness = tmp / 'test.c'
    harness.write_text((ROOT / 'tests/light_override_test.c').read_text())
    for name, source, function_only in [
        ('studio-motor', 'source/source/app/func_ctrl.c', False),
        ('studio-func', 'source/source/app/func_ctrl.c', True),
        ('arduino-motor', 'arduino/motordecoder2/func_ctrl.c', False),
        ('arduino-func', 'arduino/funcdecoder2/func_ctrl.c', True),
    ]:
        binary = tmp / name
        subprocess.run([
            os.environ.get('CC', 'cc'), '-std=c99', '-O2',
            '-fsanitize=address,undefined', '-I' + str(tmp),
            '-DFUNC_SOURCE="' + str(ROOT / source) + '"',
        ] + (['-DATTINY806_FUNC'] if function_only else []) + [
            '-DEFFECT_SOURCE="' + str((ROOT / source).with_name('func_effect.c')) + '"', str(harness), '-o', str(binary),
        ], check=True)
        subprocess.run([str(binary)], check=True)
        print(name + ': passed', flush=True)
