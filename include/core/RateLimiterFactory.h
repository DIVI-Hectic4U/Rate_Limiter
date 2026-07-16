#pragma once

#include <memory>
#include <string>

#include "Config.h"
#include "IRateLimiter.h"


//   1. Open/Closed Principle — adding a new algorithm only requires
//      a new case in the factory, not changes to main() or HttpServer.
//   2. Decouples main.cpp from concrete limiter headers.
//   3. Enables Docker/CI to switch algorithms via env vars without recompilation.

class RateLimiterFactory
{
public:
    // Supported algorithm names:
    //   "fixed_window"           — Fixed Window Counter
    //   "token_bucket"           — Token Bucket (AWS/Stripe style)
    //   "leaky_bucket"           — Leaky Bucket (constant drain)
    //   "sliding_window_log"     — Sliding Window Log (exact timestamps)
    //   "sliding_window_counter" — Sliding Window Counter (weighted estimate)
    //   "redis_fixed_window"     — Redis-backed Fixed Window (distributed)
    //
    // redisUri is only used when algorithm == "redis_fixed_window".
    // Throws std::invalid_argument for unrecognized algorithm names.
    
    static std::unique_ptr<IRateLimiter> create(
        const std::string& algorithm,
        const Config& config,
        const std::string& redisUri = "tcp://127.0.0.1:6379");
};
