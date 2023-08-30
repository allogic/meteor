#include <stdlib.h>

#include <common/timer.h>

#ifdef OS_WINDOWS
#	include <windows.h>
#endif

struct xTimer_t {
#ifdef OS_WINDOWS
	LARGE_INTEGER wStartTime;
	LARGE_INTEGER wEndTime;
	LARGE_INTEGER wFrequency;
#endif
	float fElapsedTimePrev;
	float fElapsedTime;
	float fDeltaTime;
	float fTime;
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
}

float Timer_GetElapsedMicroSeconds(struct xTimer_t* pxTimer) {
#ifdef OS_WINDOWS
	return (float)((pxTimer->fElapsedTime * 1000000.0) / pxTimer->wFrequency.QuadPart);
#endif
}

float Timer_GetElapsedMilliSeconds(struct xTimer_t* pxTimer) {
#ifdef OS_WINDOWS
	return (float)((pxTimer->fElapsedTime * 1000.0) / pxTimer->wFrequency.QuadPart);
#endif
}

float Timer_GetElapsedSeconds(struct xTimer_t* pxTimer) {
#ifdef OS_WINDOWS
	return (float)((pxTimer->fElapsedTime) / pxTimer->wFrequency.QuadPart);
#endif
}

float Timer_GetDeltaTime(struct xTimer_t* pxTimer) {
#ifdef OS_WINDOWS
	return (float)((pxTimer->fDeltaTime) / pxTimer->wFrequency.QuadPart);
#endif
}

float Timer_GetTime(struct xTimer_t* pxTimer) {
#ifdef OS_WINDOWS
	return (float)((pxTimer->fTime) / pxTimer->wFrequency.QuadPart);
#endif
}

void Timer_Measure(struct xTimer_t* pxTimer) {
#ifdef OS_WINDOWS
	QueryPerformanceCounter(&pxTimer->wEndTime);
	pxTimer->fElapsedTimePrev = pxTimer->fElapsedTime;
	pxTimer->fElapsedTime = (float)(pxTimer->wEndTime.QuadPart - pxTimer->wStartTime.QuadPart);
	pxTimer->fDeltaTime = pxTimer->fElapsedTime - pxTimer->fElapsedTimePrev;
	pxTimer->fTime += pxTimer->fDeltaTime;
#endif
}
