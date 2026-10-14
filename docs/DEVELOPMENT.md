# Development guide

## Prerequisites
- CMake 3.20+
- C++23 compiler (recent GCC, Clang, or MSVC)

## Configure, build and test
```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
ctest --test-dir build --output-on-failure
./build/z80-runner
```

On Windows, a multi-configuration generator normally places the executable under `build\Debug\z80-runner.exe`.

## Implemented instruction subset
NOP, LD r,n for A/B/C/D/E/H/L, INC B, DEC B, ADD A,B, JP nn, HALT. Unsupported instructions throw an exception intentionally. This is a learning scaffold, not a complete Z80 implementation.
