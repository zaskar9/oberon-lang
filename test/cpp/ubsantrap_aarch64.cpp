#include <csignal>
#include <cstdio>
#include <cstdint>
#include <unistd.h>

void sigtrap_handler(int sig, siginfo_t* info, void* context) {
    // mach_port_t thread = mach_thread_self(); // Get current thread

    // Get the address of the trapped instruction from info->si_addr
    uint32_t* pc = (uint32_t*)info->si_addr;

    // Read the instruction at the trapped PC
    uint32_t instr = *pc;

    // Check if it is a BRK instruction (0xD4200000 mask)
    if ((instr & 0xFFE00000) == 0xD4200000) {
        // Mask out the opcode and extract the immediate (lower 16 bits)
        uint16_t imm16 = (instr >> 5) & 0xFFFF;
        printf("Caught ubsantrap with value: %d\n", imm16);
        uint8_t cause = imm16 & 0xFF;
        printf("Caught ubsantrap with cause: %d\n", cause);
    } else {
        printf("Unknown illegal instruction at %p\n", pc);
    }

    _exit(1); // Exit program after handling
}

void register_signal_handler() {
    struct sigaction sa;
    sa.sa_sigaction = sigtrap_handler;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGTRAP, &sa, nullptr);
    sigaction(SIGILL, &sa, nullptr);
}

int main() {
    register_signal_handler();

    // Trigger UBSan trap manually
    __asm__ volatile("brk #0x5504");

    return 0;
}
