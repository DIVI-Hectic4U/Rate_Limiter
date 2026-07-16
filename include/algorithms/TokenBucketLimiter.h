#pragma once

#include <string>
#include <unordered_map>
#include <chrono>
#include <mutex>

#include "../core/Config.h"
#include "../core/TokenBucketData.h"
#include "../core/IRateLimiter.h"

class TokenBucketLimiter : public IRateLimiter
{
public:
    explicit TokenBucketLimiter(const Config& config);
    bool allowRequest(const std::string& clientId) override;

private:
    Config config_;
    std::unordered_map<std::string,TokenBucketData> users_;
    double refillRatePerSecond_; // Pre-computed for performance
    std::mutex mutex_;
};