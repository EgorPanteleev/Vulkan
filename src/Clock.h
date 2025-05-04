//
// Created by auser on 5/4/25.
//

#ifndef VULKAN_CLOCK_H
#define VULKAN_CLOCK_H

#include <chrono>

class Clock {
public:
    void start();

    void stop();

    double duration();
private:
    std::chrono::high_resolution_clock::time_point mStartTime;
    double mDuration;
};


#endif //VULKAN_CLOCK_H
