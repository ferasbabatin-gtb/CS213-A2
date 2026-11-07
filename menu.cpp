#include "menu.h"
#include <iostream>

using namespace std;

Menu::Menu(VoleMachine& machineObj) : machine(machineObj), running(true) {}

void Menu::loadProgram(string filename) {
    cout << "[Menu] Loading file: " << filename << endl;
}

void Menu::loadInteractive(string val, int digits) {
    cout << "[Menu] Interactive load active\n";
}

void Menu::loadStepByStep(string val, int digits) {
    cout << "[Menu] Step-by-step load active\n";
}

void Menu::loadInstantaneous() {
    cout << "[Menu] Instantaneous load active\n";
}

void Menu::chooseExecutionMode() {
    cout << "[Menu] Choose execution mode\n";
}

void Menu::runProgram() {
    cout << "[Menu] Program running\n";
}

void Menu::displayMenu() {
    cout << "\n===== Vole Machine Menu =====\n"
    cout << "1. Load Program from File\n"
    cout << "=============================\n"
    cout << "2. Edit Memory Cell\n"
    cout << "=============================\n"
    cout << "3. Edit CPU Register\n"
    cout << "=============================\n"
    cout << "4. Display Machine Status\n"
    cout << "=============================\n"
    cout << "5. Run Program\n"
    cout << "=============================\n"
    cout << "6. Exit\n"
    cout << "=============================\n"
    cout << "Enter your choice: ";
    cout << "=============================\n"
}

void Menu::displayCPU() {
    cout << "=== CPU Registers ===\n";
}

void Menu::displayMemory() {
    cout << "=== Memory State ===\n";
}

void Menu::displayStatus() {
    displayCPU();
    displayMemory();
}

void Menu::editCell(int address, string value) {
    cout << "[Menu] Updated memory at address " << address << "\n";
}

void Menu::editRegister(int regIndex, string value) {
    cout << "[Menu] Updated register " << regIndex << "\n";
}

void Menu::printMemory() {
    displayMemory();
}

void Menu::printRegisters() {
    displayCPU();
}

void Menu::printStatus() {
    displayStatus();
}

void Menu::mainLoop() {
    int choice = 0;
    while (running) {
        displayMenu();
        if (!(cin >> choice)) break;

        if (choice == 1) {
            string file;
            cout << "Enter filename: ";
            cin >> file;
            loadProgram(file);
        } 
        else if (choice == 2) {
            int addr; string val;
            cout << "Enter address and value: ";
            cin >> addr >> val;
            editCell(addr, val);
        } 
        else if (choice == 3) {
            int reg; string val;
            cout << "Enter register index and value: ";
            cin >> reg >> val;
            editRegister(reg, val);
        } 
        else if (choice == 4) {
            displayStatus();
        } 
        else if (choice == 5) {
            runProgram();
        } 
        else if (choice == 6) {
            running = false;
            cout << "Exiting\n";
        } 
        else {
            cout << "Invalid choice!\n";
        }
    }
}

void Menu::run() {
    mainLoop();
}
