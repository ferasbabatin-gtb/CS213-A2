#ifndef BYTESTORAGE_H
#define BYTESTORAGE_H

#include <vector>
#include <cstdint>
#include "Constants.h"
class ByteStorage {
protected:
    std::vector<uint8_t> cells;
    ~ByteStorage() = default;

public:
    ByteStorage(int size);
    uint8_t read(int index) const;
    void    write(int index, int value);
    int     size() const;
    void    clear();
    bool    isValidIndex(int index) const;
};


class Memory : public ByteStorage {
public:
    Memory();
};

class CpuRegisters : public ByteStorage {
public:
    CpuRegisters();
};
#endif