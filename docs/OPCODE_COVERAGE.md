# Z80 opcode coverage

This is a documented subset, not complete Z80 compatibility.

- Control: NOP, HALT, JP nn, JP NZ,nn, JP Z,nn.
- Loads: LD A/B/C/D/E/H/L,n; LD r,r' for B/C/D/E/H/L/(HL)/A; LD (HL),n.
- Increment/decrement: INC r and DEC r for B/C/D/E/H/L/(HL)/A.
- Arithmetic: ADD A,r/n; SUB r/n; CP r/n.
- Logic: AND A,r/n; XOR A,r/n; OR A,r/n.

CB/ED/DD/FD prefixes, interrupts, alternate registers, 16-bit arithmetic, I/O, refresh behavior and cycle-accurate bus timing are not implemented. Flags require validation against a trusted Z80 conformance suite before claiming broad compatibility.
