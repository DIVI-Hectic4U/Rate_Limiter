#pragma once

#include <string>
#include <unordered_map>
#include <chrono>

#include "../core/Config.h"
#include "../core/LeakyBucketData.h"
#include "../core/IRateLimiter.h"

class LeakyBucketLimiter: public IRateLimiter {
public :
    explicit LeakyBucketLimiter(const Config& config);
    bool allowRequest(const std::string& clientId) override;

private:
    Config config_;
    std::unordered_map<std::string, LeakyBucketData> users_;
    double leakRatePerSecond_; // Pre-Computed for performance
};