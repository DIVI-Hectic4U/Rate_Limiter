#pragma once

#include <memory>    
#include <string>
#include <crow.h>
#include "../core/IRateLimiter.h"

class HttpServer
{
public:
    explicit HttpServer(IRateLimiter& limiter);
    ~HttpServer();

    void start(int port = 8080);

private:
    IRateLimiter& limiter_; 
    std::unique_ptr<crow::SimpleApp> app_;

    void setupRoutes();
    
};