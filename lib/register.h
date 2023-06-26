#ifndef RISC_V_SIMULATOR_REGISTER_H
#define RISC_V_SIMULATOR_REGISTER_H

#include <cstring>

extern const int regSize = 32;

class RegisterFile;

class Register {
    friend class RegisterFile;

    friend void updateRegister(Register &, const RegisterFile &);

public:
    unsigned pc = 0;

    Register() = default;

    //inline unsigned &operator[](int i) { return reg[i]; }

    inline void move_pc(unsigned offset) { pc += offset; }

    inline void assign_pc(unsigned addr) { pc = addr; }

    inline void update_pc() { pc += 4; }

private:
    unsigned reg[regSize]{0}; //reg[0] should always keep 0
};

class RegisterFile {
public:
    bool valid[regSize]{};
    unsigned reg[regSize]{0};
    unsigned tag[regSize]{0};

    RegisterFile() { memset(valid, -1, sizeof(valid)); };

    explicit RegisterFile(const Register &r) {
        memset(valid, -1, sizeof(valid));
        memcpy(reg, r.reg, sizeof(reg));
    }

    inline void reinit(const Register &r) {
        memset(valid, -1, sizeof(valid));
        memcpy(reg, r.reg, sizeof(reg));
    }
};

inline void updateRegister(Register &r, const RegisterFile &regFile) {
    memcpy(r.reg, regFile.reg, sizeof(r.reg));
}

#endif //RISC_V_SIMULATOR_REGISTER_H
