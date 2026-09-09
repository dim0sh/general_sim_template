#ifndef DA_TIMING_H
#define DA_TIMING_H

typedef struct Timer {
    long long frequency;    // tick frequency
    long long t1;           // Interval start
    long long t2;           // Interval end
} timer_t;

typedef enum TimeResolution {
    S,  // seconds
    MS, // milliseconds
    US, // microseconds
    NS, // nanoseconds
} time_res_t;

void da_timer_init(timer_t *timer);
void da_timer_start(timer_t *timer);
void da_timer_end(timer_t *timer);
double da_timer_elapsed(timer_t *timer, time_res_t time_res);

#ifdef DA_WIN_TIMING_IMPLEMENTATION
#include <windows.h>

void da_timer_init(timer_t * timer) {
    LARGE_INTEGER f;
    QueryPerformanceFrequency(&f);
    timer->frequency = f.QuadPart;
}
void da_timer_start(timer_t * timer) {
    LARGE_INTEGER t1;
    QueryPerformanceCounter(&t1);
    timer->t1 = t1.QuadPart;
}
void da_timer_end(timer_t * timer) {
    LARGE_INTEGER t2;
    QueryPerformanceCounter(&t2);
    timer->t2 = t2.QuadPart;
}
// elapsed time in ms
double da_timer_elapsed(timer_t *timer, time_res_t time_res) {
    float time_factor = 1;
    switch (time_res) {
    case S:
        break;
    case MS:
        time_factor = 1e3;
        break;
    case US:
        time_factor = 1e6;
        break;
    case NS:
        time_factor = 1e9;
        break;
    }

    return (timer->t2 - timer->t1)*time_factor/timer->frequency;
}
#endif

#endif