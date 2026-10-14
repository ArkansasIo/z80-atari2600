#include "z80/cpu.hpp"
#include <algorithm>
#include <stdexcept>
namespace z80 {
namespace {
constexpr std::uint8_t S=0x80,Z=0x40,Y=0x20,H=0x10,X=0x08,PV=0x04,N=0x02,C=0x01;
void szxy(std::uint8_t& f,std::uint8_t v){f=static_cast<std::uint8_t>(f&static_cast<std::uint8_t>(~(S|Z|Y|X)));f=static_cast<std::uint8_t>(f|(v&(S|Y|X)));if(v==0)f|=Z;}
bool parity(std::uint8_t v){unsigned n=0;for(unsigned i=0;i<8;++i)n+=(v>>i)&1U;return n%2U==0U;}
}
void Cpu::reset(){r=Registers{};halted=false;memory.fill(0);}
void Cpu::load(const std::vector<std::uint8_t>& p,std::uint16_t a){if(p.size()>memory.size()-static_cast<std::size_t>(a))throw std::out_of_range("program does not fit in address space");std::copy(p.begin(),p.end(),memory.begin()+a);r.pc=a;halted=false;}
std::uint8_t Cpu::fetch8(){return memory[r.pc++];}
std::uint16_t Cpu::fetch16(){const auto lo=fetch8(),hi=fetch8();return static_cast<std::uint16_t>(lo|(static_cast<std::uint16_t>(hi)<<8U));}
void Cpu::set_zero_sign(std::uint8_t v){szxy(r.f,v);}
StepResult Cpu::step(){
 if(halted)return {4,true};const auto op=fetch8();
 auto get=[this](unsigned n)->std::uint8_t{switch(n){case 0:return r.b;case 1:return r.c;case 2:return r.d;case 3:return r.e;case 4:return r.h;case 5:return r.l;case 6:return memory[static_cast<std::uint16_t>((r.h<<8U)|r.l)];default:return r.a;}};
 auto put=[this](unsigned n,std::uint8_t v){switch(n){case 0:r.b=v;break;case 1:r.c=v;break;case 2:r.d=v;break;case 3:r.e=v;break;case 4:r.h=v;break;case 5:r.l=v;break;case 6:memory[static_cast<std::uint16_t>((r.h<<8U)|r.l)]=v;break;default:r.a=v;break;}};
 auto add=[this](std::uint8_t v){const auto old=r.a;const unsigned wide=static_cast<unsigned>(old)+v;const auto out=static_cast<std::uint8_t>(wide);r.f=0;szxy(r.f,out);if(((old^v^out)&0x10U)!=0)r.f|=H;if(((~(old^v)&(old^out))&0x80U)!=0)r.f|=PV;if(wide>255U)r.f|=C;r.a=out;};
 auto sub=[this](std::uint8_t v,bool cp){const auto old=r.a;const auto out=static_cast<std::uint8_t>(old-v);r.f=N;szxy(r.f,out);if(((old^v^out)&0x10U)!=0)r.f|=H;if((((old^v)&(old^out))&0x80U)!=0)r.f|=PV;if(old<v)r.f|=C;if(!cp)r.a=out;};
 if(op>=0x40&&op<=0x7F){if(op==0x76){halted=true;return {4,true};}const unsigned d=(op>>3U)&7U,s=op&7U;put(d,get(s));return {(d==6||s==6)?7U:4U,false};}
 if(op>=0x80&&op<=0x87){const unsigned s=op&7U;add(get(s));return {s==6?7U:4U,false};}
 if(op>=0x90&&op<=0x97){const unsigned s=op&7U;sub(get(s),false);return {s==6?7U:4U,false};}
 if(op>=0xA0&&op<=0xBF){const unsigned g=(op>>3U)&3U,s=op&7U;const auto v=get(s);if(g==0){r.a=static_cast<std::uint8_t>(r.a&v);r.f=H;szxy(r.f,r.a);if(parity(r.a))r.f|=PV;}else if(g==1){r.a=static_cast<std::uint8_t>(r.a^v);r.f=0;szxy(r.f,r.a);if(parity(r.a))r.f|=PV;}else if(g==2){r.a=static_cast<std::uint8_t>(r.a|v);r.f=0;szxy(r.f,r.a);if(parity(r.a))r.f|=PV;}else sub(v,true);return {s==6?7U:4U,false};}
 if((op&0xC7U)==0x04U||(op&0xC7U)==0x05U){const unsigned n=(op>>3U)&7U;const bool dec=(op&1U)!=0;const auto old=get(n);const auto out=static_cast<std::uint8_t>(dec?old-1U:old+1U);const auto carry=static_cast<std::uint8_t>(r.f&C);r.f=carry;szxy(r.f,out);if(dec){r.f|=N;if((old&15U)==0)r.f|=H;if(old==0x80)r.f|=PV;}else{if((old&15U)==15U)r.f|=H;if(old==0x7F)r.f|=PV;}put(n,out);return {n==6?11U:4U,false};}
 switch(op){
 case 0x00:return {4,false};
 case 0x3E:r.a=fetch8();return {7,false};case 0x06:r.b=fetch8();return {7,false};case 0x0E:r.c=fetch8();return {7,false};case 0x16:r.d=fetch8();return {7,false};case 0x1E:r.e=fetch8();return {7,false};case 0x26:r.h=fetch8();return {7,false};case 0x2E:r.l=fetch8();return {7,false};
 case 0x36:memory[static_cast<std::uint16_t>((r.h<<8U)|r.l)]=fetch8();return {10,false};
 case 0xC6:add(fetch8());return {7,false};case 0xD6:sub(fetch8(),false);return {7,false};
 case 0xE6:r.a=static_cast<std::uint8_t>(r.a&fetch8());r.f=H;szxy(r.f,r.a);if(parity(r.a))r.f|=PV;return {7,false};
 case 0xEE:r.a=static_cast<std::uint8_t>(r.a^fetch8());r.f=0;szxy(r.f,r.a);if(parity(r.a))r.f|=PV;return {7,false};
 case 0xF6:r.a=static_cast<std::uint8_t>(r.a|fetch8());r.f=0;szxy(r.f,r.a);if(parity(r.a))r.f|=PV;return {7,false};
 case 0xFE:sub(fetch8(),true);return {7,false};
 case 0xC3:r.pc=fetch16();return {10,false};
 case 0xC2:{const auto a=fetch16();if((r.f&Z)==0)r.pc=a;return {10,false};}
 case 0xCA:{const auto a=fetch16();if((r.f&Z)!=0)r.pc=a;return {10,false};}
 default:{constexpr char d[]="0123456789ABCDEF";std::string m="unsupported Z80 opcode 0x";m+=d[op>>4U];m+=d[op&15U];throw std::runtime_error(m);}
 }
}
void Cpu::run(std::uint64_t limit){for(std::uint64_t i=0;i<limit&&!halted;++i)(void)step();if(!halted)throw std::runtime_error("instruction limit reached before HALT");}
}
