#include "cpu.h"

CPU cpu;

int main() {
    //freopen("sample.data", "r", stdin);
    cpu.scan();
    cpu.process();
    return 0;
}
