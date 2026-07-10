#pragma once

#include<unordered_map>

#include "../core/Config.h"
#include "../core/UserData.h"

class FixedWindowLimiter{
    public:
        explicit FixedWindowLimiter(const Config& config);

        bool allowRequest(const std::string& clientId);
    
    private:
        Config config_;
        std::unordered_map<std::string,UserData> users_;
};