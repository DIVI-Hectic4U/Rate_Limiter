#include <chrono>
#include <iostream>
#include <stdexcept>

#include "../include/algorithms/FixedWindowLimiter.h"
#include "../include/algorithms/SlidingWindowLogLimiter.h"
#include "../include/algorithms/SlidingWindowCounterLimiter.h"
#include "../include/algorithms/TokenBucketLimiter.h"
#include "../include/algorithms/LeakyBucketLimiter.h"
#include "../include/algorithms/RedisFixedWindowLimiter.h"
#include "../include/http/HttpServer.h"

int main()
{
    try
    {
        Config config{
            .maxRequests = 5,
            .windowSize = std::chrono::seconds(60)};

        // FixedWindowLimiter limiter(config);
        // SlidingWindowLogLimiter limiter(config);
        // SlidingWindowCounterLimiter limiter(config);
        // TokenBucketLimiter limiter(config);
        // LeakyBucketLimiter limiter(config);

        std::cout << "Connecting to Redis cluster...\n";
        RedisFixedWindowLimiter limiter(config, "tcp://127.0.0.1:6379");

        std::cout << "Starting HTTP Server...\n";
        HttpServer server(limiter);

        server.start();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Critical Server Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
