#include <iostream>
#include "Cpu.h"
using namespace std;
Cpu::Cpu(Memory& m) : mem(m), pc(0), halted(false), lastOutput(0), ir(0) {}
float Cpu::byteToFloat(uint8_t b){
int s = (b >> 7) == 0 ? 1 : -1;
int expo = ((b >> 4) & (0b111)) - (1<<2);
int mant = b & 0b1111;
return s*(pow(2,expo))*(mant*0.0625);

}
uint8_t Cpu::floatToByte(float f){
    if(f==0){
        return 0;
    }
    int expo = floor(log2(abs(f))) + 5;
    int sign = f>0?  0:1;
    int mant = abs((f / (pow(2,expo-4))) * 16);
    return ((expo & 0b111) << 4) + ((mant & 0b1111)) + (sign << 7);
}
void Cpu::opLoadMem(int r, int xy){
            reg.write(r, mem.read(xy));
        }
void Cpu::opLoadImm(int r, int xy){
            reg.write(r, xy);
        }

void Cpu::opAdd(int r, int s, int t){
    reg.write(r,reg.read(s)+reg.read(t));
}
void Cpu::opAddFloat(int r, int s, int t){
    float a = byteToFloat(reg.read(s));

    float b = byteToFloat(reg.read(t));
    reg.write(r,floatToByte(a+b));
}
ExecStatus Cpu::opStore(int r, int xy){
    mem.write(xy, reg.read(r));
            if(xy==0xFF){
                lastOutput = reg.read(r);
                
                return ExecStatus::SCREEN_OUTPUT;
            }
           
            return ExecStatus::OK;
        }
void Cpu::opMove(int r, int s){
            reg.write(s, reg.read(r));
        }

void Cpu::opOr(int r, int s, int t){
    reg.write(r, reg.read(s) | reg.read(t));
}
void Cpu::opAnd(int r, int s, int t){
    reg.write(r, reg.read(s) & reg.read(t));
}
void Cpu::opXor(int r, int s, int t){
    reg.write(r, reg.read(s) ^ reg.read(t));
}
void Cpu::opRotate(int r, int x){
    x = x % 8;
    uint8_t shift = ((1<<x)-1);

    uint8_t ans = ((reg.read(r) & shift) << (8-x)) + ((reg.read(r) & (0xFF-shift)) >> x);
    
    reg.write(r,ans);
}
void Cpu::opJump(int r, int xy){
    if(reg.read(r) == reg.read(0)){
        pc=xy;
    };
};
void Cpu::opHalt(){
    this->halted = true;
};
void Cpu::reset(int startPc){
    pc = startPc;
    halted = false;
}
bool Cpu::fetch(){
    if(pc >= 0xFF){
        return false;
    }
    ir = Instruction(mem.read(pc)*16*16 + mem.read(pc+1));
    pc+=2;
    return true;
}
ExecStatus Cpu::decodeExecute(){
    if(ir.isValid() == false){
        return ExecStatus::INVALID_INSTRUCTION;
    }
    switch(ir.opcode()){
        case 1:
            opLoadMem(ir.r(), ir.xy());
            return ExecStatus::OK;
        case 2:
            opLoadImm(ir.r(), ir.xy());
            return ExecStatus::OK;
        case 3:
            return opStore(ir.r(), ir.xy());
        case 4:
            opMove(ir.s(), ir.t());
            return ExecStatus::OK;
        case 5:
            opAdd(ir.r(), ir.s(), ir.t());
            return ExecStatus::OK;
        case 6:
            opAddFloat(ir.r(), ir.s(), ir.t());
            return ExecStatus::OK;
        case 7:
            opOr(ir.r(), ir.s(), ir.t());
            return ExecStatus::OK;
        case 8:
            opAnd(ir.r(), ir.s(), ir.t());
            return ExecStatus::OK;
        case 9:
            opXor(ir.r(), ir.s(), ir.t());
            return ExecStatus::OK;
        case 10:
            opRotate(ir.r(), ir.x());
            return ExecStatus::OK;
        case 11:
            opJump(ir.r(), ir.xy());
            return ExecStatus::OK;
        case 12:
            opHalt();
            return ExecStatus::HALTED;
        default:
            return ExecStatus::INVALID_INSTRUCTION;
    }
}
Instruction Cpu::getIR() const {
    return ir;
}
int Cpu::getPC() const{
        return pc;
        }
uint8_t Cpu::getLastOutput(){
            return lastOutput;
        }
bool Cpu::isHalted(){
            return halted;
        }
uint8_t Cpu::getRegister(int idx) const{
            return reg.read(idx);
        }
bool Cpu::setRegister(int idx, int value) {
            if(!reg.isValidIndex(idx)){
                return false;
            }
            reg.write(idx, value);
            return true;
        }