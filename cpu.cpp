#include <iostream>
#include "Cpu.h"
using namespace std;


void Cpu::opLoadMem(int r, int xy){
            reg.write(r, mem.read(xy));
        }
void Cpu::opLoadImm(int r, int xy){
            reg.write(r, xy);
        }
ExecStatus Cpu::opStore(int r, int xy){
            if(xy==0xFF){
                lastOutput = reg.read(r);
                return ExecStatus::SCREEN_OUTPUT;
            }
            mem.write(xy, reg.read(r));
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
Instruction Cpu::getIR(){
    return ir;
}
int Cpu::getPC(){
        return pc;
        }
uint8_t Cpu::getLastOutput(){
            return lastOutput;
        }
bool Cpu::isHalted(){
            return halted;
        }
uint8_t Cpu::getRegister(int idx){
            return reg.read(idx);
        }
bool Cpu::setRegister(int idx, int value){
            if(!reg.isValidIndex(idx)){
                return false;
            }
            reg.write(idx, value);
            return true;
        }