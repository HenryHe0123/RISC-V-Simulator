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

    inline void reset() { issue_halt = issue_stall = false; }

    void issue();

private:
    ReorderBuffer *ROB = nullptr;
    ReservedStation *RS = nullptr;
    RAM *ram = nullptr;
    RegisterFile *regs = nullptr;
    bool issue_halt = false;
    bool issue_stall = false;

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
    if (issue_halt) {
        if (jalrBus.on) { //update pc and restart issue
            jalrBus.read(pc);
            issue_halt = false;
        }
        return;
    }
    if (ROB->full()) return;
    unsigned ins = ram->readU32(pc); //fetch
    Instruction instruction = Decoder::decode(ins); //decode
    ROBEntry robEntry(instruction);
    RSEntry rsEntry(instruction.opt);
    unsigned pre_pc = pc; //for recover use
    switch (OPTtype(instruction.opt)) {
        case NUL:
            pc += 4;
            return; //skip null instruction
        case MEM:
            //value(memory address) = x[rs1] + imm
            rsEntry.dest = ROB->add(robEntry);
            readRSEntryVal1(rsEntry, instruction);
            rsEntry.val2 = instruction.imm;
            if (!RS->addEntry(rsEntry)) { //try issue
                ROB->pop(); //RS fulled
                return; //issue false: pc keep unchanged
            }
            //issue success: set dependency if Load
            if (instruction.opt <= LHU)
                regs->aboutToWrite(instruction.rd, rsEntry.dest);
            pc += 4;
            return;
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
            if (!RS->addEntry(rsEntry)) { //try issue
                ROB->pop(); //RS fulled
                pc = pre_pc; //recover pc
                return;
            }
            return; //pc update already
        case REG:
            switch (instruction.opt) {
                case JAL:
                    regs->aboutToWrite(instruction.rd, ROB->add(robEntry));
                    pc += instruction.imm;
                    break;
                case LUI:
                    regs->aboutToWrite(instruction.rd, ROB->add(robEntry));
                    pc += 4;
                    break;
                case JALR:
                    rsEntry.dest = ROB->add(robEntry);
                    readRSEntryVal1(rsEntry, instruction);
                    rsEntry.val2 = instruction.imm;
                    if (!RS->addEntry(rsEntry)) { //try issue
                        ROB->pop(); //RS fulled
                        return; //issue false: pc keep unchanged
                    }
                    //issue success: set dependency and halt issue
                    regs->aboutToWrite(instruction.rd, rsEntry.dest);
                    issue_halt = true;
                    break;
                case AUIPC:
                    rsEntry.dest = ROB->add(robEntry);
                    rsEntry.val1 = pc;
                    rsEntry.val2 = instruction.imm;
                    if (!RS->addEntry(rsEntry)) { //try issue
                        ROB->pop(); //RS fulled
                        return; //issue false: pc keep unchanged
                    }
                    //issue success: set dependency and update pc
                    regs->aboutToWrite(instruction.rd, rsEntry.dest);
                    pc += 4;
                    break;
                default: //I-type or R-type
                    rsEntry.dest = ROB->add(robEntry);
                    readRSEntryVal1(rsEntry, instruction);
                    if (instruction.opt <= SRAI) rsEntry.val2 = instruction.imm; //I
                    else readRSEntryVal2(rsEntry, instruction); //R
                    if (!RS->addEntry(rsEntry)) { //try issue
                        ROB->pop(); //RS fulled
                        return; //issue false: pc keep unchanged
                    }
                    //issue success: set dependency and update pc
                    regs->aboutToWrite(instruction.rd, rsEntry.dest);
                    pc += 4;
                    break;
            }
            return;
        case END:
            ROB->add(robEntry);
            issue_stall = true;
    }
}

#endif //RISC_V_SIMULATOR_INSTRUCTION_UNIT_H
