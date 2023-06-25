#ifndef RISC_V_SIMULATOR_MEMORY_H
#define RISC_V_SIMULATOR_MEMORY_H

#include <cstdint>

extern const int N = 1e6;

class RAM {
public:
    RAM() = default;

    inline uint8_t readByte(unsigned pos) { return mem[pos]; }

    inline void writeByte(unsigned pos, uint8_t byte) { mem[pos] = byte; }

    inline unsigned readU32(unsigned pos) {
        unsigned val = mem[pos];
        val = (val << 8) | mem[++pos];
        val = (val << 8) | mem[++pos];
        val = (val << 8) | mem[++pos];
        return val;
    }

private:
    uint8_t mem[N]{0};
};

#endif //RISC_V_SIMULATOR_MEMORY_H
