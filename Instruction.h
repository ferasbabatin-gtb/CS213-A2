#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <string>
#include <cstdint>

class Instruction {
private:
    uint16_t value;

public:
    Instruction(int value = 0);

    static bool parse(const std::string& line, Instruction& out);

    int opcode() const;
    int r() const;
    int s() const;
    int t() const;
    int x() const;
    int xy() const;

    bool isValid() const;
    std::string toString() const;
};

#endif