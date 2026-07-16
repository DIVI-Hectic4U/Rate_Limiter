#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include "algorithms/LeakyBucketLimiter.h"

class LeakyBucketTest : public ::testing::Test
{
protected:
    // Bucket capacity 3, leaks at 3 requests/sec
    Config config_{.maxRequests = 3, .windowSize = std::chrono::seconds(1)};
    LeakyBucketLimiter limiter_{config_};
};

// Can fill up to capacity
TEST_F(LeakyBucketTest, AllowsUpToCapacity)
{
    EXPECT_TRUE(limiter_.allowRequest("alice"));
    EXPECT_TRUE(limiter_.allowRequest("alice"));
    EXPECT_TRUE(limiter_.allowRequest("alice"));
}

// Rejects when bucket is full
TEST_F(LeakyBucketTest, RejectsWhenFull)
{
    for (int i = 0; i < 3; ++i)
        limiter_.allowRequest("alice");

    EXPECT_FALSE(limiter_.allowRequest("alice"));
}

// Bucket drains over time, allowing new requests
TEST_F(LeakyBucketTest, DrainsOverTime)
{
    for (int i = 0; i < 3; ++i)
        limiter_.allowRequest("alice");

    EXPECT_FALSE(limiter_.allowRequest("alice"));

    // Wait for at least 1 request to leak out (at 3/sec, ~400ms leaks ~1.2)
    std::this_thread::sleep_for(std::chrono::milliseconds(400));

    EXPECT_TRUE(limiter_.allowRequest("alice"));
}

// Per-client isolation
TEST_F(LeakyBucketTest, IsolatesClients)
{
    for (int i = 0; i < 3; ++i)
        limiter_.allowRequest("alice");

    EXPECT_FALSE(limiter_.allowRequest("alice"));
    EXPECT_TRUE(limiter_.allowRequest("bob"));
}

// Constant drain: the leaky bucket provides a smooth, constant-rate output
// even when requests arrive in bursts
TEST_F(LeakyBucketTest, SmoothsDrainingUnderLoad)
{
    // Fill the bucket
    for (int i = 0; i < 3; ++i)
        limiter_.allowRequest("alice");

    // Over 1 full second, the entire bucket drains (3 req at 3/sec)
    std::this_thread::sleep_for(std::chrono::milliseconds(1100));

    // Bucket should be near-empty, can fill again
    EXPECT_TRUE(limiter_.allowRequest("alice"));
    EXPECT_TRUE(limiter_.allowRequest("alice"));
    EXPECT_TRUE(limiter_.allowRequest("alice"));
}

TEST_F(LeakyBucketTest, ThrowsOnInvalidConfig)
{
    Config bad{.maxRequests = -1, .windowSize = std::chrono::seconds(60)};
    EXPECT_THROW({ LeakyBucketLimiter limiter(bad); }, std::invalid_argument);
}
