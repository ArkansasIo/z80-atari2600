# Architecture and scope

## Two distinct targets
The Atari 2600 uses a MOS 6507, a 6502-family CPU with a reduced address bus, plus the TIA video/audio chip and 6532 RIOT RAM/I/O/timer. It is not a Z80 system. This repository keeps the Z80 CPU core separate from Atari hardware notes.

## Current implementation
- C++23 Z80 interpreter scaffold with a deliberately limited opcode subset.
- Flat 64 KiB Z80 address space.
- Minimal runner and assertion-based smoke tests.
- Atari 2600 documentation only; no claim of functional or cycle-accurate Atari emulation.

## Roadmap
1. Expand Z80 instruction coverage and validate flags/timings.
2. Add exhaustive opcode tests, including CB/ED/DD/FD prefixes.
3. Implement an assembler with labels, expressions, diagnostics and binary output.
4. Add a separate 6507-compatible CPU before connecting an Atari bus.
5. Implement RIOT RAM/I/O/timer and then TIA register and scanline behavior.
6. Add cartridge mapper interfaces, controller input, audio/video frontend and debugger.
7. Add CI and platform packaging.

## Principles
Keep Z80 independent of Atari hardware. Model bus transactions and cycles explicitly when cycle accuracy is added. Use public-domain or permissively licensed test assets only; do not commit commercial ROMs.
