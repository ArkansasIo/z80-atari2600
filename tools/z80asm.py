#!/usr/bin/env python3
"""Small two-pass assembler for the documented Z80 toolkit subset."""
from __future__ import annotations
import argparse
import re
import sys
from pathlib import Path

REGS = {"B": 0, "C": 1, "D": 2, "E": 3, "H": 4, "L": 5, "(HL)": 6, "A": 7}
IMM8 = {"A": 0x3E, "B": 0x06, "C": 0x0E, "D": 0x16, "E": 0x1E, "H": 0x26, "L": 0x2E}
ALU_REG = {"ADD": 0x80, "SUB": 0x90, "AND": 0xA0, "XOR": 0xA8, "OR": 0xB0, "CP": 0xB8}
ALU_IMM = {"ADD": 0xC6, "SUB": 0xD6, "AND": 0xE6, "XOR": 0xEE, "OR": 0xF6, "CP": 0xFE}

def number(token: str, labels: dict[str, int] | None = None) -> int:
    token = token.strip()
    if labels and token.upper() in labels:
        return labels[token.upper()]
    try:
        if token.startswith("$"): return int(token[1:], 16)
        if token.lower().startswith("0x"): return int(token, 16)
        if token.lower().endswith("h"): return int(token[:-1], 16)
        return int(token, 10)
    except ValueError as exc:
        raise ValueError(f"unknown number or label: {token}") from exc

def parse_line(raw: str) -> tuple[str | None, str, list[str]]:
    line = raw.split(";", 1)[0].strip()
    if not line: return None, "", []
    label = None
    if ":" in line:
        maybe, rest = line.split(":", 1)
        if not re.fullmatch(r"[A-Za-z_][A-Za-z0-9_]*", maybe.strip()):
            raise ValueError(f"invalid label: {maybe.strip()}")
        label, line = maybe.strip().upper(), rest.strip()
    if not line: return label, "", []
    pieces = line.split(None, 1)
    mnemonic = pieces[0].upper()
    operands = [x.strip().upper() for x in pieces[1].split(",")] if len(pieces) > 1 else []
    return label, mnemonic, operands

def instruction_size(mnemonic: str, operands: list[str]) -> int:
    if mnemonic in ("NOP", "HALT"): return 1
    if mnemonic in ("INC", "DEC") and len(operands) == 1 and operands[0] in REGS: return 1
    if mnemonic == "LD" and len(operands) == 2:
        dst, src = operands
        if dst in IMM8 and src != "(HL)": return 2
        if dst == "(HL)" and src != "A": return 2
        if dst in REGS and src in REGS: return 1
    if mnemonic in ALU_REG and len(operands) in (1, 2):
        if len(operands) == 1 and operands[0] in REGS: return 1
        if len(operands) == 1 and mnemonic != "ADD": return 2
        if len(operands) == 2 and operands[0] == "A" and operands[1] in REGS: return 1
        if len(operands) == 2 and operands[0] == "A": return 2
    if mnemonic == "JP" and len(operands) in (1, 2):
        if len(operands) == 1 or (len(operands) == 2 and operands[0] in ("NZ", "Z")): return 3
    raise ValueError(f"unsupported instruction or operand form: {mnemonic} {' '.join(operands)}".strip())

def encode(mnemonic: str, args: list[str], labels: dict[str, int]) -> list[int]:
    if mnemonic == "NOP": return [0x00]
    if mnemonic == "HALT": return [0x76]
    if mnemonic in ("INC", "DEC"):
        reg = REGS[args[0]]
        return [(0x04 if mnemonic == "INC" else 0x05) + reg * 8]
    if mnemonic == "LD":
        dst, src = args
        if dst in IMM8 and src != "(HL)":
            value = number(src, labels)
            if not 0 <= value <= 255: raise ValueError("8-bit immediate out of range")
            return [IMM8[dst], value]
        if dst == "(HL)":
            value = number(src, labels)
            if not 0 <= value <= 255: raise ValueError("8-bit immediate out of range")
            return [0x36, value]
        return [0x40 + REGS[dst] * 8 + REGS[src]]
    if mnemonic in ALU_REG:
        if len(args) == 1 and args[0] in REGS: return [ALU_REG[mnemonic] + REGS[args[0]]]
        if len(args) == 2 and args[0] == "A" and args[1] in REGS: return [ALU_REG[mnemonic] + REGS[args[1]]]
        immediate = args[0] if len(args) == 1 else args[1]
        value = number(immediate, labels)
        if not 0 <= value <= 255: raise ValueError("8-bit immediate out of range")
        return [ALU_IMM[mnemonic], value]
    if mnemonic == "JP":
        if len(args) == 1: opcode, target = 0xC3, args[0]
        elif args[0] == "NZ": opcode, target = 0xC2, args[1]
        else: opcode, target = 0xCA, args[1]
        value = number(target, labels)
        if not 0 <= value <= 65535: raise ValueError("jump target out of range")
        return [opcode, value & 255, value >> 8]
    raise ValueError(f"unsupported instruction: {mnemonic}")

def assemble(source: str, origin: int = 0) -> bytes:
    if not 0 <= origin <= 65535: raise ValueError("origin must be in 0..65535")
    parsed = [parse_line(line) for line in source.splitlines()]
    labels: dict[str, int] = {}
    pc = origin
    for label, mnemonic, args in parsed:
        if label:
            if label in labels: raise ValueError(f"duplicate label: {label}")
            labels[label] = pc
        if mnemonic:
            pc += instruction_size(mnemonic, args)
            if pc > 65536: raise ValueError("program exceeds 64 KiB address space")
    output: list[int] = []
    for _, mnemonic, args in parsed:
        if mnemonic: output.extend(encode(mnemonic, args, labels))
    if origin + len(output) > 65536: raise ValueError("program exceeds 64 KiB address space")
    return bytes(output)

def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("input", type=Path)
    parser.add_argument("-o", "--output", type=Path, required=True)
    parser.add_argument("--origin", type=lambda s: number(s), default=0, help="decimal, 0x-prefixed or $-prefixed address")
    args = parser.parse_args()
    try:
        binary = assemble(args.input.read_text(encoding="utf-8"), args.origin)
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_bytes(binary)
        print(f"Wrote {len(binary)} bytes to {args.output}")
        return 0
    except (OSError, ValueError) as exc:
        print(f"z80asm: error: {exc}", file=sys.stderr)
        return 2

if __name__ == "__main__":
    raise SystemExit(main())
