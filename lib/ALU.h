#ifndef RISC_V_SIMULATOR_ALU_H
#define RISC_V_SIMULATOR_ALU_H

class ALU {
public:
    template<class T>
    T add(const T &a, const T &b) { return a + b; }

    template<class T>
    T subtract(const T &a, const T &b) { return a - b; }

};

#endif //RISC_V_SIMULATOR_ALU_H
