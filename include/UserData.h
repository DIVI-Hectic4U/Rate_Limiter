#pragma once

#include <chrono>

struct UserData{
    int requestCount;
    std::chrono::steady_clock::time_point windowStart;
};