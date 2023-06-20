#ifndef RISC_V_SIMULATOR_REGISTER_H
#define RISC_V_SIMULATOR_REGISTER_H

class Register {
public:
    unsigned pc = 0;

    Register() = default;

    inline unsigned &operator[](int i) { return reg[i]; }

    inline void move_pc(unsigned offset) { pc += offset; }

    inline void assign_pc(unsigned addr) { pc = addr; }

    inline void update_pc() { pc += 4; }

private:
    unsigned reg[32]{0}; //reg[0] should always keep 0
};

#endif //RISC_V_SIMULATOR_REGISTER_H
