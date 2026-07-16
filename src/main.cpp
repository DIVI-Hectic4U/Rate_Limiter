#include <chrono>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <memory>

#include "../include/core/Config.h"
#include "../include/core/RateLimiterFactory.h"
#include "../include/http/HttpServer.h"

int main()
{
    try
    {
        // --- Configuration ---
        // Read from environment variables with sensible defaults.
        // This lets Docker Compose override values without recompilation.

        const char* maxReqEnv = std::getenv("MAX_REQUESTS");
        int maxRequests = maxReqEnv ? std::stoi(maxReqEnv) : 5;

        const char* windowEnv = std::getenv("WINDOW_SECONDS");
        int windowSeconds = windowEnv ? std::stoi(windowEnv) : 60;

        Config config{
            .maxRequests = maxRequests,
            .windowSize = std::chrono::seconds(windowSeconds)
        };

        // --- Algorithm Selection ---
        // ALGORITHM env var selects the limiter at runtime:
        //   fixed_window | token_bucket | leaky_bucket
        //   sliding_window_log | sliding_window_counter | redis_fixed_window
        const char* algoEnv = std::getenv("ALGORITHM");
        std::string algorithm = algoEnv ? algoEnv : "leaky_bucket";

        const char* redisUriEnv = std::getenv("REDIS_URI");
        std::string redisUri = redisUriEnv ? redisUriEnv : "tcp://127.0.0.1:6379";

        std::cout << "[Config] Algorithm: " << algorithm
                  << " | Max Requests: " << maxRequests
                  << " | Window: " << windowSeconds << "s\n";

        if (algorithm == "redis_fixed_window")
        {
            std::cout << "[Config] Redis URI: " << redisUri << "\n";
        }

        // --- Create Limiter via Factory ---
        // Factory returns std::unique_ptr<IRateLimiter> — zero raw pointers.
        // Ownership transfers to main(), passed by reference to HttpServer.
        std::unique_ptr<IRateLimiter> limiter = RateLimiterFactory::create(
            algorithm, config, redisUri);

        // --- Start HTTP Server ---
        std::cout << "[Main] Starting HTTP Server...\n";
        HttpServer server(*limiter);
        server.start();
    }
    catch (const std::exception& e)
    {
        std::cerr << "[FATAL] " << e.what() << '\n';
        return 1;
    }

    return 0;
}
