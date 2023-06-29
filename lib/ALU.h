#ifndef RISC_V_SIMULATOR_ALU_H
#define RISC_V_SIMULATOR_ALU_H

#include "decoder.h"

class ALU {
public:
    static unsigned compute(InstructionOPT opt, unsigned val1, unsigned val2) {
        switch (opt) {
            case LB:
            case LH:
            case LW:
            case LBU:
            case LHU:
            case SB:
            case SH:
            case SW:
            case ADDI:
            case ADD:
            case AUIPC:
                return val1 + val2;
            case JALR:
                return (val1 + val2) & ~1;
            case BEQ:
                return val1 == val2;
            case BNE:
                return val1 != val2;
            case SLTI:
            case SLT:
            case BLT:
                return int(val1) < int(val2);
            case BGE:
                return int(val1) >= int(val2);
            case SLTIU:
            case SLTU:
            case BLTU:
                return val1 < val2;
            case BGEU:
                return val1 >= val2;
            case XOR:
            case XORI:
                return val1 ^ val2;
            case OR:
            case ORI:
                return val1 | val2;
            case AND:
            case ANDI:
                return val1 & val2;
            case SLLI:
                return val1 << val2;
            case SRLI:
                return val1 >> val2;
            case SRAI:
                return int(val1) >> val2;
            case SUB:
                return val1 - val2;
            case SLL:
                return val1 << (val2 & 0b11111);
            case SRL:
                return val1 >> (val2 & 0b11111);
            case SRA:
                return int(val1) >> (val2 & 0b11111);
            default:
                return 19260817;
        }
    }

};

#endif //RISC_V_SIMULATOR_ALU_H
