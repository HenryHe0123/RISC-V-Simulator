#ifndef RISC_V_SIMULATOR_REORDER_BUFFER_H
#define RISC_V_SIMULATOR_REORDER_BUFFER_H

#include "CDB.h"

struct ROBEntry {
    bool ready = false; //turn true when result value come from RS
    bool predict_accurate = true; //clear pipeline when commit predict false
    bool predict = true; //predict result for BRANCH instruction
    InstructionOPT opt = NONE;
    unsigned dest = 0; //reg index for load/store or ALU operations
    unsigned value = 0; //result value, need compute
    //for BRANCH instruction, dest for predict pc, value for other pc branch
    //for MEM instruction, we would use value for memory address
    unsigned LS = 0; //for load/store instruction, LS = 2 and every cycle -1
    //instruction truly ready when ready = true and LS = 0 (register value prepared but not wrote)
    unsigned uhash = 0; //hash value of source instruction code

    [[nodiscard]] inline bool dest_for_reg() const { return OPTtype(opt) == REG || opt <= LHU && opt; }

    [[nodiscard]] inline bool dest_for_memory() const { return opt >= SB && opt <= SW; }

    ROBEntry() = default;

    explicit ROBEntry(const Instruction &ins) : opt(ins.opt), uhash(hash(ins.src)) {
        switch (OPTtype(opt)) {
            case NUL:
                ready = true;
                break;
            case MEM:
                LS = 2;
                if (opt <= LHU) dest = ins.rd; //load
                else dest = ins.rs2; //store
                break;
            case BRANCH:
                //not ready until compare result come out from RS and change predict_accurate
                break;
            case REG:
                dest = ins.rd;
                if (opt == JAL) {
                    value = pc + 4;
                    ready = true;
                } else if (opt == JALR) {
                    value = pc + 4;
                } else if (opt == LUI) {
                    value = ins.imm;
                    ready = true;
                }
                break;
            case END:
                ready = true;
                break;
        }
    }

    void show() const {
        std::cerr << "ROBEntry: opt" << opt << ", dest" << dest << '\n';
    } //for debug
};

class ReorderBuffer {
public:
    inline void init(RegisterFile *registerFile_, RAM *ram_) {
        registerFile = registerFile_;
        ram = ram_;
    }

    inline void clear() { nextBuffer.clear(); }

    inline void refresh() { buffer = nextBuffer; }

    inline bool full() { return buffer.full(); }

    void tryCommit();

    int add(const ROBEntry &entry) { //return ROB entry index
        nextBuffer.push_back(entry);
        return nextBuffer.back_index();
    }

    inline void pop() { nextBuffer.pop_back(); } //cancel add entry

    inline bool ready(int tag) { return tag >= 0 && buffer[tag].ready && buffer[tag].LS == 0; } //truly ready

    inline void valueRead(int tag, unsigned &uint) { if (tag >= 0) uint = buffer[tag].value; }

    inline ROBEntry &visitCur(int tag) { return buffer[tag]; }

    inline ROBEntry &visitNxt(int tag) { return nextBuffer[tag]; }

    void show() {
        std::cerr << "ROB curBuffer:\n";
        for (auto &entry: buffer) entry.show();
        std::cerr << "wait_time:" << wait_time << '\n';
    } //for debug

    [[nodiscard]] double predict_accuracy() const {
        int tot = predict_true_time + predict_false_time;
        if (tot) return double(predict_true_time) / tot;
        else return 1;
    }

private:
    Queue<ROBEntry> buffer;
    Queue<ROBEntry> nextBuffer;
    RegisterFile *registerFile = nullptr;
    RAM *ram = nullptr;

    void executeLoad(ROBEntry &entry) {
        unsigned value = ram->readU32(entry.value);
        switch (entry.opt) {
            case LB:
                value = Decoder::getPart(value, 7, 0);
                value = Decoder::sext(value, 7);
                break;
            case LH:
                value = Decoder::getPart(value, 15, 0);
                value = Decoder::sext(value, 15);
                break;
            case LW:
                value = Decoder::getPart(value, 31, 0);
                value = Decoder::sext(value, 31);
                break;
            case LBU:
                value = Decoder::getPart(value, 7, 0);
                break;
            case LHU:
                value = Decoder::getPart(value, 15, 0);
                break;
            default:
                return;
        }
        entry.value = value;
    }

    static void updatePredictor(const ROBEntry &entry) {
        if (OPTtype(entry.opt) != BRANCH) return;
        bool ans = (entry.predict_accurate == entry.predict);
        predictor.update(entry.uhash, ans);
    }

    int wait_time = 0;
    int predict_false_time = 0;
    int predict_true_time = 0;
};

void ReorderBuffer::tryCommit() {
    if (buffer.empty()) {
        ++wait_time;
        return;
    }
    const ROBEntry &entry = buffer.front();
    if (!entry.ready) {
        ++wait_time;
        return; //wait until ready
    }
    if (entry.LS) {
        --nextBuffer.front().LS; //wait until RAM visit finished
        if (nextBuffer.front().LS == 0 && entry.opt <= LHU && entry.opt) {
            executeLoad(nextBuffer.front()); //execute LOAD instruction immediately
        }
        ++wait_time;
        return;
    }
    //now we prepare to commit
    if (entry.opt) { //do nothing if NONE (error actually)
        if (entry.opt <= LHU) { //LOAD
            //same with REG, as MEM already load when update LS = 0
            registerFile->write(entry.dest, entry.value, buffer.front_index());
            CDB.broadcast(Pair{buffer.front_index(), entry.value});
        } else if (entry.opt <= SW) { //STORE
            unsigned storeVal = registerFile->regs[entry.dest].value;
            int highBit = 31; //SW
            if (entry.opt == SB) highBit = 7;
            if (entry.opt == SH) highBit = 15;
            storeVal = Decoder::getPart(storeVal, highBit, 0);
            ram->writeU32(entry.value, storeVal);
        } else if (entry.opt <= BGEU) { //BRANCH
            if (!entry.predict_accurate) { //predict wrong
                predictBus.broadcast(entry.value); //other(correct) pc branch
                predictBus.flush(); //flush immediately
                ++predict_false_time;
            } else ++predict_true_time;
            updatePredictor(entry);
        } else if (entry.opt <= AND) { //REG
            registerFile->write(entry.dest, entry.value, buffer.front_index());
            CDB.broadcast(Pair{buffer.front_index(), entry.value});
        } else { //HALT
            STALL = true;
        }
    }
    nextBuffer.pop_front(); //commit
}


#endif //RISC_V_SIMULATOR_REORDER_BUFFER_H
