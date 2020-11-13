#include <stdint.h>

// Differential decoder
class FengyunDiff
{
private:
    unsigned char Xin_1, Yin_1, Xin, Yin, Xout, Yout;
    char inBuf = 0;    // Counter used at the beggining
    uint8_t buffer[2]; // Smaller buffer for internal use

public:
    int work(uint8_t *in, int len, uint8_t *out);
};