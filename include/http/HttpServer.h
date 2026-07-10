#pragma once

#include "../core/IRateLimiter.h"

namespace crow{
    class SimpleApp;
}
class HttpServer
{
public:
    explicit HttpServer(IRateLimiter& limiter);

    void start();

private:
    IRateLimiter& limiter_;
};