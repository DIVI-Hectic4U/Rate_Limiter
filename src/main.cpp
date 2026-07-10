#include <chrono>
#include <iostream>

#include "../include/algorithms/FixedWindowLimiter.h"
#include "../include/simulator/Simulator.h"

int main()
{
    Config config{
        .maxRequests = 5,
        .windowSize = std::chrono::seconds(60)
    };

    FixedWindowLimiter limiter(config);

    Simulator simulator(limiter);

    simulator.run();

    
    return 0;
}