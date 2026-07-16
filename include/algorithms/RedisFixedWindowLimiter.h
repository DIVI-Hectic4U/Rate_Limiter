#pragma once

#include <string>
#include <memory>
#include <chrono>
#include <sw/redis++/redis++.h>

#include "../core/IRateLimiter.h"

class RedisFixedWindowLimiter : public IRateLimiter {
public:
    // Because we included IRateLimiter.h above, 'Config' is now recognized instantly!
    explicit RedisFixedWindowLimiter(const Config& config, const std::string& redisUri = "tcp://127.0.0.1:6379");
    ~RedisFixedWindowLimiter() override;

    bool allowRequest(const std::string& clientId) override;

private:
    Config config_;
    std::unique_ptr<sw::redis::Redis> redis_;
};