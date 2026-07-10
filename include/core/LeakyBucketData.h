#pragma once

#include <chrono>

struct LeakyBucketData
{
    int queuedRequests = 0;
    std::chrono::steady_clock::time_point lastLeakTime;
};