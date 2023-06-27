#ifndef RISC_V_SIMULATOR_CPU_H
#define RISC_V_SIMULATOR_CPU_H

#include "lib/memory.h"
#include "lib/register.h"
#include <iostream>
#include <string>

using std::string;
using std::cin;
using std::stoi;

class CPU {
public:
    unsigned cycle = 0;

    CPU() = default;

    void scan() {
        string line;
        int addr = 0;
        while (getline(cin, line)) {
            if (line[0] == '@') addr = stoi(line.substr(1), nullptr, 16);
            else
                for (int i = 0; i < line.size(); i += 3)  //read a 16-based number
                    ram.writeByte(addr++, stoi(line.substr(i, 2), nullptr, 16));
        }
    }

    void process() {
        bool flag = true;
        while (flag) {
            issue();
            execute();
            flag = commit();
            ++cycle;
            flush();
        }
    }

private:
    RAM ram;

    void issue();

    void execute();

    bool commit();

    void flush();

};


#endif //RISC_V_SIMULATOR_CPU_H
