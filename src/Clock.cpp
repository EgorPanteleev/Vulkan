//
// Created by auser on 5/4/25.
//

#include "Clock.h"

void Clock::start() {
    mStartTime = std::chrono::high_resolution_clock::now();
}

void Clock::stop() {
    auto stopTime = std::chrono::high_resolution_clock::now();
    mDuration = std::chrono::duration<double>( stopTime - mStartTime ).count();
}

double Clock::duration() {
    return mDuration;
}
