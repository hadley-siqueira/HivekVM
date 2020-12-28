#include <iostream>

#include "HivekVM/HivekVM.h"

int main(int argc, char* argv[]) {
    HivekVM::HivekVM vm;

    vm.load_program("prog.bin");
    vm.print_registers();

    return 0;
}
