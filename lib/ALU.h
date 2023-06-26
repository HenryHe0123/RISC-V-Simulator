#ifndef RISC_V_SIMULATOR_ALU_H
#define RISC_V_SIMULATOR_ALU_H

class ALU {
public:
    template<class T>
    inline T add(const T &a, const T &b) { return a + b; }

    template<class T>
    inline T subtract(const T &a, const T &b) { return a - b; }

    template<class T>
    inline bool cmp(const T &a, const T &b) { return a < b; }

    template<class T>
    inline bool same(const T &a, const T &b) { return a == b; } //check if a == b

};

#endif //RISC_V_SIMULATOR_ALU_H
