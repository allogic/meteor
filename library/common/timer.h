#ifndef METEOR_COMMON_TIMER_H
#define METEOR_COMMON_TIMER_H

struct xTimer_t;

struct xTimer_t* Timer_Alloc(void);
void Timer_Free(struct xTimer_t* pxTimer);

double Timer_GetElapsedMicroSeconds(struct xTimer_t* pxTimer);
double Timer_GetElapsedMilliSeconds(struct xTimer_t* pxTimer);
double Timer_GetElapsedSeconds(struct xTimer_t* pxTimer);
double Timer_GetDeltaTime(struct xTimer_t* pxTimer);
double Timer_GetTime(struct xTimer_t* pxTimer);

void Timer_Start(struct xTimer_t* pxTimer);
void Timer_Measure(struct xTimer_t* pxTimer);

#endif
