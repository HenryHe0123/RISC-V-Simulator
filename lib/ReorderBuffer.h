#ifndef RISC_V_SIMULATOR_REORDER_BUFFER_H
#define RISC_V_SIMULATOR_REORDER_BUFFER_H

#include "CDB.h"

struct ROBEntry {
    bool ready = false;
    bool predict = true; //clear pipeline when commit predict false
    InstructionOPT opt = NONE;
    unsigned dest = 0; //reg index for load/store or ALU operations
    unsigned value = 0; //result value, need compute
    //for BRANCH instruction, dest for predict pc, value for other pc branch
    //for Store instruction, we would use value for memory address
    unsigned LS = 0; //for load/store instruction, LS = 2 and every cycle -1
    //for JALR

    [[nodiscard]] inline bool dest_for_reg() const { return OPTtype(opt) == REG || opt <= LHU && opt; }

    [[nodiscard]] inline bool dest_for_memory() const { return opt >= SB && opt <= SW; }

    ROBEntry() = default;

    explicit ROBEntry(const Instruction &ins) : opt(ins.opt) { //won't init branch all
        switch (OPTtype(opt)) {
            case NUL:
                break;
            case MEM:
                if (opt <= LHU) dest = ins.rd; //load
                else dest = ins.rs2; //store
                break;
            case BRANCH:
                LS = 2;
                break;
            case REG:
                break;
            case END:
                break;
        }
    }
};

class ReorderBuffer {
public:
    inline void init(RegisterFile *registerFile_, RAM *ram_) {
        registerFile = registerFile_;
        ram = ram_;
    }

    inline void clear() { nextBuffer.clear(); }

    inline void refresh() { buffer = nextBuffer; }

    inline bool full() { return buffer.full(); }

    void tryCommit();

    int add(const ROBEntry &entry); //return ROB entry index

private:
    Queue<ROBEntry> buffer;
    Queue<ROBEntry> nextBuffer;
    RegisterFile *registerFile = nullptr;
    RAM *ram = nullptr;
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

int ReorderBuffer::add(const ROBEntry &entry) {
    nextBuffer.push_back(entry);
    if (OPTtype(entry.opt) == REG) {
        registerFile->aboutToWrite(entry.dest, nextBuffer.back_index());
    }
    return nextBuffer.back_index();
}

#endif //RISC_V_SIMULATOR_REORDER_BUFFER_H
