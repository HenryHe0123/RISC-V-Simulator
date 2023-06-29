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
        return (unsigned(mem[pos + 3]) << 24) + (unsigned(mem[pos + 2]) << 16) +
               (unsigned(mem[pos + 1]) << 8) + unsigned(mem[pos]);
    }

    inline void writeU32(unsigned pos, unsigned val) {
        mem[pos++] = uint8_t(val);
        val >>= 8;
        mem[pos++] = uint8_t(val);
        val >>= 8;
        mem[pos++] = uint8_t(val);
        val >>= 8;
        mem[pos] = uint8_t(val);
    }

private:
    uint8_t mem[N]{0};
};

#endif //RISC_V_SIMULATOR_MEMORY_H
