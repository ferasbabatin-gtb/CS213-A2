#include "ByteStorage.h"
#include <stdexcept>

ByteStorage::ByteStorage(int size) : cells(size > 0 ? size : 0, 0) {}

uint8_t ByteStorage::read(int index) const {
    if (!isValidIndex(index))
        throw std::out_of_range("ByteStorage::read: invalid index");
    return cells[index];
}

void ByteStorage::write(int index, int value) {
    if (!isValidIndex(index))
        throw std::out_of_range("ByteStorage::write: invalid index");
    cells[index] = static_cast<uint8_t>(value & 0xFF);
}

int ByteStorage::size() const {
    return static_cast<int>(cells.size());
}

void ByteStorage::clear() {
    for (auto &c : cells) c = 0;
}

bool ByteStorage::isValidIndex(int index) const {
    return index >= 0 && index < size();
}

