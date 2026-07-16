#include "../../include/core/RateLimiterFactory.h"

#include <stdexcept>
#include <iostream>

// Include all in-memory algorithm headers
#include "../../include/algorithms/FixedWindowLimiter.h"
#include "../../include/algorithms/TokenBucketLimiter.h"
#include "../../include/algorithms/LeakyBucketLimiter.h"
#include "../../include/algorithms/SlidingWindowLogLimiter.h"
#include "../../include/algorithms/SlidingWindowCounterLimiter.h"

// Redis algorithm is conditionally compiled
#ifdef ENABLE_REDIS
#include "../../include/algorithms/RedisFixedWindowLimiter.h"
#endif

std::unique_ptr<IRateLimiter> RateLimiterFactory::create(
    const std::string& algorithm,
    const Config& config,
    const std::string& redisUri)
{
    std::cout << "[Factory] Creating rate limiter: " << algorithm << "\n";

    if (algorithm == "fixed_window")
    {
        return std::make_unique<FixedWindowLimiter>(config);
    }
    else if (algorithm == "token_bucket")
    {
        return std::make_unique<TokenBucketLimiter>(config);
    }
    else if (algorithm == "leaky_bucket")
    {
        return std::make_unique<LeakyBucketLimiter>(config);
    }
    else if (algorithm == "sliding_window_log")
    {
        return std::make_unique<SlidingWindowLogLimiter>(config);
    }
    else if (algorithm == "sliding_window_counter")
    {
        return std::make_unique<SlidingWindowCounterLimiter>(config);
    }
#ifdef ENABLE_REDIS
    else if (algorithm == "redis_fixed_window")
    {
        return std::make_unique<RedisFixedWindowLimiter>(config, redisUri);
    }
#endif
    else
    {
        throw std::invalid_argument(
            "Unknown rate limiter algorithm: '" + algorithm + "'. "
            "Supported: fixed_window, token_bucket, leaky_bucket, "
            "sliding_window_log, sliding_window_counter"
#ifdef ENABLE_REDIS
            ", redis_fixed_window"
#endif
        );
    }
}
