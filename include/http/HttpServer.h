#pragma once

#include "../core/IRateLimiter.h"
#include<crow.h>

namespace crow{
    class SimpleApp;
}
class HttpServer
{
public:
    explicit HttpServer(IRateLimiter& limiter);

    void start();

private:
    void setupRoutes(crow::SimpleApp& app);
    IRateLimiter& limiter_;
};