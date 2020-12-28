#include "HivekVM/HivekVM.h"

using namespace HivekVM;


HivekVM::HivekVM::HivekVM() {

}

HivekVM::HivekVM::~HivekVM() {

}

void HivekVM::HivekVM::shutdown() {

}

void HivekVM::HivekVM::load_program(char *path) {

}

uint32_t HivekVM::HivekVM::read32(uint64_t address) {
    uint32_t* ptr = (uint32_t*) address;
    return *ptr;
}
