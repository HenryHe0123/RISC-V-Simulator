#ifndef RISC_V_SIMULATOR_DECODER_H
#define RISC_V_SIMULATOR_DECODER_H

enum InstructionOPT {
    NONE,
    //---------MEM----------//
    LB,     // I    Load Byte
    LH,     // I    Load Half Word
    LW,     // I    Load Word
    LBU,    // I    Load Byte Unsigned
    LHU,    // I    Load Half Word Unsigned
    SB,     // S    Store Byte
    SH,     // S    Store Half Word
    SW,     // S    Store Word
    //---------BRANCH----------//
    BEQ,    // B    Branch Equal
    BNE,    // B    Branch Not Equal
    BLT,    // B    Branch Less Than
    BGE,    // B    Branch Greater than or Equal
    BLTU,   // B    Branch Less than Unsigned
    BGEU,   // B    Branch Greater than or Equal Unsigned
    JAL,    // J    Jump & Link
    JALR,   // I    Jump & Link Register
    //---------REG------------//
    LUI,    // U    Load Upper Immediate
    AUIPC,  // U    Add Upper Immediate to PC
    ADDI,   // I    ADD Immediate
    SLTI,   // I    Set Less than Immediate
    SLTIU,  // I    Set Less than Immediate Unsigned
    XORI,   // I    XOR Immediate
    ORI,    // I    OR Immediate
    ANDI,   // I    AND Immediate
    SLLI,   // I    Shift Left Immediate
    SRLI,   // I    Shift Right Immediate
    SRAI,   // I    Shift Right Arith Immediate
    ADD,    // R    ADD
    SUB,    // R    Subtract
    SLL,    // R    Shift Left
    SLT,    // R    Set Less than
    SLTU,   // R    Set Less than Unsigned
    XOR,    // R    XOR
    SRL,    // R    Shift Right
    SRA,    // R    Shift Right Arithmetic
    OR,     // R    OR
    AND,    // R    AND
    //---------END-------------//
    HALT    // End of Main 0x0ff00513
};

enum InstructionType {
    NUL, MEM, BRANCH, REG, END
};

InstructionType OPTtype(InstructionOPT opt) {
    if (opt == 0) return NUL;
    else if (opt <= 8) return MEM;
    else if (opt <= 16) return BRANCH;
    else if (opt <= 37) return REG;
    else return END;
}

struct Instruction {
    InstructionOPT opt = NONE;
    unsigned rd = 0, rs1 = 0, rs2 = 0;
    unsigned imm = 0; //immediate number
    unsigned src = 0; //source code
};

class Decoder {
public:
    static Instruction decode(unsigned ins) {
        Instruction res;
        res.src = ins;
        if (ins == 0x0ff00513) {
            res.opt = HALT;
            return res;
        }
        unsigned opcode = getOpcode(ins);
        res.opt = getOPT(opcode, getFunc3(ins), getFunc7(ins));
        Type type = Types[res.opt];
        res.imm = getImm(type, ins);
        res.rd = getRd(type, ins);
        res.rs1 = getRs1(type, ins);
        res.rs2 = getRs2(type, ins);
        return res;
    }

private:
    inline static unsigned getPart(unsigned uint, int high, int low) { //0-based
        return (high == 31) ? uint >> low : (uint & (1u << (high + 1)) - 1) >> low;
    }

    inline static unsigned sext(unsigned uint, int top) { //if top digit 1, add 1
        return (uint & (1 << top)) ? uint | -(1 << top) : uint;
    }

    enum Type {
        R_type, I_type, S_type, B_type, U_type, J_type, Others
    };

    constexpr static Type Types[39] = {
            Others, I_type, I_type, I_type, I_type, I_type, S_type, S_type, S_type,
            B_type, B_type, B_type, B_type, B_type, B_type, J_type, I_type,
            U_type, U_type, I_type, I_type, I_type, I_type, I_type, I_type,
            I_type, I_type, I_type, R_type, R_type, R_type, R_type, R_type,
            R_type, R_type, R_type, R_type, R_type, Others
    };

    static unsigned getImm(const Type &type, unsigned ins) {
        unsigned imm;
        switch (type) {
            case R_type:
                return 0;
            case I_type: // 31:20 imm[11:0]
                imm = getPart(ins, 31, 20);
                imm = sext(imm, 11);
                return imm;
            case S_type: // 31:25 imm[11:5] 11:7 imm[4:0]
                imm = (getPart(ins, 31, 25) << 5)
                      + getPart(ins, 11, 7);
                imm = sext(imm, 11);
                return imm;
            case B_type: // 31:25 imm[12|10:5] 11:7 imm[4:1|11]
                imm = (getPart(ins, 31, 31) << 12)
                      + (getPart(ins, 7, 7) << 11)
                      + (getPart(ins, 30, 25) << 5)
                      + (getPart(ins, 11, 8) << 1);
                imm = sext(imm, 12);
                return imm;
            case U_type: // 31:12 imm[31:12]
                imm = getPart(ins, 31, 12) << 12;
                return imm;
            case J_type: // 31:12 imm[20|10:1|11|19:12]
                imm = (getPart(ins, 31, 31) << 20)
                      + (getPart(ins, 19, 12) << 12)
                      + (getPart(ins, 20, 20) << 11)
                      + (getPart(ins, 30, 21) << 1);
                imm = sext(imm, 20);
                return imm;
            default: //Others
                return 0;
        }
    }

    inline static unsigned getFunc3(unsigned ins) { return (ins >> 12) & 0b111; }

    inline static unsigned getFunc7(unsigned ins) { return (ins >> 25) & 0b1111111; }

    inline static unsigned getOpcode(unsigned ins) { return ins & 0b1111111; }

    static InstructionOPT getOPT(unsigned opcode, unsigned func3, unsigned func7) {
        switch (opcode) {
            case 0b0110111:
                return LUI;
            case 0b0010111:
                return AUIPC;
            case 0b1101111:
                return JAL;
            case 0b1100111:
                return JALR;
            case 0b1100011: {
                switch (func3) {
                    case 0b000:
                        return BEQ;
                    case 0b001:
                        return BNE;
                    case 0b100:
                        return BLT;
                    case 0b101:
                        return BGE;
                    case 0b110:
                        return BLTU;
                    case 0b111:
                        return BGEU;
                    default:
                        return NONE;
                }
            }
            case 0b0000011: {
                switch (func3) {
                    case 0b000:
                        return LB;
                    case 0b001:
                        return LH;
                    case 0b010:
                        return LW;
                    case 0b100:
                        return LBU;
                    case 0b101:
                        return LHU;
                    default:
                        return NONE;
                }
            }
            case 0b0100011: {
                switch (func3) {
                    case 0b000:
                        return SB;
                    case 0b001:
                        return SH;
                    case 0b010:
                        return SW;
                    default:
                        return NONE;
                }
            }
            case 0b0010011: {
                switch (func3) {
                    case 0b000:
                        return ADDI;
                    case 0b010:
                        return SLTI;
                    case 0b011:
                        return SLTIU;
                    case 0b100:
                        return XORI;
                    case 0b110:
                        return ORI;
                    case 0b111:
                        return ANDI;
                    case 0b001:
                        return SLLI;
                    case 0b101:
                        return (func7 != 0) ? SRAI : SRLI;
                    default:
                        return NONE;
                }
            }
            case 0b0110011: {
                switch (func3) {
                    case 0b000:
                        return (func7 != 0) ? SUB : ADD;
                    case 0b001:
                        return SLL;
                    case 0b010:
                        return SLT;
                    case 0b011:
                        return SLTU;
                    case 0b100:
                        return XOR;
                    case 0b101:
                        return (func7 != 0) ? SRA : SRL;
                    case 0b110:
                        return OR;
                    case 0b111:
                        return AND;
                    default:
                        return NONE;
                }
            }
            default:
                return NONE;
        }
    }

    inline static unsigned getRd(const Type &type, unsigned ins) {
        return (type == R_type || type == I_type || type == U_type || type == J_type) ? getPart(ins, 11, 7) : 0;
    }

    inline static unsigned getRs1(const Type &type, unsigned ins) {
        return (type == R_type || type == I_type || type == S_type || type == B_type) ? getPart(ins, 19, 15) : 0;
    }

    inline static unsigned getRs2(const Type &type, unsigned ins) {
        return (type == R_type || type == S_type || type == B_type) ? getPart(ins, 24, 20) : 0;
    }

};


#endif //RISC_V_SIMULATOR_DECODER_H
