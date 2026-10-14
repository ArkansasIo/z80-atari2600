import importlib.util
import unittest
from pathlib import Path

MODULE = Path(__file__).resolve().parents[1] / "tools" / "z80asm.py"
spec = importlib.util.spec_from_file_location("z80asm", MODULE)
z80asm = importlib.util.module_from_spec(spec)
assert spec and spec.loader
spec.loader.exec_module(z80asm)

class AssemblerTests(unittest.TestCase):
    def test_basic_program(self):
        self.assertEqual(z80asm.assemble("LD A, 2\nLD B, 3\nADD A, B\nHALT"), bytes([0x3E,2,0x06,3,0x80,0x76]))
    def test_label_and_conditional_jump(self):
        src = "LD B, 2\nloop: DEC B\nJP NZ, loop\nHALT"
        self.assertEqual(z80asm.assemble(src), bytes([0x06,2,0x05,0xC2,0x02,0x00,0x76]))
    def test_register_load_and_memory(self):
        self.assertEqual(z80asm.assemble("LD H, 32\nLD L, 0\nLD (HL), 66\nLD A, (HL)\nHALT"), bytes([0x26,32,0x2E,0,0x36,66,0x7E,0x76]))
    def test_immediate_range(self):
        with self.assertRaises(ValueError): z80asm.assemble("LD A, 300")
    def test_undefined_label(self):
        with self.assertRaises(ValueError): z80asm.assemble("JP nowhere")
    def test_duplicate_label(self):
        with self.assertRaises(ValueError): z80asm.assemble("x: NOP\nx: HALT")

if __name__ == "__main__":
    unittest.main()
