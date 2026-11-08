#ifndef CPU_H
#define CPU_H

#include <iostream>
#include "ByteStorage.h"
#include "Constants.h"
#include "Instruction.h"
#include <cmath>
using namespace std;

class Cpu {
    private:
        CpuRegisters reg;
        Memory mem;
        int pc;
        bool halted;
        uint8_t lastOutput;
        Instruction ir;
        void opLoadMem(int r, int xy);
        void opLoadImm(int r, int xy);
        ExecStatus opStore(int r, int xy);
        void opMove(int r, int s);
        void opAdd(int r, int s, int t);
        void opAddFloat(int r, int s, int t);
        void opOr(int r, int s, int t);
        void opAnd(int r, int s, int t);
        void opXor(int r, int s, int t);
        void opRotate(int r, int x);
        void opJump(int r, int xy);
        void opHalt();
        float byteToFloat(uint8_t b);
        uint8_t floatToByte(float f);
        
    public:
        Cpu(Memory& mem);
        void reset(int startPc);
        bool fetch();
        ExecStatus decodeExecute();
        Instruction getIR() const;
        int getPC() const;
        uint8_t getLastOutput();
        bool isHalted();
        uint8_t getRegister(int idx) const;
        bool setRegister(int idx, int value);

};

#endif