#include "z80/cpu.hpp"
#include <algorithm>
#include <stdexcept>
namespace z80 {
namespace {
constexpr std::uint8_t flag_s=0x80, flag_z=0x40, flag_pv=0x04, flag_c=0x01;
bool even_parity(std::uint8_t v) { unsigned n=0; for(unsigned i=0;i<8;++i) n+=(v>>i)&1U; return n%2U==0U; }
}
void Cpu::reset(){ r=Registers{}; halted=false; memory.fill(0); }
void Cpu::load(const std::vector<std::uint8_t>& p,std::uint16_t a){
 if(p.size()>memory.size()-static_cast<std::size_t>(a)) throw std::out_of_range("program does not fit in address space");
 std::copy(p.begin(),p.end(),memory.begin()+a); r.pc=a; halted=false;
}
std::uint8_t Cpu::fetch8(){ return memory[r.pc++]; }
std::uint16_t Cpu::fetch16(){ const auto lo=fetch8(); const auto hi=fetch8(); return static_cast<std::uint16_t>(lo|(static_cast<std::uint16_t>(hi)<<8U)); }
void Cpu::set_zero_sign(std::uint8_t v){
 r.f=static_cast<std::uint8_t>(r.f&static_cast<std::uint8_t>(~(flag_s|flag_z|flag_pv)));
 if(v==0) r.f|=flag_z; if((v&0x80U)!=0) r.f|=flag_s; if(even_parity(v)) r.f|=flag_pv;
}
StepResult Cpu::step(){
 if(halted) return {4,true};
 const auto op=fetch8();
 switch(op){
 case 0x00:return {4,false};
 case 0x3E:r.a=fetch8();return {7,false};
 case 0x06:r.b=fetch8();return {7,false};
 case 0x0E:r.c=fetch8();return {7,false};
 case 0x16:r.d=fetch8();return {7,false};
 case 0x1E:r.e=fetch8();return {7,false};
 case 0x26:r.h=fetch8();return {7,false};
 case 0x2E:r.l=fetch8();return {7,false};
 case 0x04:++r.b;set_zero_sign(r.b);return {4,false};
 case 0x05:--r.b;set_zero_sign(r.b);return {4,false};
 case 0x80:{
  const auto old=r.a; const unsigned sum=static_cast<unsigned>(old)+r.b; const auto result=static_cast<std::uint8_t>(sum);
  r.f=0; set_zero_sign(result); if(sum>0xFFU)r.f|=flag_c;
  if(((old^r.b^result)&0x10U)!=0)r.f|=0x10;
  if(((~(old^r.b)&(old^result))&0x80U)!=0)r.f|=flag_pv;
  r.a=result;return {4,false};
 }
 case 0xC3:r.pc=fetch16();return {10,false};
 case 0x76:halted=true;return {4,true};
 default:{
  constexpr char digits[]="0123456789ABCDEF";
  std::string msg="unsupported Z80 opcode 0x";msg+=digits[op>>4U];msg+=digits[op&0x0FU];
  throw std::runtime_error(msg);
 }
 }
}
void Cpu::run(std::uint64_t limit){ for(std::uint64_t i=0;i<limit&&!halted;++i)(void)step(); if(!halted)throw std::runtime_error("instruction limit reached before HALT"); }
}
