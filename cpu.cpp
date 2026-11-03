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
        