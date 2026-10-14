# Z80 / Atari 2600 Toolkit

A C++23 experimental Z80 CPU core and development toolkit, with a separate Atari 2600 hardware reference layer. The original Atari 2600 uses the MOS 6507 (6502-family), not a Z80; see `docs/ARCHITECTURE.md`.

## Build

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
```

See `docs/` for architecture and scope. This is an educational foundation, not yet a cycle-accurate Atari 2600 emulator.

## Current status

The initial CPU scaffold is intentionally incomplete. Implemented instructions: NOP, LD r,n (A/B/C/D/E/H/L), INC B, DEC B, ADD A,B, JP nn, and HALT. Unsupported opcodes fail loudly. The GitHub Actions workflow builds with CMake and runs the smoke tests.

## Assemble a sample

Python 3 is required for the optional assembler and its tests. The assembler supports a documented subset of Z80 instructions and emits a flat binary:

```sh
python tools/z80asm.py examples/loop.asm -o out/loop.bin
```

See [assembler documentation](docs/ASSEMBLER.md), [opcode coverage](docs/OPCODE_COVERAGE.md), and the [roadmap](docs/ROADMAP.md). This remains an educational subset, not a complete or cycle-accurate Z80/Atari 2600 emulator.
