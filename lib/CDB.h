#ifndef RISC_V_SIMULATOR_CDB_H
#define RISC_V_SIMULATOR_CDB_H
//common data (bus) hpp
#include "queue.h"
#include "decoder.h"
#include "memory.h"
#include "register.h"

unsigned pc = 0;
unsigned CDB = 0;
unsigned nextCDB = 0;
bool issue_stall = false;
//global variable

void flushCDB() { CDB = nextCDB; }

#endif //RISC_V_SIMULATOR_CDB_H
