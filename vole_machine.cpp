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

// =============== SCREEN ===============

// This class holds the bytes the program prints
class Screen {
private:
    vector<uint8_t> output; // Stores the printed bytes
    ScreenFormat format;    // How we want to display them

public:
    Screen() {
        format = ScreenFormat::ASCII; // Default format
    }

    // Add a new byte to the output when the program prints
    void append(uint8_t value) {
        output.push_back(value);
    }

    // Clear the screen for a new program
    void clear() {
        output.clear();
    }

    // Change how the screen is formatted
    void setFormat(ScreenFormat f) {
        format = f;
    }

    // convert the stored bytes into a printable string
    string toString() const {
        string result = "";

        for (int i = 0; i < output.size(); i++) { // loop through every byte

            if (format == ScreenFormat::ASCII) {
                if (output[i] >= 32 && output[i] <= 126) {
                    result += (char)output[i]; // Show readable characters
                } else {
                    result += '.'; // Show a dot for unreadable characters
                }
            }
            else if (format == ScreenFormat::DECIMAL) {
                result += to_string(output[i]) + " "; // Show as normal numbers
            }
            else {
                // Manual hex conversion to uppercase letters
                const char digits[] = "0123456789ABCDEF";
                result += digits[output[i] / 16];
                result += digits[output[i] % 16];
                result += " ";
            }
        }

        return result;
    }
};
