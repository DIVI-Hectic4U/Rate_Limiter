#pragma once

#include <chrono>

struct FixedWindowData
{
    int requestCount = 0;
    std::chrono::steady_clock::time_point windowStart;
};