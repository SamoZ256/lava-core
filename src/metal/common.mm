#include "lvcore/metal/common.hpp"

size_t roundToMultipleOf16(size_t size) {
    size_t remainder = size % 16;

    if (remainder == 0)
        return size;

    return size + 16 - remainder;
}
