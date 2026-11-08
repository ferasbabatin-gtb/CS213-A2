#ifndef MENU_H
#define MENU_H

#include <string>
#include "vole_machine.h" 

class Menu 
{
private:
    private:
    VoleMachine machine;
    bool running;

    int readHex(string prompt);
    string toHex(int value, int digits);

    void printMenu();
    void loadProgramFlow();
    void overwriteFlow();
    RunMode chooseRunMode();
    ScreenFormat chooseScreenFormat();
    void runProgramFlow();
    void runAllFlow();
    void stepFlow();
    bool wantToExit();

    void showError(StepResult result);
    void showLoadError(LoadStatus result, int badLine);

    void printMem();
    void printReg();
    void printIRandPC();
    void printScreen();
    void printState();
public:
   
    Menu();
    void run();
};
#endif
