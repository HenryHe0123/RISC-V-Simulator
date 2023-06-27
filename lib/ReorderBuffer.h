#ifndef RISC_V_SIMULATOR_REORDER_BUFFER_H
#define RISC_V_SIMULATOR_REORDER_BUFFER_H

#include "queue.h"
#include "decoder.h"

struct ROBEntry {
    bool ready = false;
    bool predict = false;
    InstructionOPT opt = NONE;
    unsigned dest = 0; //reg index for load or ALU operations, memory address for store
    unsigned value = 0; //result value

    [[nodiscard]] inline bool dest_for_reg() const { return OPTtype(opt) == REG || opt <= LHU && opt; }

    [[nodiscard]] inline bool dest_for_memory() const { return opt >= SB && opt <= SW; }
};

class ReorderBuffer {
public:
    inline void clear() { nextBuffer.clear(); }

    inline void refresh() { buffer = nextBuffer; }

    inline bool full() { return buffer.full(); }

    void tryCommit();

private:
    Queue<ROBEntry> buffer;
    Queue<ROBEntry> nextBuffer;
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
    nextBuffer.pop();
}

#endif //RISC_V_SIMULATOR_REORDER_BUFFER_H
