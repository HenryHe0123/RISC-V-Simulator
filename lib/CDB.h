#ifndef RISC_V_SIMULATOR_CDB_H
#define RISC_V_SIMULATOR_CDB_H
//common data (bus) hpp
#include "queue.h"
#include "decoder.h"
#include "memory.h"
#include "register.h"

unsigned pc = 0;
bool issue_halt = false;
bool issue_stall = false;
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
};

struct Pair {
    int tag = -1;
    unsigned val = 0;
};

Bus<Pair> CDB; //for ROB commit use
Bus<unsigned> jalrBus; //for jalr(pc) result transition

#endif //RISC_V_SIMULATOR_CDB_H
