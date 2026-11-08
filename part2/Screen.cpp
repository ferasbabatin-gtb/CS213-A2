#include "Screen.h"

Screen::Screen() { 
    format = ScreenFormat::ASCII; 
}

void Screen::append(uint8_t value) { 
    bytes.push_back(value); 
}

void Screen::clear() { 
    bytes.clear(); 
}

void Screen::setFormat(ScreenFormat f) { 
    format = f;
}

string Screen::formatBytes(const vector<uint8_t> & bytes, ScreenFormat format) const {
    string result = "";

    for (int i = 0; i < bytes.size(); i++) { 
        if (format == ScreenFormat::ASCII) { 
            if (bytes[i] >= 32 && bytes[i] <= 126) { 
                result += (char)bytes[i]; 
            } else {
                result += '.'; 
            }
        }
        else if (format == ScreenFormat::DECIMAL) { 
            result += to_string(bytes[i]) + " "; 
        }
        else { 
            const char digits[] = "0123456789ABCDEF"; 
            result += digits[bytes[i] / 16]; 
            result += digits[bytes[i] % 16]; 
            result += " ";
        }
    }
    return result;
}

string Screen::toString() const { 
    return formatBytes(bytes, format);
}
