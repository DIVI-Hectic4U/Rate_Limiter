#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>

// Test all in-memory algorithms under concurrent thread pressure.
// This proves our std::mutex guards (added in Phase 1) actually work.

#include "algorithms/FixedWindowLimiter.h"
#include "algorithms/TokenBucketLimiter.h"
#include "algorithms/LeakyBucketLimiter.h"
#include "algorithms/SlidingWindowLogLimiter.h"
#include "algorithms/SlidingWindowCounterLimiter.h"

// Helper: fire N threads at a limiter, each making requestsPerThread calls.
// Returns {totalAllowed, totalDenied}.
static std::pair<int, int> concurrentBurst(
    IRateLimiter& limiter,
    const std::string& clientId,
    int numThreads,
    int requestsPerThread)
{
    std::atomic<int> allowed{0};
    std::atomic<int> denied{0};

    std::vector<std::thread> threads;
    threads.reserve(numThreads);

    for (int t = 0; t < numThreads; ++t)
    {
        threads.emplace_back([&]()
        {
            for (int r = 0; r < requestsPerThread; ++r)
            {
                if (limiter.allowRequest(clientId))
                    allowed.fetch_add(1, std::memory_order_relaxed);
                else
                    denied.fetch_add(1, std::memory_order_relaxed);
            }
        });
    }

    for (auto& th : threads)
        th.join();

    return {allowed.load(), denied.load()};
}

// ============================================================================
// Concurrency: FixedWindowLimiter
// ============================================================================
TEST(ConcurrencyTest, FixedWindow_ExactlyMaxAllowed)
{
    Config config{.maxRequests = 10, .windowSize = std::chrono::seconds(60)};
    FixedWindowLimiter limiter(config);

    // 8 threads × 5 requests = 40 total attempts, but only 10 should pass
    auto [allowed, denied] = concurrentBurst(limiter, "concurrent_client", 8, 5);

    EXPECT_EQ(allowed, 10) << "Race condition: more than maxRequests were allowed!";
    EXPECT_EQ(denied, 30);
    EXPECT_EQ(allowed + denied, 40);
}

// ============================================================================
// Concurrency: TokenBucketLimiter
// ============================================================================
TEST(ConcurrencyTest, TokenBucket_ExactlyMaxAllowed)
{
    Config config{.maxRequests = 10, .windowSize = std::chrono::seconds(60)};
    TokenBucketLimiter limiter(config);

    auto [allowed, denied] = concurrentBurst(limiter, "concurrent_client", 8, 5);

    // Token bucket starts with maxRequests tokens, so exactly 10 should be allowed
    EXPECT_EQ(allowed, 10) << "Race condition in TokenBucket!";
    EXPECT_EQ(allowed + denied, 40);
}

// ============================================================================
// Concurrency: LeakyBucketLimiter
// ============================================================================
TEST(ConcurrencyTest, LeakyBucket_ExactlyMaxAllowed)
{
    Config config{.maxRequests = 10, .windowSize = std::chrono::seconds(60)};
    LeakyBucketLimiter limiter(config);

    auto [allowed, denied] = concurrentBurst(limiter, "concurrent_client", 8, 5);

    EXPECT_EQ(allowed, 10) << "Race condition in LeakyBucket!";
    EXPECT_EQ(allowed + denied, 40);
}

// ============================================================================
// Concurrency: SlidingWindowLogLimiter
// ============================================================================
TEST(ConcurrencyTest, SlidingWindowLog_ExactlyMaxAllowed)
{
    Config config{.maxRequests = 10, .windowSize = std::chrono::seconds(60)};
    SlidingWindowLogLimiter limiter(config);

    auto [allowed, denied] = concurrentBurst(limiter, "concurrent_client", 8, 5);

    EXPECT_EQ(allowed, 10) << "Race condition in SlidingWindowLog!";
    EXPECT_EQ(allowed + denied, 40);
}

// ============================================================================
// Concurrency: SlidingWindowCounterLimiter
// ============================================================================
TEST(ConcurrencyTest, SlidingWindowCounter_NoMoreThanMaxAllowed)
{
    Config config{.maxRequests = 10, .windowSize = std::chrono::seconds(60)};
    SlidingWindowCounterLimiter limiter(config);

    auto [allowed, denied] = concurrentBurst(limiter, "concurrent_client", 8, 5);

    // SlidingWindowCounter uses floating-point weighted estimates,
    // so we assert at-most rather than exact equality.
    EXPECT_LE(allowed, 10) << "Race condition in SlidingWindowCounter!";
    EXPECT_EQ(allowed + denied, 40);
}

// ============================================================================
// Stress: high thread count doesn't crash (no segfaults, no UB)
// ============================================================================
TEST(ConcurrencyTest, HighThreadStress_NoCrash)
{
    Config config{.maxRequests = 100, .windowSize = std::chrono::seconds(60)};
    FixedWindowLimiter limiter(config);

    // 32 threads × 100 requests = 3200 total — stress-tests the mutex
    auto [allowed, denied] = concurrentBurst(limiter, "stress_client", 32, 100);

    EXPECT_EQ(allowed, 100);
    EXPECT_EQ(allowed + denied, 3200);
}
