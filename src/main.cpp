#include <iostream>

#include "HivekVM/HivekVM.h"

int main(int argc, char* argv[]) {
    HivekVM::HivekVM vm;

    vm.load_program("out.bin");
    vm.run();
    vm.print_registers();

    return 0;
}
