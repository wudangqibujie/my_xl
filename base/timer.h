//
// Created by jay on 2022/12/31.
//

#ifndef MY_XL_TIMER_H
#define MY_XL_TIMER_H

#include <chrono>
#include "common.h"

//------------------------------------------------------------------------------
// We can use the Timer class like this:
//
//   Timer timer();
//   timer.tic();
//
//     .... /* code we want to evaluate */
//
//   float time = timer.toc();  // (sec)
//
// This class can be used to evaluate multi-thread code.
//------------------------------------------------------------------------------
class Timer {
public:
    Timer();
    // Reset start time
    void reset();
    // Code start
    void tic();
    // Code end
    float toc();
    // Get the time duration
    float get();

protected:
    std::chrono::high_resolution_clock::time_point begin;
    std::chrono::milliseconds duration;

};

#endif //MY_XL_TIMER_H
