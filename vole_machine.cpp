#include "VoleMachine.h"

// screen class stuff

Screen::Screen() { // setup
    format = ScreenFormat::ASCII; // default is ascii
}

void Screen::append(uint8_t value) { // save byte
    output.push_back(value);
}

void Screen::clear() { // clear output
    output.clear();
}

void Screen::setFormat(ScreenFormat f) { // change mode
    format = f;
}

string Screen::toString() const { // format for printing
    string result = "";

    for (int i = 0; i < output.size(); i++) { // loop through bytes
        if (format == ScreenFormat::ASCII) { // ascii mode
            if (output[i] >= 32 && output[i] <= 126) { // printable?
                result += (char)output[i]; // show char
            } else {
                result += '.'; // dot if unreadable
            }
        }
        else if (format == ScreenFormat::DECIMAL) { // decimal mode
            result += to_string(output[i]) + " "; // print number
        }
        else { // hex mode
            const char digits[] = "0123456789ABCDEF"; // manual hex
            result += digits[output[i] / 16]; // high part
            result += digits[output[i] % 16]; // low part
            result += " ";
        }
    }
    return result;
}

// main machine

VoleMachine::VoleMachine() : mem(), cpu(mem) { // setup
    programLoaded = false; // nothing loaded yet
}

bool VoleMachine::isSkippableLine(const string& line) const { // check for comments
    int firstChar = -1;
    
    for (int i = 0; i < line.length(); i++) { // find first real char
        if (line[i] != ' ' && line[i] != '\t' && line[i] != '\r' && line[i] != '\n') {
            firstChar = i;
            break;
        }
    }
    
    if (firstChar == -1) return true; // empty line
    if (line[firstChar] == ';') return true; // comment line
    return false;
}

LoadStatus VoleMachine::readProgramFile(const string& file, vector<Instruction>& out, int& badLine) const { // read file
    badLine = 0;
    
    ifstream input(file); // open file
    if (!input) {
        return LoadStatus::FILE_NOT_FOUND;
    }

    out.clear();
    string line;
    int lineNumber = 0;

    while (getline(input, line)) { // loop lines
        lineNumber++;

        if (isSkippableLine(line)) { // skip bad lines
            continue;
        }

        Instruction instr;
        if (!Instruction::parse(line, instr)) { // check if valid hex
            badLine = lineNumber;
            return LoadStatus::INVALID_INSTRUCTION_IN_FILE;
        }
        
        out.push_back(instr); // save it
    }
    return LoadStatus::LOAD_OK;
}

LoadStatus VoleMachine::loadProgram(const string& file, int startAddr, int& badLine) {
    badLine = 0;

    if (startAddr % 2 != 0) { // must be even
        return LoadStatus::ODD_ADDRESS;
    }
    if (startAddr < 0 || startAddr > 0xFE) { // check bounds
        return LoadStatus::ADDRESS_OUT_OF_RANGE;
    }

    vector<Instruction> instructions;
    LoadStatus status = readProgramFile(file, instructions, badLine); // get instructions
    if (status != LoadStatus::LOAD_OK) {
        return status;
    }

    if (startAddr + instructions.size() * 2 > MEMORY_SIZE) { // check size
        return LoadStatus::PROGRAM_TOO_LARGE;
    }

    mem.clear(); // clear old program

    for (int i = 0; i < instructions.size(); i++) {
        int address = startAddr + i * 2;
        int value = instructions[i].value();
        
        mem.write(address, value / 256);       // write high byte
        mem.write(address + 1, value % 256);   // write low byte
    }

    cpu.reset(startAddr); // point cpu to start
    screen.clear();       // clear screen
    programLoaded = true; // ready to run
    
    return LoadStatus::LOAD_OK;
}

StepResult VoleMachine::step() { // run one instruction
    if (!programLoaded) { // check loaded
        return StepResult::STEP_NOT_LOADED;
    }

    if (cpu.isHalted()) { // check halt
        return StepResult::STEP_HALTED;
    }

    if (!cpu.fetch()) { // fetch next
        return StepResult::STEP_PC_OUT_OF_RANGE;
    }

    ExecStatus result = cpu.decodeExecute(); // execute

    if (result == ExecStatus::HALTED) {
        return StepResult::STEP_HALTED;
    }

    if (result == ExecStatus::SCREEN_OUTPUT) { // wrote to ff
        screen.append(cpu.getLastOutput()); // save output
        return StepResult::STEP_OK;
    }

    if (result == ExecStatus::OK) {
        return StepResult::STEP_OK;
    }

    return StepResult::STEP_INVALID_INSTRUCTION; // bad instruction
}

StepResult VoleMachine::runAll(int maxSteps) { // loop step
    for (int i = 0; i < maxSteps; i++) { // step limit
        StepResult result = step();
        if (result != StepResult::STEP_OK) {
            return result; // stop running
        }
    }
    return StepResult::STEP_LIMIT_REACHED; // hit limit
}

// getters and setters

bool VoleMachine::setMemoryCell(int addr, int value) {
    if (addr < 0 || addr >= MEMORY_SIZE) { // bounds check
        return false;
    }
    mem.write(addr, value);
    return true;
}

uint8_t VoleMachine::getMemoryCell(int addr) const {
    return mem.read(addr);
}

bool VoleMachine::setRegister(int idx, int value) {
    return cpu.getRegisters().write(idx, value);
}

uint8_t VoleMachine::getRegister(int idx) const {
    return cpu.getRegisters().read(idx);
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
