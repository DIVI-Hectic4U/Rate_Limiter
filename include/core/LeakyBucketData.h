#pragma once

#include <chrono>

struct LeakyBucketData
{
    double queuedRequests = 0.0;
    std::chrono::steady_clock::time_point lastLeakTime;
};