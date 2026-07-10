#pragma once

#include <chrono>
#include <deque>

struct SlidingWindowData
{
    std::deque<std::chrono::steady_clock::time_point> requests;
};