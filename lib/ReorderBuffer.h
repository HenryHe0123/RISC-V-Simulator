#ifndef RISC_V_SIMULATOR_REORDER_BUFFER_H
#define RISC_V_SIMULATOR_REORDER_BUFFER_H

#include "CDB.h"

struct ROBEntry {
    bool ready = false; //turn true when result value come from RS
    bool predict = true; //clear pipeline when commit predict false
    InstructionOPT opt = NONE;
    unsigned dest = 0; //reg index for load/store or ALU operations
    unsigned value = 0; //result value, need compute
    //for BRANCH instruction, dest for predict pc, value for other pc branch
    //for MEM instruction, we would use value for memory address
    unsigned LS = 0; //for load/store instruction, LS = 2 and every cycle -1
    //instruction truly ready when ready = true and LS = 0 (register value prepared but not wrote)

    [[nodiscard]] inline bool dest_for_reg() const { return OPTtype(opt) == REG || opt <= LHU && opt; }

    [[nodiscard]] inline bool dest_for_memory() const { return opt >= SB && opt <= SW; }

    ROBEntry() = default;

    explicit ROBEntry(const Instruction &ins) : opt(ins.opt) { //won't init branch all
        switch (OPTtype(opt)) {
            case NUL:
                ready = true;
                break;
            case MEM:
                LS = 2;
                if (opt <= LHU) dest = ins.rd; //load
                else dest = ins.rs2; //store
                break;
            case BRANCH:
                //not ready until compare result come out from RS and change predict
                break;
            case REG:
                dest = ins.rd;
                if (opt == JAL) {
                    value = pc + 4;
                    ready = true;
                } else if (opt == JALR) {
                    value = pc + 4;
                } else if (opt == LUI) {
                    value = ins.imm;
                    ready = true;
                }
                break;
            case END:
                ready = true;
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

    int add(const ROBEntry &entry) { //return ROB entry index
        nextBuffer.push_back(entry);
        return nextBuffer.back_index();
    }

    inline void pop() { nextBuffer.pop_back(); } //cancel add entry

    inline bool ready(int tag) { return tag >= 0 && buffer[tag].ready && buffer[tag].LS == 0; } //truly ready

    inline void valueRead(int tag, unsigned &uint) { if (tag >= 0) uint = buffer[tag].value; }

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


#endif //RISC_V_SIMULATOR_REORDER_BUFFER_H
