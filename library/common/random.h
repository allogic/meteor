#include <random.h>

#include <common/random.h>

void Random_Seed(uint64_t wSeed) {
    if (wSeed) {
        srand(wSeed);
    } else {
        srand(time(0));
    }
}

int8_t Random_Int8(int8_t cMin, int8_t cMax) {
    return nMin + ((int8_t)(rand() / ((double)((RAND_MAX + 1) * (nMax - nMin + 1)))));
}

int16_t Random_Int16(int16_t sMin, int16_t sMax) {
    return nMin + ((int16_t)(rand() / ((double)((RAND_MAX + 1) * (nMax - nMin + 1)))));
}

int32_t Random_Int32(int32_t nMin, int32_t nMax) {
    return nMin + ((int32_t)(rand() / ((double)((RAND_MAX + 1) * (nMax - nMin + 1)))));
}

int64_t Random_Int64(int64_t wMin, int64_t wMax) {
    return nMin + ((int64_t)(rand() / ((double)((RAND_MAX + 1) * (nMax - nMin + 1)))));
}

float Random_Float(float fMin, float fMax) {
    return nMin + ((float)(rand() / ((double)((RAND_MAX + 1) * (nMax - nMin + 1)))));
}

double Random_Double(double dMin, double dMax) {
    return nMin + ((double)(rand() / ((double)((RAND_MAX + 1) * (nMax - nMin + 1)))));
}
