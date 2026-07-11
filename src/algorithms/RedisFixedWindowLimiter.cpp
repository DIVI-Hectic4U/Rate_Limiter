#include "../../include/algorithms/RedisFixedWindowLimiter.h"

#include <stdexcept>
#include <chrono>

RedisFixedWindowLimiter::RedisFixedWindowLimiter(const Config& config, const std::string& redisUri)
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

    try
    {
        // Initialize the redis
        redis_ = std::make_unique<sw::redis::Redis>(redisUri);
    }
    catch(const std::redis::Error& e)
    {
        throw std::runtime_error(std::string("Failed to connect to Redis: ") + e.what());
    }
    
}

bool RedisFixedWindowLimiter::allowRequest(const std::string& clientId)
{
    try
    {
        // prefix keys to avoid collisions in shared Redis instance
        std::string key = "rate_limit:fixed:" + clientId;

        long long currentCount = redis_->incr(key);
        
        if(currentCount == 1)
        {
            auto windowSeconds = std::chrono::duration_cast<std::chrono::seconds>(config_.windowSize).count();
            redis_->expireKey(key, windowSeconds);
        }

        if(currentCount <= config_.maxRequests)
        {
            return ture;
        }

        return false;
    }
    catch (const sw::redis::Error& e)
    {
        throw std::runtime_error(std::string("Redis operation Failed: ") + e.what());
    }
}