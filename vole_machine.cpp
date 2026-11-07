#ifndef VOLEMACHINE_HPP
#define VOLEMACHINE_HPP

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

// These are the possible results when loading a file
enum class LoadStatus {
    LOAD_OK,
    FILE_NOT_FOUND,
    ODD_ADDRESS,
    ADDRESS_OUT_OF_RANGE,
    PROGRAM_TOO_LARGE,
    INVALID_INSTRUCTION_IN_FILE
};

// These are the possible results after running one instruction
enum class StepResult {
    STEP_OK,
    STEP_HALTED,
    STEP_INVALID_INSTRUCTION,
    STEP_PC_OUT_OF_RANGE,
    STEP_NOT_LOADED,
    STEP_LIMIT_REACHED
};

// The different ways we can display the screen output
enum class ScreenFormat {
    ASCII,
    HEX,
    DECIMAL
};


// =============== SCREEN ===============

class Screen {
private:
    vector<uint8_t> output; // Stores the printed bytes
    ScreenFormat format;    // How we want to display them

public:
    Screen() { // Constructor
        format = ScreenFormat::ASCII; // Default format
    }

    void append(uint8_t value) { // Add a new byte when the program prints
        output.push_back(value);
    }

    void clear() { // Clear the screen for a new program
        output.clear();
    }

    void setFormat(ScreenFormat f) { // Change how the screen is formatted
        format = f;
    }

    string toString() const { // Convert the stored bytes into a printable string
        string result = "";

        for (int i = 0; i < output.size(); i++) { // Loop through every byte
            if (format == ScreenFormat::ASCII) { // Check if ASCII
                if (output[i] >= 32 && output[i] <= 126) { // Check if printable
                    result += (char)output[i]; // Show readable characters
                } else {
                    result += '.'; // Show a dot for unreadable characters
                }
            }
            else if (format == ScreenFormat::DECIMAL) { // Check if decimal
                result += to_string(output[i]) + " "; // Show as normal numbers
            }
            else { // Must be hex
                const char digits[] = "0123456789ABCDEF"; // Manual hex conversion
                result += digits[output[i] / 16]; // High nibble
                result += digits[output[i] % 16]; // Low nibble
                result += " ";
            }
        }

        return result;
    }
};

// =============== VOLE MACHINE ===============

class VoleMachine {
private:
    Memory mem;          //  machine's main memory
    Cpu cpu;             // machine's processor
    Screen screen;       // machine's output screen
    bool programLoaded;  // tracks if a program is ready to run

    // These two functions are explicitly in the UML design
    bool isSkippableLine(const string& line) const;
    LoadStatus readProgramFile(const string& file, vector<Instruction>& out, int& badLine) const;

public:
    VoleMachine() : mem(), cpu(mem) { // Constructor
        programLoaded = false; // Nothing is loaded at start
    }
