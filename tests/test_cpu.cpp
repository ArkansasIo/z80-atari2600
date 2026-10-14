#include "z80/cpu.hpp"
#include <cassert>
#include <stdexcept>
int main(){
 {z80::Cpu c;c.load({0x3E,0x02,0x06,0x03,0x80,0x76});c.run();assert(c.r.a==5);assert(c.halted);}
 {z80::Cpu c;c.load({0x06,0xFF,0x04,0x76});c.run();assert(c.r.b==0);assert((c.r.f&0x40U)!=0);}
 {z80::Cpu c;bool threw=false;try{c.load({0x00},0xFFFF);}catch(const std::out_of_range&){threw=true;}assert(threw);}
 {z80::Cpu c;c.load({0xFF});bool threw=false;try{(void)c.step();}catch(const std::runtime_error&){threw=true;}assert(threw);}
}
