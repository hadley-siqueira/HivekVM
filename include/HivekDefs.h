#ifndef HIVEK_DEFS_H
#define HIVEK_DEFS_H

namespace HivekVM {
    const int N_REGISTERS = 32;
    const int STACK_DEFAULT_SIZE = 1024 * 1024;

    const int REG_V0 = 2;
    const int REG_V1 = 3;

    const int REG_A0 = 4;
    const int REG_A1 = 5;
    const int REG_A2 = 6;
    const int REG_A3 = 7;

    const int REG_T0 = 8;
    const int REG_T1 = 9;

    const int REG_SP = 29;
    const int REG_FP = 30;
    const int REG_RA = 31;

    const int OPCODE_ADD = 0x00;
    const int OPCODE_SUB = 0x01;
    const int OPCODE_AND = 0x02;
    const int OPCODE_OR  = 0x03;
    const int OPCODE_XOR = 0x04;
    const int OPCODE_JR = 0x05;

    const int OPCODE_ADDI = 0x01;

    const int OPCODE_LOAD_D = 0x02;
    const int OPCODE_STORE_D = 0x03;

    const int OPCODE_BEQ = 0x04;
    const int OPCODE_BNE = 0x05;
    const int OPCODE_BLT = 0x06;

    const int OPCODE_JAL = 0x07;

    const int OPCODE_SYSCALL = 0x7e;
    const int OPCODE_HALT = 0x7f;

    const int TYPE_RR = 0;
    const int TYPE_RI = 1;
    const int TYPE_J = 2;
}

#endif
