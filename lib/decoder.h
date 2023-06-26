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

enum InstructionType { NULL, MEM, BRANCH, REG, END};

InstructionType OPTtype(InstructionOPT opt) {
    if(opt == 0) return NULL;
    else if(opt <= 8) return MEM;
    else if(opt <= 16) return BRANCH;
    else if(opt <= 37) return REG;
    else return END;
}


#endif //RISC_V_SIMULATOR_DECODER_H
