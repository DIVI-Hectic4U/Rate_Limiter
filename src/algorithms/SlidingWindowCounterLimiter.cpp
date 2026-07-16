#include "../../include/algorithms/SlidingWindowCounterLimiter.h"
#include <stdexcept>

SlidingWindowCounterLimiter::SlidingWindowCounterLimiter(const Config& config)
    : config_(config)
{
    if(config_.maxRequests <= 0)
    {
        throw std::invalid_argument("maxRequests must be greater than zero.");
    }

    if(config_.windowSize <= std::chrono::seconds(0))
    {
        throw std::invalid_argument("WindowSize must be greater than zero.");
    }
}

bool SlidingWindowCounterLimiter::allowRequest(const std::string& clientId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto now = std::chrono::steady_clock::now();

    auto it = users_.find(clientId);
    if(it == users_.end())
    {
        users_.emplace(clientId, SlidingWindowCounterData{0,1,now});
        return true;
    }

    SlidingWindowCounterData& user = it->second;

    auto windowMs = std::chrono::duration_cast<std::chrono::milliseconds>(config_.windowSize);
    
    auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - user.currWindowStart);

    if(elapsedMs >= windowMs)
    {
        long long windowsPassed = elapsedMs.count() / windowMs.count();

        if(windowsPassed == 1)
        {
            user.prevWindowCount = user.currWindowCount;
        }
        else
        {
            user.prevWindowCount = 0;
        }

        user.currWindowCount = 0;

        user.currWindowStart += std::chrono::milliseconds(windowsPassed * windowMs.count());

        elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - user.currWindowStart);
    }

    // Calculate the weighted estimate of requests
    double overlapPercentage = 1.0 - (static_cast<double>(elapsedMs.count()) / windowMs.count());
    double estimatedRequests = (user.prevWindowCount * overlapPercentage) + user.currWindowCount;

    if(estimatedRequests  < config_.maxRequests)
    {
        user.currWindowCount++;
        return true;
    }
    
    return false;
}