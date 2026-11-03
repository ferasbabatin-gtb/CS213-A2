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
};
void Cpu::opAnd(int r, int s, int t){
    reg.write(r, reg.read(s) & reg.read(t));
};
void Cpu::opXor(int r, int s, int t){
    reg.write(r, reg.read(s) ^ reg.read(t));
};
void Cpu::opRotate(int r, int x){
    x = x % 8;
    uint8_t shift = ((1<<x)-1);

    uint8_t ans = ((reg.read(r) & shift) << (8-x)) + ((reg.read(r) & (0xFF-shift)) >> x);
    
    reg.write(r,ans);
};

        