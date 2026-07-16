#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include "algorithms/FixedWindowLimiter.h"

class FixedWindowTest : public ::testing::Test
{
protected:
    // Use a short 1-second window for fast tests
    Config config_{.maxRequests = 3, .windowSize = std::chrono::seconds(1)};
    FixedWindowLimiter limiter_{config_};
};

// Basic: first N requests should be allowed, N+1 should be denied
TEST_F(FixedWindowTest, AllowsUpToMaxRequests)
{
    EXPECT_TRUE(limiter_.allowRequest("alice"));
    EXPECT_TRUE(limiter_.allowRequest("alice"));
    EXPECT_TRUE(limiter_.allowRequest("alice"));
}

TEST_F(FixedWindowTest, DeniesAfterMaxRequests)
{
    for (int i = 0; i < 3; ++i)
        limiter_.allowRequest("alice");

    // Request N+1 must be blocked
    EXPECT_FALSE(limiter_.allowRequest("alice"));
    EXPECT_FALSE(limiter_.allowRequest("alice"));
}

// Per-client isolation: alice's limit doesn't affect bob
TEST_F(FixedWindowTest, IsolatesClients)
{
    for (int i = 0; i < 3; ++i)
        limiter_.allowRequest("alice");

    EXPECT_FALSE(limiter_.allowRequest("alice"));
    EXPECT_TRUE(limiter_.allowRequest("bob")); // bob is unaffected
}

// Window expiry: after the window passes, counter resets
TEST_F(FixedWindowTest, ResetsAfterWindowExpiry)
{
    for (int i = 0; i < 3; ++i)
        limiter_.allowRequest("alice");

    EXPECT_FALSE(limiter_.allowRequest("alice"));

    // Wait for window to expire (1 second + small margin)
    std::this_thread::sleep_for(std::chrono::milliseconds(1100));

    // Should be allowed again
    EXPECT_TRUE(limiter_.allowRequest("alice"));
}

// Constructor validation: bad config should throw
TEST_F(FixedWindowTest, ThrowsOnZeroMaxRequests)
{
    Config bad{.maxRequests = 0, .windowSize = std::chrono::seconds(60)};
    EXPECT_THROW({ FixedWindowLimiter limiter(bad); }, std::invalid_argument);
}

TEST_F(FixedWindowTest, ThrowsOnZeroWindowSize)
{
    Config bad{.maxRequests = 5, .windowSize = std::chrono::seconds(0)};
    EXPECT_THROW({ FixedWindowLimiter limiter(bad); }, std::invalid_argument);
}
