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

        // Atomic Lua script: INCR + EXPIRE in a single Redis round-trip.
        static const std::string LUA_SCRIPT = R"(
            local current = redis.call('INCR', KEYS[1])
            if current == 1 then
                redis.call('EXPIRE', KEYS[1], ARGV[1])
            end
            return current
        )";

        auto result = redis_->eval<long long>(
            LUA_SCRIPT,
            {key},                                   // KEYS[1]
            {std::to_string(windowSeconds)}           // ARGV[1]
        );

        if (result <= config_.maxRequests)
        {
            return true;
        }

        return false;
    }
    catch (const sw::redis::Error& e)
    {
        std::cerr << "[Redis Error] " << e.what() << ". Denying request for safety.\n";
        return false;
    }
}