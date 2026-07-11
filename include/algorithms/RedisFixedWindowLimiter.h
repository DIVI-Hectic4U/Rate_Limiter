#pragma once

#include <string>
#include <memory>
#include <sw/redis++/redis++.h>

#include "../core/Config.h"
#include "../core/IRateLimiter.h"

class RedisFixedWindowLimiter : public IRateLimiter {
public:
    // connection string (standard) ["tcp://127.0.0.1:6379"]
    RedisFixedWindowLimiter(const Config& config, const std::string& redisUri);
    bool allowRequest(const std::string& clientId) override;

private:
    Config config_;
    std::unique_ptr<sw::redis::Redis> redis_;
};