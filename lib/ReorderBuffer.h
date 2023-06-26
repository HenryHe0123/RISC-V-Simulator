#ifndef RISC_V_SIMULATOR_REORDER_BUFFER_H
#define RISC_V_SIMULATOR_REORDER_BUFFER_H

#include <queue>
#include "decoder.h"

struct ROBEntry {
    bool ready = false;
    InstructionOPT opt;
};

class ReorderBuffer {

};

#endif //RISC_V_SIMULATOR_REORDER_BUFFER_H
