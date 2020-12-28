#include <iostream>
#include <cstdio>
#include <inttypes.h>

#include "HivekVM/HivekVM.h"

using namespace HivekVM;

HivekVM::HivekVM::HivekVM() {
    program = nullptr;
    ip = 0;

    for (int i = 0; i < N_REGISTERS; ++i) {
        regs[i] = i;
    }

    // 8 * 5 is a random number just to avoid segfault on accessing stack on the last address
    stack = new uint8_t[STACK_DEFAULT_SIZE + 8 * 5];
    regs[REG_SP] = (uint64_t) stack + STACK_DEFAULT_SIZE - 1;
}

HivekVM::HivekVM::~HivekVM() {
    delete[] program;
    delete[] stack;
}

void HivekVM::HivekVM::shutdown() {
    delete[] program;
    program = nullptr;
}

void HivekVM::HivekVM::load_program(char* path) {
    if (program != nullptr) {
        delete[] program;
    }

    program = new uint32_t[1024 * 1024 / sizeof(uint32_t)];
    ip = (uint64_t) program;
}

void HivekVM::HivekVM::print_registers() {
    printf("ip = 0x%016" PRIx64 "\n", ip);

    for (int i = 0; i < N_REGISTERS; i += 4) {
        printf("r[%02i] = 0x%016" PRIx64 "    ", i, regs[i]);
        printf("r[%02i] = 0x%016" PRIx64 "    ", i + 1, regs[i + 1]);
        printf("r[%02i] = 0x%016" PRIx64 "    ", i + 2, regs[i + 2]);
        printf("r[%02i] = 0x%016" PRIx64 "\n", i + 3, regs[i + 3]);
    }
}

uint32_t HivekVM::HivekVM::read32(uint64_t address) {
    uint32_t* ptr = (uint32_t*) address;
    return *ptr;
}
