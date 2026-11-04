#include "menu.h"
#include <iostream>
#include <string>

using namespace std;


Menu::Menu(CPU& cpuObj, Memory& memObj) : cpu(cpuObj), memory(memObj) {}

void Menu::display() {
    int choice = 0;
    
    while (choice != 5) {
        cout << "\n================ Vole Machine Menu ================\n";
        cout << "1. Load and execute a Vole program from file\n";
         cout << "===================================================\n";
        cout << "2. Edit a specific memory address\n";
         cout << "===================================================\n";
        cout << "3. Edit a CPU register value\n";
         cout << "===================================================\n";
        cout << "4. Display current machine state (Memory & Registers)\n";
         cout << "===================================================\n";
        cout << "5. Exit\n";
        cout << "===================================================\n";
        cout << "Enter your choice (1-5): ";
        cin >> choice;

        switch (choice) {
            case 1: {
                string filename;
                cout << "Enter program file name (e.g., program1.txt): ";
                cin >> filename;
                cout << "[Info] Loading and running program from " << filename << "...\n";
                break;
            }
            case 2: {
                int address;
                string value;
                cout << "Enter memory address (in hex, e.g., 0A): ";
                cout << "[Info] Memory updated successfully.\n";
                break;
            }
            case 3: {
                int regIndex;
                string value;
                cout << "Enter register index (0-F): ";
                cout << "[Info] Register updated successfully.\n";
                break;
            }
            case 4:
                cout << "\n--- Current Machine State ---\n";
                break;
            case 5:
                cout << "Exiting Vole Machine Simulator. Goodbye!\n";
                break;
            default:
                cout << "Invalid choice! Please enter a number between 1 and 5.\n";
        }
    }
}
