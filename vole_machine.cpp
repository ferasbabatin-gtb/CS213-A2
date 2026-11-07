#ifndef VOLEMACHINE_HPP
#define VOLEMACHINE_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "Constants.h"
#include "Memory.h"
#include "Cpu.h"
#include "Instruction.h"

using namespace std;

// possible results when loading a file
enum class LoadStatus {
    LOAD_OK,
    FILE_NOT_FOUND,
    ODD_ADDRESS,
    ADDRESS_OUT_OF_RANGE,
    PROGRAM_TOO_LARGE,
    INVALID_INSTRUCTION_IN_FILE
};

// possible results after running one instruction
enum class StepResult {
    STEP_OK,
    STEP_HALTED,
    STEP_INVALID_INSTRUCTION,
    STEP_PC_OUT_OF_RANGE,
    STEP_NOT_LOADED,
    STEP_LIMIT_REACHED
};

// different ways we can display the screen output
enum class ScreenFormat {
    ASCII,
    HEX,
    DECIMAL
};
