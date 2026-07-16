#include "../../include/algorithms/FixedWindowLimiter.h"

#include <stdexcept>

FixedWindowLimiter::FixedWindowLimiter(const Config& config)
    : config_(config)
{
    if (config_.maxRequests <= 0)
    {
        throw std::invalid_argument("maxRequests must be greater than zero.");
    }

    if (config_.windowSize <= std::chrono::seconds(0))
    {
        throw std::invalid_argument("windowSize must be greater than zero.");
    }
}

bool FixedWindowLimiter::allowRequest(const std::string& clientId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto now = std::chrono::steady_clock::now();

    auto it = users_.find(clientId);

    if(it == users_.end())
    {
        users_.emplace(clientId, FixedWindowData{1,now});
        return true;
    }
    
    FixedWindowData& user = it->second;
    auto elapsed = now - user.windowStart;

    if (elapsed >= config_.windowSize)
    {
        user = FixedWindowData{1,now};
        return true;
    }

    if (user.requestCount < config_.maxRequests)
    {
        ++user.requestCount;
        return true;
    }

    return false;
}