#pragma once

#include <chrono>

struct TokenBucketData
{
    double tokens = 0.0;
    std::chrono::steady_clock::time_point lastRefill;
};