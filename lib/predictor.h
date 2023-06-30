#ifndef RISC_V_SIMULATOR_PREDICTOR_H
#define RISC_V_SIMULATOR_PREDICTOR_H

#include <cstdint>
#include "decoder.h"

extern const int kP = 2017;

unsigned hash(unsigned u) { return u * 998244353 % kP; }

class BitsCounter { //2-bit saturation counter
public:
    BitsCounter() = default;

    inline bool res() { return b[0]; }

    inline BitsCounter &operator++() { //前置++
        if (b[0] || (!b[0] & !b[1])) b[1] = true;
        else { //01->10
            b[0] = true;
            b[1] = false;
        }
        return *this;
    }

    inline BitsCounter &operator--() {
        if (!b[0] || (b[0] & b[1])) b[1] = false;
        else { //10->01
            b[0] = false;
            b[1] = true;
        }
        return *this;
    }

    inline void reset() {
        b[0] = true;
        b[1] = false;
    }

private:
    bool b[2] = {true, false}; //10 (true)
};

class Predictor {
public:
    Predictor() = default;

    void init() { for (auto &c: counter) c.reset(); }

private: //only allow IU and ROB visit
    BitsCounter counter[kP]{};

    bool predict(const Instruction &ins) {
        return counter[hash(ins.src)].res();
    }

    void update(unsigned uhash, bool realRes) {
        if (realRes) ++counter[uhash];
        else --counter[uhash];
    }

    friend class InstructionUnit;

    friend class ReorderBuffer;
};


#endif //RISC_V_SIMULATOR_PREDICTOR_H
