#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include "algorithms/SlidingWindowLogLimiter.h"

class SlidingWindowLogTest : public ::testing::Test
{
protected:
    Config config_{.maxRequests = 3, .windowSize = std::chrono::seconds(1)};
    SlidingWindowLogLimiter limiter_{config_};
};

TEST_F(SlidingWindowLogTest, AllowsUpToMaxRequests)
{
    EXPECT_TRUE(limiter_.allowRequest("alice"));
    EXPECT_TRUE(limiter_.allowRequest("alice"));
    EXPECT_TRUE(limiter_.allowRequest("alice"));
}

TEST_F(SlidingWindowLogTest, DeniesAfterMaxRequests)
{
    for (int i = 0; i < 3; ++i)
        limiter_.allowRequest("alice");

    EXPECT_FALSE(limiter_.allowRequest("alice"));
}

// The key advantage of sliding window log: no boundary burst problem.
// Old timestamps are evicted as the window slides forward.
TEST_F(SlidingWindowLogTest, SlidesWindowAndEvictsOldTimestamps)
{
    // Use all 3 slots
    for (int i = 0; i < 3; ++i)
        limiter_.allowRequest("alice");

    EXPECT_FALSE(limiter_.allowRequest("alice"));

    // Wait for all timestamps to age out
    std::this_thread::sleep_for(std::chrono::milliseconds(1100));

    // Old timestamps evicted, full quota available again
    EXPECT_TRUE(limiter_.allowRequest("alice"));
    EXPECT_TRUE(limiter_.allowRequest("alice"));
    EXPECT_TRUE(limiter_.allowRequest("alice"));
    EXPECT_FALSE(limiter_.allowRequest("alice"));
}

TEST_F(SlidingWindowLogTest, IsolatesClients)
{
    for (int i = 0; i < 3; ++i)
        limiter_.allowRequest("alice");

    EXPECT_FALSE(limiter_.allowRequest("alice"));
    EXPECT_TRUE(limiter_.allowRequest("bob"));
}

TEST_F(SlidingWindowLogTest, ThrowsOnInvalidConfig)
{
    Config bad{.maxRequests = 0, .windowSize = std::chrono::seconds(60)};
    EXPECT_THROW({ SlidingWindowLogLimiter limiter(bad); }, std::invalid_argument);
}
