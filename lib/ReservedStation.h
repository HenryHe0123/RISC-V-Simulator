#ifndef RISC_V_SIMULATOR_RESERVED_STATION_H
#define RISC_V_SIMULATOR_RESERVED_STATION_H

#include <cstring>
#include "CDB.h"
#include "ALU.h"
#include "ReorderBuffer.h"

class ReorderBuffer;

extern const int RSEntrySize = 32;

struct RSEntry {
    bool empty = true;
    bool busy = false; //if executing
    InstructionOPT opt = NONE;
    unsigned val1 = 0;
    unsigned val2 = 0;
    int Q1 = -1; //Q == -1 means value available, else we use tag Q
    int Q2 = -1;
    int dest = 0; //ROB entry tag

    RSEntry() = default;

    [[nodiscard]] inline bool ready() const { return Q1 == -1 && Q2 == -1; }

    inline void setVal1(unsigned v) {
        Q1 = -1;
        val1 = v;
    }

    inline void setVal2(unsigned v) {
        Q2 = -1;
        val2 = v;
    }

    explicit RSEntry(InstructionOPT opt_) : opt(opt_), empty(false) {}
};

class ReservedStation {
public:
    inline void init(ReorderBuffer *reorderBuffer_ = nullptr) { ROB = reorderBuffer_; }

    inline void refresh() { memcpy(entries, nextEntries, sizeof(entries)); }

    void clear() { for (RSEntry &entry: nextEntries) entry.empty = true; }

    bool addEntry(const RSEntry &entry);

    void execute();

private:
    RSEntry entries[RSEntrySize];
    RSEntry nextEntries[RSEntrySize];
    ALU cALU; //common ALU (physically one by one)
    ReorderBuffer *ROB = nullptr;

    void selfBroadcast(int tag, unsigned val);

    void sendResToROB(int dest, unsigned res);
};

bool ReservedStation::addEntry(const RSEntry &entry) {
    for (int i = 0; i < RSEntrySize; ++i) {
        if (entries[i].empty) {
            nextEntries[i] = entry;
            nextEntries[i].empty = false;
            return true;
        }
    }
    return false;
}

void ReservedStation::execute() {
    for (int i = 0; i < RSEntrySize; ++i) {
        const RSEntry &entry = entries[i]; //all modification should apply to entry_next
        RSEntry &entry_nxt = nextEntries[i];
        if (entry.empty) continue;
        if (entry.ready()) {
            if (entry.busy) { //compute finished
                unsigned result = ALU::compute(entry.opt, entry.val1, entry.val2);
                sendResToROB(entry.dest, result);
                entry_nxt.empty = true; //clear entry
            } else entry_nxt.busy = true;
        } else { //unready
            if (CDB.on) {
                if (entry.Q1 == CDB.cur.tag) entry_nxt.setVal1(CDB.cur.val);
                if (entry.Q2 == CDB.cur.tag) entry_nxt.setVal2(CDB.cur.val);
            }
        }
    }
}

void ReservedStation::selfBroadcast(int tag, unsigned int val) {
    if (tag < 0) return;
    for (RSEntry &entry: nextEntries) {
        if (entry.empty || entry.ready()) continue;
        if (entry.Q1 == tag) entry.setVal1(val);
        if (entry.Q2 == tag) entry.setVal2(val);
    }
}

void ReservedStation::sendResToROB(int dest, unsigned int res) {
    if (dest < 0) throw std::exception();
    const ROBEntry &cur = ROB->visitCur(dest);
    ROBEntry &nxt = ROB->visitNxt(dest);
    if (cur.opt == JALR) {
        nxt.ready = true;
        jalrBus.broadcast(res);
        return;
    }
    if (cur.opt == JAL || cur.opt == LUI) throw std::exception(); //shouldn't appear
    nxt.ready = true;
    switch (OPTtype(cur.opt)) {
        case MEM:
            nxt.value = res; //memory address
            break;
        case BRANCH:
            nxt.predict_accurate = (res == cur.predict);
            break;
        case REG: //not jump now
            nxt.value = res; //register value
            selfBroadcast(dest, res);
            break;
        default: //NONE or HALT
            throw std::exception();
    }
}


#endif //RISC_V_SIMULATOR_RESERVED_STATION_H
