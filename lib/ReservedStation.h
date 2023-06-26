#ifndef RISC_V_SIMULATOR_RESERVED_STATION_H
#define RISC_V_SIMULATOR_RESERVED_STATION_H

#include <cstring>
#include "decoder.h"
#include "ALU.h"
#include "ReorderBuffer.h"

extern const int RSEntrySize = 32;

struct RSEntry {
    bool empty = true;
    bool busy = false; //if executing
    InstructionOPT opt = NONE;
    unsigned val1 = 0;
    unsigned val2 = 0;
    unsigned Q1 = 0; //Q == 0 means value available, else we use tag Q
    unsigned Q2 = 0;
    unsigned dest = 0; //ROB entry tag
};

class ReservedStation {
public:
    void initROB(ReorderBuffer *reorderBuffer = nullptr) { ROB = reorderBuffer; }

    void refresh() { memcpy(entries, nextEntries, sizeof(entries)); }

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
