#ifndef TIMING_H
#define TIMING_H
// #include <windows.h>

typedef struct Timer {
    long long frequency;    // tick frequency
    long long t1;           // Interval start
    long long t2;           // Interval end
} timer_t;

typedef enum TimeResolution {
    S,  // seconds
    MS, // milliseconds
    US  // microseconds
} time_res_t;

void timer_init(timer_t *timer);
void timer_start(timer_t *timer);
void timer_end(timer_t *timer);
double timer_elapsed(timer_t *timer, time_res_t time_res);
#endif