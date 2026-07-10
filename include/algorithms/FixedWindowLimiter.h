#pragma once

#include <string>
#include <unordered_map>
#include <chrono>

#include "../core/Config.h"
#include "../core/FixedWindowData.h"

class FixedWindowLimiter{
    public:
        explicit FixedWindowLimiter(const Config& config);

        bool allowRequest(const std::string& clientId);
    
    private:
        Config config_;
        std::unordered_map<std::string, FixedWindowData> users_;
};