#include "z80/cpu.hpp"
#include <iomanip>
#include <iostream>
int main(){
 z80::Cpu cpu; cpu.load({0x3E,0x02,0x06,0x03,0x80,0x76});
 try{cpu.run();}catch(const std::exception& e){std::cerr<<"Execution error: "<<e.what()<<'\n';return 1;}
 std::cout<<"Z80 toolkit smoke program\nA=0x"<<std::hex<<std::uppercase<<std::setw(2)<<std::setfill('0')<<static_cast<unsigned>(cpu.r.a)
 <<" ("<<std::dec<<static_cast<unsigned>(cpu.r.a)<<")\nPC=0x"<<std::hex<<cpu.r.pc<<"\nHALT="<<std::boolalpha<<cpu.halted<<'\n';
 return cpu.r.a==5&&cpu.halted?0:2;
}
