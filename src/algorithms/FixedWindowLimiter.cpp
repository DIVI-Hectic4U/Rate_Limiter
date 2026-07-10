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
    auto now = std::chrono::steady_clock::now();

    if (!storage_.contains(clientId))
    {
        storage_.insert(clientId, UserData{1, now});
        return true;
    }

    UserData* user = storage_.get(clientId);

    auto elapsed = now - user->windowStart;

    if (elapsed >= config_.windowSize)
    {
        *user = UserData{1, now};
        return true;
    }

    if (user->requestCount < config_.maxRequests)
    {
        ++user->requestCount;
        return true;
    }

    return false;
}