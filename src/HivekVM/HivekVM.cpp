#include <iostream>
#include <cstdio>
#include <fstream>
#include <inttypes.h>
#include <arpa/inet.h>

#include "HivekVM/HivekVM.h"

using namespace HivekVM;

HivekVM::HivekVM::HivekVM() {
    program = nullptr;
    ip = 0;

    for (int i = 0; i < N_REGISTERS; ++i) {
        regs[i] = i;
    }

    stack = new uint8_t[STACK_DEFAULT_SIZE];
    regs[REG_SP] = (uint64_t) stack + STACK_DEFAULT_SIZE - 8;
}

HivekVM::HivekVM::~HivekVM() {
    delete[] program;
    delete[] stack;
}

void HivekVM::HivekVM::run() {
    while (true) {
        fetch_instruction();
        execute_instruction();
    }
}

void HivekVM::HivekVM::shutdown() {
    delete[] program;
    program = nullptr;
}

void HivekVM::HivekVM::load_program(const char* path) {
    uint8_t* ptr;
    std::ifstream file(path, std::ios::binary);

    file.seekg(0, std::ios::end);
    int sz = file.tellg() / sizeof(uint32_t);
    file.seekg(0, std::ios::beg);

    program = new uint32_t[sz];
    ptr = (uint8_t*) program;
    ip = (uint64_t) program;

    for (int i = 0; i < sz * sizeof(uint32_t); ++i) {
        *ptr = file.get();
        ++ptr;
    }

    file.close();
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

void HivekVM::HivekVM::fetch_instruction() {
    instruction = read32u(ip);
    printf("instruction = %08x\n", instruction);
}

void HivekVM::HivekVM::execute_instruction() {
    int opcode = get_opcode();

    switch (opcode) {
    case 0:
        execute_rr();
        break;

    default:
        execute_ri();
        break;
    }
}

void HivekVM::HivekVM::execute_rr() {
    int opcode = instruction & 0x07ff;
    int ra = get_ra();
    int rb = get_rb();
    int rc = get_rc();
    int tmp;

    switch (opcode) {
    case OPCODE_ADD:
        regs[rc] = regs[ra] + regs[rb];
        ip += 4;
        break;

    case OPCODE_SUB:
        regs[rc] = regs[ra] - regs[rb];
        ip += 4;
        break;

    case OPCODE_AND:
        regs[rc] = regs[ra] & regs[rb];
        ip += 4;
        break;

    case OPCODE_OR:
        regs[rc] = regs[ra] | regs[rb];
        ip += 4;
        break;

    case OPCODE_XOR:
        regs[rc] = regs[ra] ^ regs[rb];
        ip += 4;
        break;

    case OPCODE_JR:
        ip = regs[REG_RA];
        break;

    case OPCODE_HALT:
        printf("vm halted...\n");
        print_registers();
        scanf("%i", &tmp);
        ip += 4;
        break;

    case OPCODE_SYSCALL:
        printf("%li\n", regs[1]);
        ip += 4;
        break;

    default:
        break;
    }

    regs[0] = 0;
}

void HivekVM::HivekVM::execute_ri() {
    int opcode = get_opcode();
    int ra = get_ra();
    int rb = get_rb();
    uint64_t immd16 = get_immd16();
    uint64_t immd26 = get_immd26();

    switch (opcode) {
    case OPCODE_ADDI:
        regs[rb] = regs[ra] + immd16;
        ip += 4;
        break;

    case OPCODE_LOAD_D:
        regs[rb] = read64(regs[ra] + immd16);
        ip += 4;
        break;

    case OPCODE_STORE_D:
        write64(regs[ra] + immd16, regs[rb]);
        ip += 4;
        break;

    case OPCODE_BEQ:
        if (regs[ra] == regs[rb]) {
            ip = ip + (int64_t) immd16 * 4;
        } else {
            ip += 4;
        }

        break;

    case OPCODE_BNE:
        if (regs[ra] != regs[rb]) {
            ip = ip + (int64_t) immd16 * 4;
        } else {
            ip += 4;
        }

        break;

    case OPCODE_BLT:
        ip = ip;
        printf("blt %i %i %i\n", (int64_t) regs[rb], (int64_t) regs[ra], (int64_t) regs[rb] < (int64_t) regs[ra]);
        if ((int64_t) regs[rb] < (int64_t) regs[ra]) {
            ip = ip + (int64_t) immd16 * 4;
        } else {
            ip += 4;
        }

        break;

    case OPCODE_JAL:
        regs[REG_RA] = ip + 4;
        ip = ip + (int64_t) immd26 * 4;
        break;

    default:
        break;
    }

    regs[0] = 0;
}

uint32_t HivekVM::HivekVM::read32u(uint64_t address) {
    uint32_t* ptr = (uint32_t*) address;
    return ntohl(*ptr);
}

uint64_t HivekVM::HivekVM::read64(uint64_t address) {
    uint64_t* ptr = (uint64_t*) address;
    uint64_t value = *ptr;

    return  ((value & 0xFF00000000000000ull) >> 56)
             | ((value & 0x00FF000000000000ull) >> 40)
             | ((value & 0x0000FF0000000000ull) >> 24)
             | ((value & 0x000000FF00000000ull) >>  8)
             | ((value & 0x00000000FF000000ull) <<  8)
             | ((value & 0x0000000000FF0000ull) << 24)
             | ((value & 0x000000000000FF00ull) << 40)
             | ((value & 0x00000000000000FFull) << 56);
}

void HivekVM::HivekVM::write32(uint64_t address, uint32_t data) {
    uint32_t* ptr = (uint32_t*) address;

    *ptr = htonl(data);
}

void HivekVM::HivekVM::write64(uint64_t address, uint64_t value) {
    uint64_t* ptr = (uint64_t*) address;

    value = ((value & 0xFF00000000000000ull) >> 56)
             | ((value & 0x00FF000000000000ull) >> 40)
             | ((value & 0x0000FF0000000000ull) >> 24)
             | ((value & 0x000000FF00000000ull) >>  8)
             | ((value & 0x00000000FF000000ull) <<  8)
             | ((value & 0x0000000000FF0000ull) << 24)
             | ((value & 0x000000000000FF00ull) << 40)
             | ((value & 0x00000000000000FFull) << 56);

    *ptr = value;
}

int HivekVM::HivekVM::get_opcode() {
    return (instruction >> 26) & 0x3f;
}

int HivekVM::HivekVM::get_ra() {
    return (instruction >> 21) & 0x1f;
}

int HivekVM::HivekVM::get_rb() {
    return (instruction >> 16) & 0x1f;
}

int HivekVM::HivekVM::get_rc() {
    return (instruction >> 11) & 0x1f;
}

uint64_t HivekVM::HivekVM::get_immd16() {
    uint64_t res = 0;

    if (instruction & 0x08000) {
        res = ~0;
        res << 16;
        res = res | (instruction & 0x0ffff);
    } else {
        res = instruction & 0x0ffff;
    }

    return res;
}

uint64_t HivekVM::HivekVM::get_immd26() {
    uint64_t res = 0;

    if (instruction & (1 << 25)) {
        res = ~0;
        res = res << 26;
        res = res | (instruction & 0x3ffffff);
    } else {
        res = instruction & 0x3ffffff;
    }

    return res;
}


