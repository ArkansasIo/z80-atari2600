# Z80 / Atari 2600 Toolkit

A C++23 experimental Z80 CPU core and development toolkit, with a separate Atari 2600 hardware reference layer. The original Atari 2600 uses the MOS 6507 (6502-family), not a Z80; see `docs/ARCHITECTURE.md`.

## Build

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
```

See `docs/` for architecture and scope. This is an educational foundation, not yet a cycle-accurate Atari 2600 emulator.