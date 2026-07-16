#pragma once

#include <string>
#include "Config.h" 

class IRateLimiter
{
public:
    virtual ~IRateLimiter() = default;

    virtual bool allowRequest(const std::string& clientId) = 0;
};