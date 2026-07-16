#pragma once

#include <string>
#include <unordered_map>
#include <chrono>
#include <mutex>

#include "../core/Config.h"
#include "../core/SlidingWindowCounterData.h"
#include "../core/IRateLimiter.h"

class SlidingWindowCounterLimiter : public IRateLimiter {
public:
    explicit SlidingWindowCounterLimiter(const Config& config);
    bool allowRequest(const std::string& clientId) override;

private:
    Config config_;
    std::unordered_map<std::string,SlidingWindowCounterData> users_;
    std::mutex mutex_;
};