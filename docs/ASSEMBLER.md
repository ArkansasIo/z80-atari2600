# Z80 assembler

`tools/z80asm.py` is a small, dependency-free, two-pass assembler for the documented toolkit subset. It supports labels, decimal/hex byte operands, register loads, INC/DEC, selected ALU operations, NOP, HALT and JP/JP NZ/JP Z.

Example:

```sh
python tools/z80asm.py examples/loop.asm -o out/loop.bin
```

The assembler is intentionally not a complete Zilog assembler. It currently emits a flat binary from the chosen origin; it does not emit Intel HEX, support macros, directives, expressions, relocations, or prefixed instruction families.
