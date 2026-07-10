#include <chrono>
#include <iostream>
#include <stdexcept>

#include "../include/algorithms/FixedWindowLimiter.h"
#include "../include/http/HttpServer.h"

int main()
{
    try
    {
        Config config{
            .maxRequests = 5,
            .windowSize = std::chrono::seconds(60)};

        FixedWindowLimiter limiter(config);

        HttpServer server(limiter);

        server.start();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Critical Server Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
