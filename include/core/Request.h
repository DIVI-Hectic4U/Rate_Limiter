#pragma once

#include <chrono>
#include <string>

struct Request{
    
    std::string clientId;
    std::chrono::steady_clock::time_point timestamp;
};