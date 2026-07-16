#pragma once

#include <string>
#include "Config.h" // <-- Include your existing Config.h instead of defining it twice!

class IRateLimiter
{
public:
    virtual ~IRateLimiter() = default;

    virtual bool allowRequest(const std::string& clientId) = 0;
};