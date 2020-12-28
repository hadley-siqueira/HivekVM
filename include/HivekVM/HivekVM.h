#ifndef HIVEKVM_H
#define HIVEKVM_H

#include <cstdint>

namespace HivekVM {
    const int N_REGISTERS = 32;
    const int REG_SP = 31;

    class HivekVM {
    public:
        HivekVM();
        ~HivekVM();

    public:
        void reset();
        void shutdown();
        void load_program(char* path);

    private:
        uint32_t read32(uint64_t address);

    private:
        uint64_t regs[N_REGISTERS];
        uint64_t ip;
        uint32_t* program;
    };
}

#endif // HIVEKVM_H
