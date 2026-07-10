#pragma once

#include "../algorithms/FixedWindowLimiter.h"

class HttpServer
{
public:
    explicit HttpServer(FixedWindowLimiter &limiter);

    void start();

private:
    FixedWindowLimiter &limiter_;
};