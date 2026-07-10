#pragma once

#include<string>

class IRateLimiter
{
public:
    virtual ~IRateLimiter() = default;

    virtual bool allowRequest(const std::string& clientId) = 0;
};