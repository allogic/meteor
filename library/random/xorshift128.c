#include <random/xorshift128.h>

#define U64_TO_DOUBLE(X) ((X >> 11) * 0x1.0p-53)

static uint64_t s_awState[2];

static uint64_t XorShift128_Next(void) {
	uint64_t wS0 = s_awState[0];
	uint64_t wS1 = s_awState[1];
	uint64_t wRes = wS0 + wS1;

	wS1 ^= wS0;

	s_awState[0] = ((wS0 << 24) | (wS0 >> 40)) ^ wS1 ^ (wS1 << 16);
	s_awState[1] = (wS1 << 37) | (wS1 >> 27);

	return wRes;
}

void XorShift128_Init(uint64_t wSeed) {
	s_awState[0] = wSeed;
	s_awState[1] = 0x6C0789654321E684;
}

int8_t XorShift128_Int8(int8_t cMin, int8_t cMax) {
	return cMin + ((int8_t)XorShift128_Next()) * (cMax - cMin);
}

int16_t XorShift128_Int16(int16_t sMin, int16_t sMax) {
	return sMin + ((int16_t)XorShift128_Next()) * (sMax - sMin);
}

int32_t XorShift128_Int32(int32_t nMin, int32_t nMax) {
	return nMin + ((int32_t)XorShift128_Next()) * (nMax - nMin);
}

int64_t XorShift128_Int64(int64_t wMin, int64_t wMax) {
	return wMin + ((int64_t)XorShift128_Next()) * (wMax - wMin);
}

float XorShift128_Float(float fMin, float fMax) {
	return fMin + ((float)U64_TO_DOUBLE(XorShift128_Next())) * (fMax - fMin);
}

double XorShift128_Double(double dMin, double dMax) {
	return dMin + ((double)U64_TO_DOUBLE(XorShift128_Next())) * (dMax - dMin);
}
