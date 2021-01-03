#ifndef HIVEKVM_H
#define HIVEKVM_H

#include <cstdint>
#include <map>

#include "HivekDefs.h"

namespace HivekVM {
    class HivekVM {
    public:
        HivekVM();
        ~HivekVM();

    public:
        void run();
        void reset();
        void shutdown();
        void load_program(const char *path);

        void print_registers();
        void print_stack();

    private:
        void fetch();
        void execute();
        void execute16();
        void execute24();
        void execute32();
        void execute_rr();
        void execute_ri();

        // memory access
        uint32_t read32u(uint64_t address);
        uint64_t read64(uint64_t address);

        void write32(uint64_t address, uint32_t data);
        void write64(uint64_t address, uint64_t value);

        int get_opcode();
        int get_ra();
        int get_rb();
        int get_rc();
        uint64_t get_immd16();
        uint64_t get_immd26();



    private:
        uint64_t regs[N_REGISTERS];
        uint64_t ip;

        uint32_t instruction;
        uint8_t* program;
        uint8_t* stack;

        std::map<int, std::string> imap;
    };
}

#endif // HIVEKVM_H
