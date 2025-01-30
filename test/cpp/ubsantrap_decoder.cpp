#include <csignal>
#include <cstdio>
#include <cstdint>
#include <unordered_set>
#include <unistd.h>

[[noreturn]] void aarch64_handler(int, siginfo_t* info, void*) {
    // Get the address of the trapped instruction from info->si_addr
    uint32_t* pc = (uint32_t*)info->si_addr;
    // Read the instruction at the trapped PC
    uint32_t instr = *pc;
    // Check if it is a `BRK` instruction (0xD4200000 mask)
    if ((instr & 0xFFE00000) == 0xD4200000) {
        // Mask out the opcode and extract the immediate (lower 16 bits)
        uint16_t cause = (instr >> 5) & 0xFF;
        printf("Caught ubsantrap with cause: %d\n", cause);
    }
    // Exit program after handling
    _exit(1);
}

[[noreturn]] void x86_handler(int, siginfo_t* info, void*) {
    std::unordered_set<uint8_t> prefixes({ 0xF0, 0xF2, 0xF3, 0x2E, 0x36, 0x3E, 0x26, 0x64, 0x65, 0x66, 0x67 });
    uint8_t* instr = (uint8_t*) info->si_addr;
    size_t pos = 0;
    // Skip Prefixes
    while (true) {
        if (prefixes.contains(instr[pos]) || (instr[pos] >= 0x40 && instr[pos] <= 0x4F)) {
            ++pos;
            continue;
        }
        break;
    }
    // Check if it is a `UD1` instruction (0F B9 opcode)
    if (instr[pos] == 0x0F || instr[pos + 1] == 0xB9) {
        pos += 3;  // Move past the opcode and ModR/M byte
        uint16_t cause = static_cast<uint8_t>(instr[pos]);
        printf("Caught ubsantrap with cause: %d\n", cause);
    }
    // Exit program after handling
    _exit(1);
}

void register_signal_handler() {
    struct sigaction sa;
    sa.sa_sigaction = x86_handler;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGTRAP, &sa, nullptr);
    sigaction(SIGILL, &sa, nullptr);
}

int main() {
    register_signal_handler();

    // Trigger UBSan trap manually
    // __asm__ volatile("brk #0x5504");
    __asm__ volatile("ud1 255(%eax), %eax");

    return 0;
}
