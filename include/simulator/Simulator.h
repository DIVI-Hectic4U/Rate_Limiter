#pragma once

#include "../algorithms/FixedWindowLimiter.h"

class Simulator
{
    public:
        explicit Simulator(FixedWindowLimiter& limiter);

        void run();

    private:
        FixedWindowLimiter& limiter_;
};