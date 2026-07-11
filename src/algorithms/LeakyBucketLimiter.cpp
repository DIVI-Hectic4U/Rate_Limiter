#include "../../include/algorithms/LeakyBucketLimiter.h"
#include <stdexcept>
#include <algorithm>
#include <chrono>


LeakyBucketLimiter::LeakyBucketLimiter(const Config& config)
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
    leakRatePerSecond_ = static_cast<double>(config_.maxRequests) / windowSeconds;
}

bool LeakyBucketLimiter::allowRequest(const std::string& clientId)
{
    //------THREAD SAFETY--------
    std::lock_guard<std::mutex> lock(mutex_);
    auto now = std::chrono::steady_clock::now();

    auto it = users_.find(clientId);
    if(it == users_.end())
    {
        users_.emplace(clientId,LeakyBucketData{1.0,now});
        return true;
    }

    LeakyBucketData& user = it->second;

    // 1. Leak old requests out of bucket
    std::chrono::duration<double> elapsedSeconds = now - user.lastLeakTime;
    double leakedRequests = elapsedSeconds.count() * leakRatePerSecond_;

    if(leakedRequests > 0.0)
    {
        //Subtract leaked requests, but the queue can't go below 0
        user.queuedRequests = std::max(0.0,user.queuedRequests - leakedRequests);
        user.lastLeakTime = now;
    }

    // 2. Check if we have room in the bucket for 1 more request
    if(user.queuedRequests + 1.0 <= config_.maxRequests)
    {
        user.queuedRequests += 1.0;
        return true;
    }

    //Bucket is Full
    return false;
}