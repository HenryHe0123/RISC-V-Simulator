#ifndef RISC_V_SIMULATOR_INSTRUCTION_UNIT_H
#define RISC_V_SIMULATOR_INSTRUCTION_UNIT_H

#include "CDB.h"
#include "predictor.h"
#include "ReorderBuffer.h"
#include "ReservedStation.h"

class InstructionUnit {
public:
    inline void init(ReorderBuffer *ROB_, ReservedStation *RS_, RAM *ram_, RegisterFile *regs_) {
        ROB = ROB_;
        RS = RS_;
        ram = ram_;
        regs = regs_;
    }

    void issue();

private:
    ReorderBuffer *ROB = nullptr;
    ReservedStation *RS = nullptr;
    RAM *ram = nullptr;
    RegisterFile *regs = nullptr;

    inline void readRSEntryVal1(RSEntry &rsEntry, const Instruction &instruction) {
        Register tmp;
        regs->read(instruction.rs1, tmp);
        if (tmp.valid) rsEntry.val1 = tmp.value;
        else if (ROB->ready(tmp.tag)) ROB->valueRead(tmp.tag, rsEntry.val1);
        else rsEntry.Q1 = tmp.tag;
    } //read from register file

    inline void readRSEntryVal2(RSEntry &rsEntry, const Instruction &instruction) {
        Register tmp;
        regs->read(instruction.rs2, tmp);
        if (tmp.valid) rsEntry.val2 = tmp.value;
        else if (ROB->ready(tmp.tag)) ROB->valueRead(tmp.tag, rsEntry.val2);
        else rsEntry.Q2 = tmp.tag;
    }
};

void InstructionUnit::issue() {
    if (issue_stall) return;
    if (ROB->full()) return;
    unsigned ins = ram->readU32(pc); //fetch
    Instruction instruction = Decoder::decode(ins); //decode
    ROBEntry robEntry(instruction);
    RSEntry rsEntry(instruction.opt);
    switch (OPTtype(instruction.opt)) {
        case NUL:
            return; //skip null instruction
        case MEM:
            //value(memory address) = x[rs1] + imm
            rsEntry.dest = ROB->add(robEntry);
            readRSEntryVal1(rsEntry, instruction);
            rsEntry.val2 = instruction.imm;
            //set dependency
            if (!RS->addEntry(rsEntry)) { //issue
                ROB->pop(); //RS fulled
                return;
            }
            break;
        case BRANCH:
            //predict true (default)
            robEntry.value = pc + 4;
            pc += instruction.imm;
            robEntry.dest = pc;
            //now prepare to issue RSEntry
            rsEntry.dest = ROB->add(robEntry);
            readRSEntryVal1(rsEntry, instruction);
            readRSEntryVal2(rsEntry, instruction);
            //no need to set new dependency in BRANCH
            if (!RS->addEntry(rsEntry)) { //issue
                ROB->pop(); //RS fulled
                return;
            }
            break;
        case REG:
            switch (instruction.opt) {
                case JAL:

                case LUI:
                    ROB->add(robEntry);
                    break;
                default:
                    //todo
            }
            break;
        case END:
            ROB->add(robEntry);
    }


}

#endif //RISC_V_SIMULATOR_INSTRUCTION_UNIT_H
