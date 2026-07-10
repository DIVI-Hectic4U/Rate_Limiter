#pragma once

#include <string>
#include <unordered_map>
#include <chrono>

#include "../core/Config.h"
#include "../core/FixedWindowData.h"
#include "../core/IRateLimiter.h"

class FixedWindowLimiter : public IRateLimiter{
    public:
        explicit FixedWindowLimiter(const Config& config);

        bool allowRequest(const std::string& clientId) override;
    
    private:
        Config config_;
        std::unordered_map<std::string, FixedWindowData> users_;
};