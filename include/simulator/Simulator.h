#pragma once

#include "../core/IRateLimiter.h"

class Simulator
{
    public:
        explicit Simulator(IRateLimiter& limiter);

        void run();

    private:
        IRateLimiter& limiter_;
};