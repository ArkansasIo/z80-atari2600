# Atari 2600 hardware notes

The Atari 2600's core components are the MOS 6507 CPU, Television Interface Adapter (TIA), and 6532 RIOT (128 bytes RAM, I/O and timer). The console does not use a Z80 CPU. The 6507 exposes a reduced address bus; cartridge bank switching extends program space beyond a simple 4 KiB image.

## Emulator implications
- CPU: 6507-compatible 6502 instruction behavior with console-specific bus constraints.
- TIA: memory-mapped video/audio registers and beam-synchronous rendering; no conventional framebuffer RAM.
- RIOT: 128 bytes RAM, controller/switch I/O, interval timer.
- Cartridge: mapper strategy depends on cartridge type and bank-switching hardware.
- Timing: CPU, TIA color clocks, scanlines, and region-specific video standards must remain synchronized.

## References
- iFixit teardown: https://www.ifixit.com/Teardown/Atari%2B2600%2BTeardown/3541
- Libretro TIA core notes: https://docs.libretro.com/library/tia/
- Hardware overview: https://en.wikipedia.org/wiki/Atari_2600_hardware
