; Illustrative only: no bundled assembler yet.
        LD B, 3
loop:   DEC B
        JP NZ, loop
        HALT
