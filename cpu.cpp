#include <iostream>
using namespace std;
class Cpu {
    private:
        CpuRegisters reg;
        Memory mem;
        int pc;
        bool halted;
        uint8_t lastOutput;
        void opLoadMem(int r, int xy){
            reg.write(r, mem.read(xy));
        };
        void opLoadImm(int r, int xy){
            reg.write(r, xy);
        };
        ExecStatus opStore(int r, int xy){
            if(xy==0xFF){
                lastOutput = reg.read(r);
                return ExecStatus::SCREEN_OUTPUT;
            }
            mem.write(xy, reg.read(r));
            return ExecStatus::OK;
        };
        void opMove(int r, int s){
            reg.write(s, reg.read(r));
        };
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
        Instruction getIR();
        int getPC();
        uint8_t getLastOutput();
        bool isHalted();
        uint8_t getRegister(int idx);
        bool setRegister(int idx, int value);

}
int main() {
    
    return 0;
} 