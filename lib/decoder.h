#ifndef RISC_V_SIMULATOR_DECODER_H
#define RISC_V_SIMULATOR_DECODER_H

enum OPT {
    NONE,
    //--------MEM----------//
    LB,     // I    Load Byte
    LH,     // I    Load Half Word
    LW,     // I    Load Word
    LBU,    // I    Load Byte Unsigned
    LHU,    // I    Load Half Word Unsigned
    SB,     // S    Store Byte
    SH,     // S    Store Half Word
    SW,     // S    Store Word
    //--------BRANCH----------//
    BEQ,    // SB   Branch Equal
    BNE,    // SB   Branch Not Equal
    BLT,    // SB   Branch Less Than
    BGE,    // SB   Branch Greater than or Equal
    BLTU,   // SB   Branch Less than Unsigned
    BGEU,   // SB   Branch Greater than or Equal Unsigned
    JAL,    // UJ   Jump & Link
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
    HALT
};



#endif //RISC_V_SIMULATOR_DECODER_H
