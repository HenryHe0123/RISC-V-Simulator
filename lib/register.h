#ifndef RISC_V_SIMULATOR_REGISTER_H
#define RISC_V_SIMULATOR_REGISTER_H

#include <cstring>

extern const int regSize = 32;

struct Register {
    unsigned value = 0;
    int tag = -1;
    bool valid = true;

    inline void reset() {
        value = 0;
        tag = -1;
        valid = true;
    }
};

class RegisterFile {
public:
    void refresh() { memcpy(regs, nextRegs, sizeof(regs)); }

    void clear() { for (auto &r: nextRegs) r.reset(); }

    void write(unsigned id, unsigned val, int dependency = -2) {
        if (id && id < regSize) {
            nextRegs[id].value = val;
            if (nextRegs[id].tag == dependency || dependency == -2) nextRegs[id].valid = true;
        }
    }

    void aboutToWrite(unsigned id, int dependency) {
        if (id && id < regSize) {
            nextRegs[id].valid = false;
            nextRegs[id].tag = dependency;
        }
    }

    bool valid(unsigned id) { return regs[id].valid; }

    void read(unsigned id, Register &reg) { reg = regs[id]; }

private:
    Register regs[regSize]{};
    Register nextRegs[regSize]{};
};


#endif //RISC_V_SIMULATOR_REGISTER_H
