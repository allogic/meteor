#include <stdlib.h>
#include <stdint.h>

#include <common/timer.h>

#ifdef OS_WINDOWS
#	include <windows.h>
#endif

#ifdef OS_LINUX
#	include <time.h>
#endif

struct xTimer_t {
#ifdef OS_WINDOWS
	LARGE_INTEGER wStartTime;
	LARGE_INTEGER wEndTime;
	LARGE_INTEGER wFrequency;
#endif
#ifdef OS_LINUX
	struct timespec xStartTime;
	struct timespec xEndTime;
#endif
	double dElapsedTimePrev;
	double dElapsedTime;
	double dDeltaTime;
};

struct xTimer_t* Timer_Alloc(void) {
	return calloc(1, sizeof(struct xTimer_t));
}

void Timer_Free(struct xTimer_t* pxTimer) {
	free(pxTimer);
}

void Timer_Start(struct xTimer_t* pxTimer) {
#ifdef OS_WINDOWS
	QueryPerformanceFrequency(&pxTimer->wFrequency); 
	QueryPerformanceCounter(&pxTimer->wStartTime);
#endif
#ifdef OS_LINUX
	clock_gettime(CLOCK_MONOTONIC_RAW, &pxTimer->xStartTime);
#endif
}

double Timer_GetElapsedMicroSeconds(struct xTimer_t* pxTimer) {
#ifdef OS_WINDOWS
	return (pxTimer->dElapsedTime * 1000000.0) / pxTimer->wFrequency.QuadPart;
#endif
#ifdef OS_LINUX
	return (pxTimer->dElapsedTime) / 1000.0;
#endif
}

double Timer_GetElapsedMilliSeconds(struct xTimer_t* pxTimer) {
#ifdef OS_WINDOWS
	return (pxTimer->dElapsedTime * 1000.0) / pxTimer->wFrequency.QuadPart;
#endif
#ifdef OS_LINUX
	return (pxTimer->dElapsedTime) / 1000000.0;
#endif
}

double Timer_GetElapsedSeconds(struct xTimer_t* pxTimer) {
#ifdef OS_WINDOWS
	return (pxTimer->dElapsedTime) / pxTimer->wFrequency.QuadPart;
#endif
#ifdef OS_LINUX
	return (pxTimer->dElapsedTime) / 1000000000.0;
#endif
}

double Timer_GetDeltaTime(struct xTimer_t* pxTimer) {
#ifdef OS_WINDOWS
	return (pxTimer->dDeltaTime) / pxTimer->wFrequency.QuadPart;
#endif
#ifdef OS_LINUX
	return (pxTimer->dDeltaTime) / 1.0;
#endif
}

double Timer_GetTime(struct xTimer_t* pxTimer) {
#ifdef OS_WINDOWS
	return (pxTimer->dElapsedTime) / pxTimer->wFrequency.QuadPart;
#endif
#ifdef OS_LINUX
	return (pxTimer->dElapsedTime) / 1.0;
#endif
}

void Timer_Measure(struct xTimer_t* pxTimer) {
#ifdef OS_WINDOWS
	QueryPerformanceCounter(&pxTimer->wEndTime);
	pxTimer->dElapsedTimePrev = pxTimer->dElapsedTime;
	pxTimer->dElapsedTime = (double)(pxTimer->wEndTime.QuadPart - pxTimer->wStartTime.QuadPart);
#endif
#ifdef OS_LINUX
	clock_gettime(CLOCK_MONOTONIC_RAW, &pxTimer->xEndTime);
	int64_t wStartTime = (pxTimer->xStartTime.tv_sec * 1000000000LL) + pxTimer->xStartTime.tv_nsec;
	int64_t wEndTime = (pxTimer->xEndTime.tv_sec * 1000000000LL) + pxTimer->xEndTime.tv_nsec;
	pxTimer->dElapsedTimePrev = pxTimer->dElapsedTime;
	pxTimer->dElapsedTime = (double)(wEndTime - wStartTime);
#endif
	pxTimer->dDeltaTime = pxTimer->dElapsedTime - pxTimer->dElapsedTimePrev;
}
