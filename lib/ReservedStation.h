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
    //todo
}


#endif //RISC_V_SIMULATOR_RESERVED_STATION_H
