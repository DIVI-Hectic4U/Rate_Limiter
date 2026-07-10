#pragma once

#include "../core/Config.h"
#include "../storage/InMemoryStorage.h"

class FixedWindowLimiter{
    public:
        explicit FixedWindowLimiter(const Config& config);

        bool allowRequest(const std::string& clientId);
    
    private:
        Config config_;
        InMemoryStorage storage_;
};