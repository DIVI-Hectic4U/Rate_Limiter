#pragma once
#include <chrono>

struct Config {
    int maxRequests{5};
    std::chrono::milliseconds windowSize{std::chrono::seconds(60)};
};