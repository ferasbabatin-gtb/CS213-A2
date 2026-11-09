#include "VoleMachine.h"

VoleMachine::VoleMachine() : mem(), cpu(mem) {
    programLoaded = false;
}

bool VoleMachine::isSkippableLine(const string& line) const {
    int firstChar = -1;

    for (int i = 0; i < line.length(); i++) {
        if (line[i] != ' ' && line[i] != '\t' && line[i] != '\r' && line[i] != '\n') {
            firstChar = i;
            break;
        }
    }

    if (firstChar == -1) return true;
    if (line[firstChar] == ';') return true;
    return false;
}

LoadStatus VoleMachine::readProgramFile(const string& file, vector<Instruction>& out, int& badLine) const {
    badLine = 0;

    ifstream input(file);
    if (!input) {
        return LoadStatus::FILE_NOT_FOUND;
    }

    out.clear();
    string line;
    int lineNumber = 0;

    while (getline(input, line)) {
        lineNumber++;
        if(line.substr(0,3)=="\xEF\xBB\xBF") line = line.substr(3,line.size());

        if (isSkippableLine(line)) {
            continue;
        }

        Instruction instr;
        if (!Instruction::parse(line, instr)) {
            badLine = lineNumber;
            return LoadStatus::INVALID_INSTRUCTION_IN_FILE;
        }

        out.push_back(instr);
    }
    return LoadStatus::LOAD_OK;
}

LoadStatus VoleMachine::loadProgram(const string& file, int startAddr, int& badLine) {
    badLine = 0;

    if (startAddr % 2 != 0) {
        return LoadStatus::ODD_ADDRESS;
    }
    if (startAddr < 0 || startAddr > 0xFE) {
        return LoadStatus::ADDRESS_OUT_OF_RANGE;
    }

    vector<Instruction> instructions;
    LoadStatus status = readProgramFile(file, instructions, badLine);
    if (status != LoadStatus::LOAD_OK) {
        return status;
    }

    if (startAddr + instructions.size() * 2 > Constants::MEMORY_SIZE ) {
        return LoadStatus::PROGRAM_TOO_LARGE;
    }

    mem.clear();

    for (int i = 0; i < instructions.size(); i++) {
        int address = startAddr + i * 2;


        int value = instructions[i].opcode()*(1<<12) + instructions[i].r()*(1<<8) + instructions[i].xy();

        mem.write(address, value / 256);
        mem.write(address + 1, value % 256);
    }

    cpu.reset(startAddr);
    screen.clear();
    programLoaded = true;

    return LoadStatus::LOAD_OK;
}

StepResult VoleMachine::step() {
    if (!programLoaded) {
        return StepResult::STEP_NOT_LOADED;
    }

    if (cpu.isHalted()) {
        return StepResult::STEP_HALTED;
    }

    if (!cpu.fetch()) {
        return StepResult::STEP_PC_OUT_OF_RANGE;
    }

    ExecStatus result = cpu.decodeExecute();

    if (result == ExecStatus::HALTED) {
        return StepResult::STEP_HALTED;
    }

    if (result == ExecStatus::SCREEN_OUTPUT) {
        screen.append(cpu.getLastOutput());
        return StepResult::STEP_OK;
    }

    if (result == ExecStatus::OK) {
        return StepResult::STEP_OK;
    }

    return StepResult::STEP_INVALID_INSTRUCTION;
}

StepResult VoleMachine::runAll(int maxSteps) {
    for (int i = 0; i < maxSteps; i++) {
        StepResult result = step();
        if (result != StepResult::STEP_OK) {
            return result;
        }
    }
    return StepResult::STEP_LIMIT_REACHED;
}

bool VoleMachine::setMemoryCell(int addr, int value) {
    if (addr < 0 || addr >= Constants::MEMORY_SIZE) {
        return false;
    }
    mem.write(addr, value);
    return true;
}

uint8_t VoleMachine::getMemoryCell(int addr) const {
    return mem.read(addr);
}

bool VoleMachine::setRegister(int idx, int value) {
    return cpu.setRegister(idx, value);
}

uint8_t VoleMachine::getRegister(int idx) const {
    return cpu.getRegister(idx);
}

int VoleMachine::getPC() const {
    return cpu.getPC();
}

string VoleMachine::getIRString() const {
    return cpu.getIR().toString();
}

string VoleMachine::getScreenString() const {
    return screen.toString();
}

void VoleMachine::setScreenFormat(ScreenFormat f) {
    screen.setFormat(f);
}

bool VoleMachine::isProgramLoaded() const {
    return programLoaded;
}
