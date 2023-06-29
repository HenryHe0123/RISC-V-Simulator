#ifndef RISC_V_SIMULATOR_CPU_H
#define RISC_V_SIMULATOR_CPU_H

#include <iostream>
#include <string>
#include "lib/CDB.h"
#include "lib/InstructionUnit.h"

using std::string;
using std::cin;
using std::stoi;

class CPU {
public:
    CPU() {
        initCommonData();
        reorderBuffer.init(&registerFile, &ram);
        reservedStation.init(&reorderBuffer);
        instructionUnit.init(&reorderBuffer, &reservedStation, &ram, &registerFile);
    }

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
        while (!STALL && cycle < 3000000) {
            ++cycle;
            instructionUnit.issue();
            reservedStation.execute();
            reorderBuffer.tryCommit();
            //show_detail();
            flush();
        }
        //show_detail();
        std::cout << int(registerFile.a0());
    }

private:
    unsigned cycle = 0;
    RAM ram;
    RegisterFile registerFile;
    InstructionUnit instructionUnit;
    ReservedStation reservedStation;
    ReorderBuffer reorderBuffer;

    void flush() {
        if (predictBus.on) { //predict false
            registerFile.clear();
            reservedStation.clear();
            reorderBuffer.clear();
            instructionUnit.reset();
            pc = predictBus.cur;
        }
        registerFile.refresh();
        reservedStation.refresh();
        reorderBuffer.refresh();
        flushBus();
    }

    void show_detail() {
        std::cerr << "cycle " << cycle << '\n';
        reorderBuffer.show();
        std::cerr << '\n';
    }

};


#endif //RISC_V_SIMULATOR_CPU_H
