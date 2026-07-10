#include "RateLimiter.h"
#include <stdexcept>

RateLimiter::RateLimiter(int maxRequests,
                         std::chrono::seconds windowDuration)
    : maxRequests(maxRequests),
      windowDuration(windowDuration)
{
    if (maxRequests <= 0)
    {
        throw std::invalid_argument("maxRequests must be greater than zero");
    }

    if (windowDuration <= std::chrono::seconds(0))
    {
        throw std::invalid_argument("windowDuration must be greater than zero");
    }
}

bool RateLimiter::allowRequests(const std::string& clientId){
    
    auto now = std::chrono::steady_clock::now();

    auto it = clients.find(clientId);

    // new Client

    if(it == clients.end()){

        UserData newUser;

        newUser.requestCount = 1;
        newUser.windowStart = now;

        clients.emplace(clientId,newUser);

        return true;
    }

    // Existing Client
    UserData &user = it->second;

    auto elapsed = now - user.windowStart;

    if(elapsed >= windowDuration){

        user.requestCount = 1;
        user.windowStart = now;
    }

    return false;
}