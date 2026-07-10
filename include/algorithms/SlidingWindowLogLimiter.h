#pragma once

#include <string>
#include <unordered_map>

#include "../core/Config.h"
#include "../core/SlidingWindowData.h"

class SlidingWindowLogLimiter
{
public:
    explicit SlidingWindowLogLimiter(const Config& config);

    bool allowRequest(const std::string& clientId);

private:
    Config config_;

    std::unordered_map<std::string, SlidingWindowData> users_;
};