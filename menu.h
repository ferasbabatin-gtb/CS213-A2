#ifndef MENU_H
#define MENU_H

#include "cpu.h"
#include "memory.h"

class Menu {
private:
    CPU& cpu;
    Memory& memory;

public:
    Menu(CPU& cpuObj, Memory& memObj);
    void display();
};

#endif
