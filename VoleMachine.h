#ifndef VOLEMACHINE_H
#define VOLEMACHINE_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdint>

#include "Constants.h"
#include "Memory.h"
#include "Cpu.h"
#include "Instruction.h"

using namespace std;

enum class LoadStatus {
    LOAD_OK,
    FILE_NOT_FOUND,
    ODD_ADDRESS,
    ADDRESS_OUT_OF_RANGE,
    PROGRAM_TOO_LARGE,
    INVALID_INSTRUCTION_IN_FILE
};

enum class StepResult {
    STEP_OK,
    STEP_HALTED,
    STEP_INVALID_INSTRUCTION,
    STEP_PC_OUT_OF_RANGE,
    STEP_NOT_LOADED,
    STEP_LIMIT_REACHED
};

enum class ScreenFormat {
    ASCII,
    HEX,
    DECIMAL
};


// =============== SCREEN ===============

class Screen {
private:
    vector<uint8_t> bytes; 
    ScreenFormat format;    

public:
    Screen(); 
    void append(uint8_t value);
    void clear();
    void setFormat(ScreenFormat f);
    string toString() const;
};


// =============== VOLE MACHINE ===============

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
