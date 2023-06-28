#ifndef RISC_V_SIMULATOR_INSTRUCTION_UNIT_H
#define RISC_V_SIMULATOR_INSTRUCTION_UNIT_H

#include "CDB.h"
#include "predictor.h"
#include "ReorderBuffer.h"
#include "ReservedStation.h"

class InstructionUnit {
public:
    inline void init(ReorderBuffer *ROB_, ReservedStation *RS_, RAM *ram_) {
        ROB = ROB_;
        RS = RS_;
        ram = ram_;
    }

    void issue();

private:
    ReorderBuffer *ROB = nullptr;
    ReservedStation *RS = nullptr;
    RAM *ram = nullptr;
};

void InstructionUnit::issue() {
    if (stall) return;
    if (ROB->full()) return;
    unsigned ins = ram->readU32(pc); //fetch
    Instruction instruction = Decoder::decode(ins); //decode
    ROBEntry robEntry(instruction);
    //todo
}

#endif //RISC_V_SIMULATOR_INSTRUCTION_UNIT_H
