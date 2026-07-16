#include "../../include/algorithms/TokenBucketLimiter.h"
#include <stdexcept>
#include <algorithm> 

TokenBucketLimiter::TokenBucketLimiter(const Config& config)
    : config_(config)
{
    if(config_.maxRequests <= 0)
    {
        throw std::invalid_argument("maxRequests must be greater than zero.");
    }

    if(config_.windowSize <= std::chrono::seconds(0))
    {
        throw std::invalid_argument("windowSize must be greater than zero.");
    }

    auto windowSeconds = std::chrono::duration_cast<std::chrono::seconds>(config_.windowSize).count();
    refillRatePerSecond_ = static_cast<double>(config_.maxRequests) / windowSeconds;
}

bool TokenBucketLimiter::allowRequest(const std::string& clientId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto now = std::chrono::steady_clock::now();

    auto it = users_.find(clientId);
    if(it == users_.end())
    {
        double initialTokens = static_cast<double>(config_.maxRequests) - 1.0;
        users_.emplace(clientId,TokenBucketData{initialTokens,now});
        return true;
    }

    TokenBucketData& user = it->second;

    std::chrono::duration<double> elapsedSeconds = now - user.lastRefillTime;

    double tokensToAdd = elapsedSeconds.count() * refillRatePerSecond_;

    if(tokensToAdd > 0.0)
    {
        user.tokens = std::min(static_cast<double>(config_.maxRequests),user.tokens + tokensToAdd);
        user.lastRefillTime = now;
    }

    if(user.tokens >= 1.0)
    {
        user.tokens -= 1.0;
        return true;
    }

    return false;
}