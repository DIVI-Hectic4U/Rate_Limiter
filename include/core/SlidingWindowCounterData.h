#pragma once

#include <chrono>

struct SlidingWindowCounterData
{
    int prevWindowCount = 0;
    int currWindowCount = 0;
    std::chrono::steady_clock::time_point currWindowStart;
};