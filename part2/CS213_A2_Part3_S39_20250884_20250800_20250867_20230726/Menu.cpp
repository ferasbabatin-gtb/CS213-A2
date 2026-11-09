#include "Menu.h"
#include <iostream>

using namespace std;

Menu::Menu() : machine(), running(true) {

}

int Menu::readHex(string prompt){
    cout << prompt;
    string input;

    while(true){
        getline(cin, input);
    try {
        return stoi(input, nullptr, 16);
    } catch (const exception& e) {
        cout << "Invalid hex input: " << input << endl;
        cout << prompt;

    }
}
}
string Menu::toHex(int value, int digits){
    string s(digits, '0');
    for (int i = 0; i < digits; i++) {
        s[i] = "0123456789ABCDEF"[(value >> (4 * (digits - 1 - i))) & 0xF];
    }
    return s;
}
ScreenFormat Menu::chooseScreenFormat() {
    cout << "Choose screen format:\n";
    cout << "1. ASCII\n";
    cout << "2. HEX\n";
    cout << "3. DECIMAL\n";
    int choice;
    string input;
    getline(cin,input);
    try{
    choice = stoi(input);
    }
    catch(const exception & e){
        choice = 4;
    }
    switch (choice) {
        case 1: return ScreenFormat::ASCII;
        case 2: return ScreenFormat::HEX;
        case 3: return ScreenFormat::DECIMAL;
        default:
            cout << "Invalid choice, defaulting to ASCII.\n";
            return ScreenFormat::ASCII;
    }
}
RunMode Menu::chooseRunMode() {
    cout << "Choose run mode:\n";
    cout << "1. Run All\n";
    cout << "2. Step\n";

    string input;
    getline(cin,input);

    if(input == "1") return RunMode::RUN_ALL;
    else if(input == "2") return RunMode::STEP_BY_STEP;
    else{cout << "Invalid choice, defaulting to Run All.\n";
            return RunMode::RUN_ALL;}

}
void Menu::overwriteFlow() {
    cout << "Memory or cpu register overwrite(1 for memroy,0 for cpu registers)?\n";

    string input;
    getline(cin,input);
    if(input == "0"){
        int regIndex = readHex("Enter register index (0-15 in hex): ");
        int value = readHex("Enter new value (in hex): ");
        if(!machine.setRegister(regIndex, value)) {
            cout << "Error: Register index out of range.\n";
            return;
        }
        cout << "Register R" << toHex(regIndex, 1) << " overwritten with " << toHex(value, 4) << ".\n";
    } else if(input == "1"){
        int addr = readHex("Enter address to overwrite (in hex): ");
        int value = readHex("Enter new value (in hex): ");
        if(!machine.setMemoryCell(addr, value)) {
            cout << "Error: Address out of range.\n";
            return;
        }
        cout << "Memory at " << toHex(addr, 2) << " overwritten with " << toHex(value, 2) << ".\n";
    } else {
        cout << "Invalid choice. Please enter 0 for CPU register or 1 for memory.\n";
    }
    }
bool Menu::loadProgramFlow() {
    string filename;
    cout << "Enter program filename: ";
    getline(cin, filename);
    int startAddr = readHex("Enter starting address (in hex): ");
    int badLine;
    LoadStatus status = machine.loadProgram(filename, startAddr, badLine);
    if (status != LoadStatus::LOAD_OK) {
        showLoadError(status, badLine);
        return false;
    } else {
        cout << "Program loaded successfully.\n";
        cout << "enter screen format for display:\n";
        machine.setScreenFormat(chooseScreenFormat());
        printState();
        return true;
    }
}
void Menu::showLoadError(LoadStatus result, int badLine) {
    switch (result) {
        case LoadStatus::FILE_NOT_FOUND:
            cout << "Error: File not found.\n";
            break;
        case LoadStatus::ODD_ADDRESS:
            cout << "Error: Odd address encountered\n";
            break;
        case LoadStatus::ADDRESS_OUT_OF_RANGE:
            cout << "Error: Address out of range\n";
            break;
        case LoadStatus::PROGRAM_TOO_LARGE:
            cout << "Error: Program too large to fit in memory.\n";
            break;
        case LoadStatus::INVALID_INSTRUCTION_IN_FILE:
            cout << "Error: Invalid instruction in file at line " << badLine << ".\n";
            break;
        default:
            cout << "Unknown error occurred while loading program.\n";
    }

    }
void Menu::showError(StepResult result) {
    switch (result) {
        case StepResult::STEP_HALTED:
            cout << "Program halted.\n";
            break;
        case StepResult::STEP_INVALID_INSTRUCTION:
            cout << "Error: Invalid instruction encountered.\n";
            break;
        case StepResult::STEP_PC_OUT_OF_RANGE:
            cout << "Error: Program counter out of range.\n";
            break;
        case StepResult::STEP_NOT_LOADED:
            cout << "Error: No program loaded.\n";
            break;
        case StepResult::STEP_LIMIT_REACHED:
            cout << "Error: Step limit reached.\n";
            break;
        default:
            cout << "Unknown error occurred during execution.\n";
    }
}
void Menu::stepFlow() {
    while (true) {
        StepResult result = machine.step();
        if (result != StepResult::STEP_OK) {
            showError(result);
            printState();
            break;
        } else {
            printState();
            string _;
            getline(cin, _);

        }
    }

}
void Menu::runAllFlow() {
    StepResult result = machine.runAll(Constants::MAX_STEPS);
    if (result != StepResult::STEP_OK) {
        showError(result);
        printState();
    } else {
        printState();
    }

}
void Menu::printMenu() {
    cout << "\n===== Vole Machine Menu =====\n";
    cout << "1. Load Program from File\n";
    cout << "=============================\n";
    cout << "2. Edit Memory Cell or a Register\n";
    cout << "=============================\n";
    cout << "3. Run a Program\n";
    cout << "=============================\n";
    cout << "4. Exit\n";
    cout << "=============================\n";
    cout << "Enter your choice: ";
    cout << "=============================\n";
}

void Menu::printReg() {
    cout << "=== CPU Registers ===\n";
    for (int i = 0; i < Constants::REGISTER_COUNT; ++i) {
        cout << "R" << toHex(i,1) << ": " << toHex(machine.getRegister(i), 2) << "\n";
    }
}

void Menu::printMem() {
    cout << "=== Memory===\n";
    for (int r = 0; r < Constants::MEMORY_SIZE / 16; r++) {
        for (int c = 0; c < 16; c++) {
            int addr = r * 16 + c;
            cout << toHex(machine.getMemoryCell(addr), 2) << " ";
        }
        cout << "\n";
    }
}

void Menu::printState() {
    printReg();
    printIRandPC();
    printMem();
    printScreen();
}
void Menu::printIRandPC(){
    cout << "=== Instruction Register and Program Counter ===\n";
    cout << "IR: " << machine.getIRString() << "\n";
    cout << "PC: " << toHex(machine.getPC(), 2) << "\n";
}
void Menu::printScreen(){
    cout << "=== Screen State ===\n";
    cout << machine.getScreenString() << "\n";
}
void Menu::runProgramFlow() {
    RunMode mode = chooseRunMode();
    if (mode == RunMode::RUN_ALL) {
        runAllFlow();
    } else if (mode == RunMode::STEP_BY_STEP) {
        stepFlow();
    }
    if(wantToExit()){
        running = false;
    }
    else{
        if(loadProgramFlow()) runProgramFlow();
    }
}
bool Menu::wantToExit() {
    string choice;
    cout << "Do you want to exit or load an other program? (y/n): ";
    getline(cin,choice);
    if (choice == "y" || choice == "Y") {
        return true;
    }
    else {
        return false;
    }
}
void Menu::run() {
    int choice = 0;
    while (running) {
        printMenu();
        string input;
        getline(cin,input);
        try{
            choice = stoi(input);
        }
        catch(const  exception & e){
            choice = 5;
        }
        switch (choice) {
            case 1:
                loadProgramFlow();
                break;
            case 2:
                overwriteFlow();
                break;
            case 3:
                runProgramFlow();
                break;
            case 4:
                if (wantToExit()) {
                    running = false;
                }
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
        }
    }
}
