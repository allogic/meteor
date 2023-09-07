#ifndef METEOR_RANDOM_H
#define METEOR_RANDOM_H

#include <stdint.h>

void XorShift128_Init(uint64_t wSeed);

int8_t XorShift128_Int8(int8_t cMin, int8_t cMax);
int16_t XorShift128_Int16(int16_t sMin, int16_t sMax);
int32_t XorShift128_Int32(int32_t nMin, int32_t nMax);
int64_t XorShift128_Int64(int64_t wMin, int64_t wMax);

float XorShift128_Float(float fMin, float fMax);
double XorShift128_Double(double dMin, double dMax);

#endif
