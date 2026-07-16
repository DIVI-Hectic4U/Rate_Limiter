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
    
    static std::unique_ptr<IRateLimiter> create(
        const std::string& algorithm,
        const Config& config,
        const std::string& redisUri = "tcp://127.0.0.1:6379");
};
