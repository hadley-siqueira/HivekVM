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
        regs[i] = 0;
    }

    stack = new uint8_t[STACK_DEFAULT_SIZE];
    regs[REG_SP] = (uint64_t) stack + STACK_DEFAULT_SIZE - 8;

    imap[0x04190001] = "    addi %25, %0, 1";
    imap[0x04040005] = "    addi a0, %0, 5";
    imap[0x1c000005] = "    jal fib";
    imap[0x0000007f] = "    halt";
    imap[0x00000000] = "    add %0, %0, %0";
    imap[0x00000000] = "    add %0, %0, %0";
    imap[0x00000000] = "    add %0, %0, %0";
    imap[0x18990003] = "    blt %25, a0, recurse";
    imap[0x00801000] = "    add v0, a0, %0";
    imap[0x00000005] = "    jr";
    imap[0x07bdffe8] = "    addi sp, sp, -24";
    imap[0x0fa00000] = "    sd ra, sp, 0";
    imap[0x0fa40008] = "    sd a0, sp, 8";
    imap[0x0484ffff] = "    addi a0, a0, -1";
    imap[0x1ffffff9] = "    jal fib";
    imap[0x0fa20010] = "    sd v0, sp, 16";
    imap[0x0ba40008] = "    ld a0, sp, 8";
    imap[0x0484fffe] = "    addi a0, a0, -2";
    imap[0x1ffffff5] = "    jal fib";
    imap[0x0ba80010] = "    ld t0, sp, 16";
    imap[0x00481000] = "    add v0, v0, t0";
    imap[0x0ba00000] = "    ld ra, sp, 0";
    imap[0x07bd0018] = "    addi sp, sp, 24";
    imap[0x00000005] = "    jr ";
}

HivekVM::HivekVM::~HivekVM() {
    delete[] program;
    delete[] stack;
}

void HivekVM::HivekVM::run() {
    while (true) {
        fetch();
        execute();
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
    int sz = file.tellg();
    file.seekg(0, std::ios::beg);

    program = new uint8_t[sz];
    ptr = program;
    ip = (uint64_t) program;

    for (int i = 0; i < sz; ++i) {
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

void HivekVM::HivekVM::print_stack() {
    printf("stack:\n");

    for (int i = 0; i < 20; ++i) {
        uint64_t addr = (uint64_t) stack + STACK_DEFAULT_SIZE - 8 - i * 8;
        uint64_t v = read64(addr);
        printf("sp[0x%016" PRIx64 "] = 0x%016" PRIx64 "\n", addr, v);
    }
}

void HivekVM::HivekVM::fetch() {
    instruction = read32u(ip);
    printf("instruction = %08x %s\n", instruction, imap[instruction].c_str());
}

void HivekVM::HivekVM::execute() {
    int ikind = (instruction >> 29) & 0x7;

    switch (ikind) {
    case 0b000:
    case 0b001:
    case 0b010:
    case 0b011:
    case 0b100:
    case 0b101:
        execute16();
        break;

    case 0b110:
        execute24();
        break;

    case 0b111:
        execute32();
        break;

    default:
        printf("invalid ikind\n");
        exit(0);
        break;
    }

    // better to always set to zero than to make a lot of if else. Its cheaper
    regs[0] = 0;
}

void HivekVM::HivekVM::execute16() {
    printf("invalid 16 bit instruction\n");
    exit(0);
}

void HivekVM::HivekVM::execute24() {
    int opcode = (instruction >> 23) & 0x3f;
    int ra = (instruction >> 18) & 0x1f;
    int rb = (instruction >> 13) & 0x1f;
    int rc = (instruction >> 8) & 0x1f;

    switch (opcode) {
    case OPCODE_24_ADD:
        regs[rc] = regs[ra] + regs[rb];
        ip += 3;
        break;

    case OPCODE_24_SUB:
        regs[rc] = regs[ra] - regs[rb];
        ip += 3;
        break;

    case OPCODE_24_AND:
        regs[rc] = regs[ra] & regs[rb];
        ip += 3;
        break;

    case OPCODE_24_OR:
        regs[rc] = regs[ra] | regs[rb];
        ip += 3;
        break;

    case OPCODE_24_XOR:
        regs[rc] = regs[ra] ^ regs[rb];
        ip += 3;
        break;
    }
}

void HivekVM::HivekVM::execute32() {
    int opcode = (instruction >> 23) & 0x3f;
    int ra;
    int rb;
    int rc;
    uint64_t immd13;
    uint64_t immd23;

    // jal
    if (opcode == 0b111010 || opcode == 0b111011) {

    } else {
        ra = (instruction >> 18) & 0x1f;
        rb = (instruction >> 13) & 0x1f;
        rc = (instruction >> 8) & 0x1f;
        immd13 = instruction & 0x1000 ? 0xffffffffffffe000 | (instruction & 0x1fff) : instruction & 0x1fff;

        switch (opcode) {
        case OPCODE_32_ADDI:
            regs[rb] = regs[ra] + immd13;
            ip += 4;
            break;

        case OPCODE_32_ANDI:
            regs[rb] = regs[ra] & immd13;
            ip += 4;
            break;

        case OPCODE_32_ORI:
            regs[rb] = regs[ra] | immd13;
            ip += 4;
            break;

        case OPCODE_32_XORI:
            regs[rb] = regs[ra] ^ immd13;
            ip += 4;
            break;

        // memory
        case OPCODE_32_LD:
            regs[rb] = read64(regs[ra] + immd13);
            ip += 4;
            break;

        case OPCODE_32_SD:
            write64(regs[ra] + immd13, regs[rb]);
            ip += 4;
            break;

        // branches
        case OPCODE_32_BEQ:
            if (regs[ra] == regs[rb]) {
                ip = ip + (int64_t) immd13;
            } else {
                ip += 4;
            }

            break;

        case OPCODE_32_BNE:
            if (regs[ra] != regs[rb]) {
                ip = ip + (int64_t) immd13;
            } else {
                ip += 4;
            }

            break;

        case OPCODE_32_BLT:
            if ((int64_t) regs[rb] < (int64_t) regs[ra]) {
                ip = ip + (int64_t) immd13;
            } else {
                ip += 4;
            }

            break;

        // jumps
        case OPCODE_32_JAL:
            immd23 = instruction & 0x400000 ? 0xffffffffff800000 | (instruction & 0x7fffff) : instruction & 0x7fffff;
            regs[REG_RA] = ip + 4;
            ip = ip + (int64_t) immd23;
            break;


        default:
            break;
        }
    }
}

void HivekVM::HivekVM::execute_rr() {
    int opcode = instruction & 0x07ff;
    int ra = get_ra();
    int rb = get_rb();
    int rc = get_rc();
    int tmp;

    switch (opcode) {
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
}

uint32_t HivekVM::HivekVM::read32u(uint64_t address) {
    uint32_t* ptr = (uint32_t*) address;
    return ntohl(*ptr);
}

uint64_t HivekVM::HivekVM::read64(uint64_t address) {
    uint64_t* ptr = (uint64_t*) address;
    uint64_t value = *ptr;

    value = ((value & 0xFF00000000000000ull) >> 56)
             | ((value & 0x00FF000000000000ull) >> 40)
             | ((value & 0x0000FF0000000000ull) >> 24)
             | ((value & 0x000000FF00000000ull) >>  8)
             | ((value & 0x00000000FF000000ull) <<  8)
             | ((value & 0x0000000000FF0000ull) << 24)
             | ((value & 0x000000000000FF00ull) << 40)
             | ((value & 0x00000000000000FFull) << 56);

    //printf("HERE READ v = %016" PRIx64 "; addr = %016" PRIx64 "\n", value, address);
    return value;
}

void HivekVM::HivekVM::write32(uint64_t address, uint32_t data) {
    uint32_t* ptr = (uint32_t*) address;

    *ptr = htonl(data);
}

void HivekVM::HivekVM::write64(uint64_t address, uint64_t value) {
    uint64_t* ptr = (uint64_t*) address;

    //printf("werbf v = %016" PRIx64 "\n", value);
    value = ((value & 0xFF00000000000000ull) >> 56)
             | ((value & 0x00FF000000000000ull) >> 40)
             | ((value & 0x0000FF0000000000ull) >> 24)
             | ((value & 0x000000FF00000000ull) >>  8)
             | ((value & 0x00000000FF000000ull) <<  8)
             | ((value & 0x0000000000FF0000ull) << 24)
             | ((value & 0x000000000000FF00ull) << 40)
             | ((value & 0x00000000000000FFull) << 56);

    *ptr = value; //printf("HERE WRITE v = %016" PRIx64 "; addr = %016" PRIx64 "\n", value, address);
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
        res = res << 16;
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


