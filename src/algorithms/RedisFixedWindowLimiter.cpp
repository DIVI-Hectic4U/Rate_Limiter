#include "../../include/algorithms/RedisFixedWindowLimiter.h"
#include <iostream>
#include <stdexcept>
#include <chrono>

RedisFixedWindowLimiter::RedisFixedWindowLimiter(const Config& config, const std::string& redisUri)
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

    try 
    {
        redis_ = std::make_unique<sw::redis::Redis>(redisUri);
    }
    catch (const sw::redis::Error& e) // 1. Fixed: sw::redis::Error
    {
        throw std::runtime_error(std::string("Failed to connect to Redis: ") + e.what());
    }
}

RedisFixedWindowLimiter::~RedisFixedWindowLimiter() = default;

bool RedisFixedWindowLimiter::allowRequest(const std::string& clientId)
{
    try 
    {
        auto now = std::chrono::system_clock::now();
        auto windowSeconds = std::chrono::duration_cast<std::chrono::seconds>(config_.windowSize).count();
        auto currentWindow = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count() / windowSeconds;

        std::string key = "rate_limit:" + clientId + ":" + std::to_string(currentWindow);

        // Atomically increment the counter in Redis
        // TODO(Phase2): Replace INCR+EXPIRE with a Lua script to make key
        // creation atomic. If the server crashes between INCR and EXPIRE,
        // the key persists forever with no TTL (ghost entry).
        long long currentRequests = redis_->incr(key);

        if (currentRequests == 1)
        {
            // 2. Fixed: expire instead of expireKey
            redis_->expire(key, std::chrono::seconds(windowSeconds));
        }

        if (currentRequests <= config_.maxRequests)
        {
            return true; // 3. Fixed: true instead of ture
        }

        return false;
    }
    catch (const sw::redis::Error& e)
    {
        std::cerr << "[Redis Error] " << e.what() << ". Denying request for safety.\n";
        return false;
    }
}