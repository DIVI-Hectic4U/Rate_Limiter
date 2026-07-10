#include <chrono>
#include <iostream>

#include "../include/algorithms/FixedWindowLimiter.h"

int main()
{
    Config config{
        .maxRequests = 5,
        .windowSize = std::chrono::seconds(60)
    };

    FixedWindowLimiter limiter(config);

    std::cout << std::boolalpha;

    for(int i = 1 ; i <= 7; i++){
        std::cout
            << "Requests "
            << i
            << " : "
            << limiter.allowRequest("Alice")
            << '\n';
    }

    return 0;
}