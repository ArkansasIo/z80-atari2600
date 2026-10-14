# Project roadmap

## Educational Z80 core
- [x] CMake project, CPU runner and CI workflow
- [x] Basic register loads, ALU subset and conditional jumps
- [ ] Verify CI on the latest commit
- [ ] Add reference vectors and opcode conformance tests

## Development tools
- [ ] Assembler with labels, expressions and diagnostics
- [ ] Disassembler and debugger
- [ ] Binary/Intel HEX loading
- [ ] Sanitizers and multi-platform CI

## Atari 2600 emulator (separate CPU architecture)
- [ ] MOS 6507-compatible CPU module
- [ ] RIOT RAM, I/O and timer
- [ ] TIA register model and scanline/color-clock timing
- [ ] Cartridge mapper, controllers and test ROM harness
- [ ] Audio/video frontend and packaging
