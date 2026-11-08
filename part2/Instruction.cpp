#include "Instruction.h"

using namespace std;


Instruction::Instruction(int value)
    : value(static_cast<uint16_t>(value & 0xFFFF)) {}

int Instruction::opcode() const { return (value >> 12) & 0xF; }
int Instruction::r()      const { return (value >> 8)  & 0xF; }
int Instruction::s()      const { return (value >> 4)  & 0xF; }
int Instruction::t()      const { return value & 0xF; }
int Instruction::x()      const { return value & 0xF; }
int Instruction::xy()     const { return value & 0xFF; }

bool Instruction::isValid() const {
    switch (opcode()) {
        case 0x1: case 0x2: case 0x3:
        case 0x5: case 0x6: case 0x7: case 0x8: case 0x9:
        case 0xB:
            return true;
        case 0x4: return r() == 0;
        case 0xA: return s() == 0;
        case 0xC: return (value & 0x0FFF) == 0;
        default:  return false;
    }
}

string Instruction::toString() const {
    const char* digits = "0123456789ABCDEF";
    string text(4, '0');
    for (int i = 0; i < 4; i++)
        text[i] = digits[(value >> (12 - 4 * i)) & 0xF];
    return text;
}
bool Instruction::parse(const std::string& line, Instruction& out){
    for(int i = 0;i<4;i++){
        if((line[i]<='F' && line[i]>='A') || (line[i]>='0' && line[i]<='9' ||( line[i]>='a' && line[i]<='f'))){
            continue;
        }
        else{
            return false;
        }

        
    }


    int val = stoi(line.substr(0,4),nullptr,16);
    out = Instruction(val);
    return out.isValid();

   
    
}