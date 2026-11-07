#ifndef MENU_H
#define MENU_H

#include <string>
#include "vole_machine.h" 

class Menu {
private:
    VoleMachine& machine; 
    bool running;

public:
   
    Menu(VoleMachine& machineObj);

    
    void loadProgram(std::string filename);
    void loadInteractive(std::string val, int digits);
    void loadStepByStep(std::string val, int digits);
    void loadInstantaneous();
    void chooseExecutionMode();


    void runProgram();
    void displayMenu();
    void displayCPU();
    void displayMemory();
    void displayStatus();

