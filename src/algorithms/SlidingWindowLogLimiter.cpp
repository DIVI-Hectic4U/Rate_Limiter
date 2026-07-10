#include "../../include/algorithms/SlidingWindowLogLimiter.h"

SlidingWindowLogLimiter::SlidingWindowLogLimiter(const Config& config)
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
}

bool SlidingWindowLogLimiter::allowRequest(const std::string& clientId)
{
    auto now = std::chrono::steady_clock::now();

    auto& user = users_[clientId];

    while(!user.requests.empty())
    {
        auto timeSinceRequest = now - user.requests.front();
        if(timeSinceRequest >= config_.windowSize)
        {
            user.requests.pop_front();
        }
        else
        {
            break;
        }
    }

    if(user.requests.size() < static_cast<size_t>(config_.maxRequests))
    {
        user.requests.push_back(now);
        return true;
    }

    return false;
}
