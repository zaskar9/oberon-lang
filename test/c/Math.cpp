#include <numbers>
#include <cinttypes>
#include <cmath>
#include <cstdint>
#include <cstdio>

int main() {
    float pif = std::numbers::pi_v<float>;
    union {
        float f;
        uint32_t u;
    } f2u = { .f = pif };

    printf("pi : %40.38f\n   : 0x%" PRIx32 "\n", pif, f2u.u);

    double pi = std::numbers::pi;
    union {
        double f;
        uint64_t u;
    } d2u = { .f = pi };

    printf("pi : %40.38f\n   : 0x%" PRIx64 "\n", pi, d2u.u);

    return 0;
}