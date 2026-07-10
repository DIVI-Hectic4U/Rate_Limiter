#include <chrono>

#include "../include/algorithms/FixedWindowLimiter.h"
#include "../include/http/HttpServer.h"

int main()
{
    Config config{
        .maxRequests = 5,
        .windowSize = std::chrono::seconds(60)
    };

    FixedWindowLimiter limiter(config);

    HttpServer server(limiter);
    
    server.start();
}
