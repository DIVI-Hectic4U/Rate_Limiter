#pragma once

#include <chrono>
#include <string>
#include <unordered_map>

#include "UserData.h"

class RateLimiter{
    private:
        std::unordered_map<std::string, UserData> clients;

        int maxRequests;

        std::chrono::seconds windowDuration;

    public:
        RateLimiter(int maxRequests,
                    std:: chrono::seconds windowDuration);

        bool allowRequests(const std::string& clientId);
};