#ifndef RISC_V_SIMULATOR_REORDER_BUFFER_H
#define RISC_V_SIMULATOR_REORDER_BUFFER_H

#include "queue.h"
#include "decoder.h"
#include "register.h"
#include "ReservedStation.h"

struct ROBEntry {
    bool ready = false;
    bool predict = false;
    InstructionOPT opt = NONE;
    unsigned dest = 0; //reg index for load or ALU operations, memory address for store
    unsigned value = 0; //result value
    //for BRANCH instruction, dest for predict pc, value for other pc branch

    [[nodiscard]] inline bool dest_for_reg() const { return OPTtype(opt) == REG || opt <= LHU && opt; }

    [[nodiscard]] inline bool dest_for_memory() const { return opt >= SB && opt <= SW; }
};

class ReorderBuffer {
public:
    inline void init(RegisterFile *registerFile_) {
        registerFile = registerFile_;
    }

    inline void clear() { nextBuffer.clear(); }

    inline void refresh() { buffer = nextBuffer; }

    inline bool full() { return buffer.full(); }

    void tryCommit();

    void add();

private:
    Queue<ROBEntry> buffer;
    Queue<ROBEntry> nextBuffer;
    RegisterFile *registerFile = nullptr;
};

void ReorderBuffer::tryCommit() {
    if (buffer.empty()) return;
    if (!buffer.front().ready) return;
    switch (OPTtype(buffer.front().opt)) {
        case InstructionType::MEM:
            break;
        case InstructionType::BRANCH:
            break;
        case InstructionType::REG:
            break;
        case InstructionType::END:
            break;
        default: //NUL
            throw 0;
    }
    nextBuffer.pop_front();
}

#endif //RISC_V_SIMULATOR_REORDER_BUFFER_H
