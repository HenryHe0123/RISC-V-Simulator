#ifndef RISC_V_SIMULATOR_CDB_H
#define RISC_V_SIMULATOR_CDB_H
//common data (bus) hpp
#include "queue.h"
#include "decoder.h"
#include "memory.h"
#include "register.h"
#include "predictor.h"

unsigned pc = 0;
bool STALL = false;
AdaptiveTwoLevalPredictor predictor;
//global variable

template<class T>
class Bus {
public:
    bool on = false;
    T cur{};

    void flush() {
        cur = next;
        on = on_nxt;
        on_nxt = false;
    }

    void broadcast(const T &value) {
        on_nxt = true;
        next = value;
    }

    void read(T &u) const { if (on) u = cur; }

private:
    T next{};
    bool on_nxt = false;

    friend void initCommonData();
};

struct Pair {
    int tag = -2;
    unsigned val = 0;
};

Bus<Pair> CDB; //for ROB commit use
Bus<unsigned> jalrBus; //for jalr(pc) result transition
Bus<unsigned> predictBus; //broadcast correct pc for predict false

void initCommonData() {
    pc = 0;
    STALL = false;
    CDB.on = CDB.on_nxt = false;
    jalrBus.on = jalrBus.on_nxt = false;
    predictBus.on = predictBus.on = false;
    predictor.init();
}

void flushBus() {
    CDB.flush();
    jalrBus.flush();
    predictBus.flush();
}

#endif //RISC_V_SIMULATOR_CDB_H
