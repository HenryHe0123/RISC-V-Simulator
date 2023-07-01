#ifndef RISC_V_SIMULATOR_PREDICTOR_H
#define RISC_V_SIMULATOR_PREDICTOR_H

#include <cstdint>
#include "decoder.h"
#include "queue.h"

extern const int kWidth = 13;

unsigned hash(unsigned u) { return Decoder::getPart(u, kWidth + 11, 12); }

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
        b[0] = false;
        b[1] = true;
    }

private:
    bool b[2] = {false, true}; //01 (weak false)
};

class Predictor {
public:
    virtual void init() = 0;

protected:
    virtual bool predict(const Instruction &ins) = 0;

    virtual void update(unsigned uhash, bool realRes) = 0;
}; //abstract class

class DefaultPredictor : Predictor {
public:
    DefaultPredictor() = default;

    void init() override {}

private: //only allow IU and ROB visit
    bool predict(const Instruction &ins) override { return true; }

    void update(unsigned uhash, bool realRes) override {}

    friend class InstructionUnit;

    friend class ReorderBuffer;
}; //always true

class BimodalPredictor : Predictor { //2KB
public:
    BimodalPredictor() = default;

    void init() override { for (auto &c: counter) c.reset(); }

private: //only allow IU and ROB visit
    BitsCounter counter[1 << kWidth]{};

    bool predict(const Instruction &ins) override {
        return counter[hash(ins.src)].res();
    }

    void update(unsigned uhash, bool realRes) override {
        if (realRes) ++counter[uhash];
        else --counter[uhash];
    }

    friend class InstructionUnit;

    friend class ReorderBuffer;
}; //one-leval predictor

class AdaptiveTwoLevalPredictor : Predictor { //8KB
public:
    void init() override {
        for (auto &r: BHR) memset(r.p, 0, sizeof(r.p));
        for (auto &t: PHT) t.reset();
    }

private: //only allow IU and ROB visit
    static const int historyBits = 2;

    struct BranchHistoryRegister {
        bool p[historyBits]{false};

        inline void update(bool res) {
            for (int i = 0; i < historyBits - 1; ++i) p[i] = p[i + 1];
            p[historyBits - 1] = res;
        }

        inline uint8_t val() {
            uint8_t u = 0;
            for (bool b: p) u = (u << 1) + b;
            return u;
        }
    } BHR[1 << kWidth];

    struct PatternHistoryTable {
        BitsCounter counter[1 << historyBits]{};

        inline void reset() { for (auto &c: counter) c.reset(); }
    } PHT[1 << kWidth];

    bool predict(const Instruction &ins) override {
        unsigned uhash = hash(ins.src);
        return PHT[uhash].counter[BHR[uhash].val()].res();
    }

    void update(unsigned uhash, bool realRes) override {
        if (realRes) ++PHT[uhash].counter[BHR[uhash].val()];
        else --PHT[uhash].counter[BHR[uhash].val()];
        BHR[uhash].update(realRes);
    }

    friend class InstructionUnit;

    friend class ReorderBuffer;
}; //local branch prediction

#endif //RISC_V_SIMULATOR_PREDICTOR_H
