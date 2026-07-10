#pragma once

#include<chrono>

struct Config{
    int maxRequests;
    std::chrono::seconds windowSize;
};