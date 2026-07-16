#include <chrono>
#include <iostream>
#include <stdexcept>
#include <cstdlib>

#ifdef ENABLE_REDIS
#include "../include/algorithms/RedisFixedWindowLimiter.h"
#else
#include "../include/algorithms/FixedWindowLimiter.h"
#include "../include/algorithms/SlidingWindowLogLimiter.h"
#include "../include/algorithms/SlidingWindowCounterLimiter.h"
#include "../include/algorithms/TokenBucketLimiter.h"
#include "../include/algorithms/LeakyBucketLimiter.h"
#endif

#include "../include/core/IRateLimiter.h"
#include "../include/http/HttpServer.h"

int main()
{
    try
    {
        Config config{
            .maxRequests = 5,
            .windowSize = std::chrono::seconds(60)
        };

        // FixedWindowLimiter limiter(config);
        // SlidingWindowLogLimiter limiter(config);
        // SlidingWindowCounterLimiter limiter(config);
        // TokenBucketLimiter limiter(config);
        // LeakyBucketLimiter limiter(config);
#ifdef ENABLE_REDIS
        const char* redisUriEnv = std::getenv("REDIS_URI");
        std::string redisUri = redisUriEnv ? redisUriEnv : "tcp://127.0.0.1:6379";

        std::cout << "[Config] Distributed Redis Limiter Enabled. Connecting to " << redisUri << "...\n";
        RedisFixedWindowLimiter limiter(config, redisUri);
#else   
        std::cout << "[Config] Local Memory Limiter Enabled (Leaky Bucket). \n";
        LeakyBucketLimiter limiter(config);
#endif

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
