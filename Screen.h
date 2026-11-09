#ifndef SCREEN_H
#define SCREEN_H

#include <vector>
#include <string>
#include <cstdint>
#include "Constants.h"

using namespace std;

class Screen {
private:
    vector<uint8_t> bytes; 
    ScreenFormat format;    
    string formatBytes(const vector<uint8_t> & bytes, ScreenFormat format) const;

public:
    Screen(); 
    void append(uint8_t value);
    void clear();
    void setFormat(ScreenFormat f);
    string toString() const;
};

#endif
