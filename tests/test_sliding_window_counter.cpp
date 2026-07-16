#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include "algorithms/SlidingWindowCounterLimiter.h"

class SlidingWindowCounterTest : public ::testing::Test
{
protected:
    Config config_{.maxRequests = 3, .windowSize = std::chrono::seconds(1)};
    SlidingWindowCounterLimiter limiter_{config_};
};

TEST_F(SlidingWindowCounterTest, AllowsUpToMaxRequests)
{
    EXPECT_TRUE(limiter_.allowRequest("alice"));
    EXPECT_TRUE(limiter_.allowRequest("alice"));
    EXPECT_TRUE(limiter_.allowRequest("alice"));
}

TEST_F(SlidingWindowCounterTest, DeniesAfterMaxRequests)
{
    for (int i = 0; i < 3; ++i)
        limiter_.allowRequest("alice");

    EXPECT_FALSE(limiter_.allowRequest("alice"));
}

// Weighted estimate: the counter uses prevWindowCount * overlapPercentage
// to smooth the transition between windows
TEST_F(SlidingWindowCounterTest, ResetsAfterWindowExpiry)
{
    for (int i = 0; i < 3; ++i)
        limiter_.allowRequest("alice");

    EXPECT_FALSE(limiter_.allowRequest("alice"));

    // Wait for window to fully expire (+ extra window to clear prev count)
    std::this_thread::sleep_for(std::chrono::milliseconds(2100));

    // Both prev and curr windows should be clear
    EXPECT_TRUE(limiter_.allowRequest("alice"));
}

TEST_F(SlidingWindowCounterTest, IsolatesClients)
{
    for (int i = 0; i < 3; ++i)
        limiter_.allowRequest("alice");

    EXPECT_FALSE(limiter_.allowRequest("alice"));
    EXPECT_TRUE(limiter_.allowRequest("bob"));
}

TEST_F(SlidingWindowCounterTest, ThrowsOnInvalidConfig)
{
    Config bad{.maxRequests = 0, .windowSize = std::chrono::seconds(60)};
    EXPECT_THROW({ SlidingWindowCounterLimiter limiter(bad); }, std::invalid_argument);
}
