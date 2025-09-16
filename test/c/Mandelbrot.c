#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

static int mandelbrot(int size)
{
    int sum     = 0;
    int byteAcc = 0;
    int bitNum  = 0;

    int y = 0;

    while (y < size) {
        double ci = (2.0 * y / size) - 1.0;
        int x = 0;

        while (x < size) {
            double zr   = 0.0;
            double zrzr = 0.0;
            double zi   = 0.0;
            double zizi = 0.0;
            double cr = (2.0 * x / size) - 1.5;

            int z = 0;
            bool notDone = true;
            int escape = 0;
            while (notDone && z < 50) {
                zr = zrzr - zizi + cr;
                zi = 2.0 * zr * zi + ci;

                // preserve recalculation
                zrzr = zr * zr;
                zizi = zi * zi;

                if (zrzr + zizi > 4.0) {
                    notDone = false;
                    escape  = 1;
                }
                z += 1;
            }

            // printf("> %d | %d | %d\n", byteAcc, (byteAcc << 1), escape);
            byteAcc = (byteAcc << 1) + escape;
            bitNum += 1;

            // Code is very similar for these cases, but using separate blocks
            // ensures we skip the shifting when it's unnecessary, which is most cases.
            if (bitNum == 8) {
                sum ^= byteAcc;
                byteAcc = 0;
                bitNum  = 0;
            } else if (x == size - 1) {
                byteAcc <<= (8 - bitNum);
                sum ^= byteAcc;
                byteAcc = 0;
                bitNum  = 0;
            }
            x += 1;
        }
        printf("%d\n", sum);
        y += 1;
    }
    printf("\n%d\n", sum);
    return sum;
}

int main() {
    mandelbrot(750);
    return 0;
}
