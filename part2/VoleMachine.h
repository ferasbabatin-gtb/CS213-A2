#ifndef VOLEMACHINE_H
#define VOLEMACHINE_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdint>

#include "Constants.h"
#include "ByteStorage.h"
#include "Cpu.h"
#include "Instruction.h"
#include "Screen.h"

using namespace std;

class VoleMachine {
private:
    Memory mem;          
    Cpu cpu;             
    Screen screen;       
    bool programLoaded;  

    bool isSkippableLine(const string& line) const;
    LoadStatus readProgramFile(const string& file, vector<Instruction>& out, int& badLine) const;
    
public:
    VoleMachine();
    LoadStatus loadProgram(const string& file, int startAddr, int& badLine);
    StepResult step();
    StepResult runAll(int maxSteps);
    bool       setMemoryCell(int addr, int value);
    uint8_t    getMemoryCell(int addr) const;
    bool       setRegister(int idx, int value);
    uint8_t    getRegister(int idx) const;
    int        getPC() const;
    string     getIRString() const;
    string     getScreenString() const;
    void       setScreenFormat(ScreenFormat f);
    bool       isProgramLoaded() const;
};

#endif
