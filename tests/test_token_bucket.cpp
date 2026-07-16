#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include "algorithms/TokenBucketLimiter.h"

class TokenBucketTest : public ::testing::Test
{
protected:
    // 3 tokens max, refill over 1 second → 3 tokens/sec refill rate
    Config config_{.maxRequests = 3, .windowSize = std::chrono::seconds(1)};
    TokenBucketLimiter limiter_{config_};
};

// Burst: all tokens can be consumed immediately
TEST_F(TokenBucketTest, AllowsBurstUpToMaxTokens)
{
    EXPECT_TRUE(limiter_.allowRequest("alice"));
    EXPECT_TRUE(limiter_.allowRequest("alice"));
    EXPECT_TRUE(limiter_.allowRequest("alice"));
}

// After burst, next request is denied (no tokens left)
TEST_F(TokenBucketTest, DeniesWhenTokensDepleted)
{
    for (int i = 0; i < 3; ++i)
        limiter_.allowRequest("alice");

    EXPECT_FALSE(limiter_.allowRequest("alice"));
}

// Tokens refill over time
TEST_F(TokenBucketTest, RefillsTokensOverTime)
{
    for (int i = 0; i < 3; ++i)
        limiter_.allowRequest("alice");

    EXPECT_FALSE(limiter_.allowRequest("alice"));

    // Wait for refill (at 3 tokens/sec, 500ms gives ~1.5 tokens)
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    EXPECT_TRUE(limiter_.allowRequest("alice"));
}

// Per-client isolation
TEST_F(TokenBucketTest, IsolatesClients)
{
    for (int i = 0; i < 3; ++i)
        limiter_.allowRequest("alice");

    EXPECT_FALSE(limiter_.allowRequest("alice"));
    EXPECT_TRUE(limiter_.allowRequest("bob"));
}

// Constructor validation
TEST_F(TokenBucketTest, ThrowsOnInvalidConfig)
{
    Config bad{.maxRequests = 0, .windowSize = std::chrono::seconds(60)};
    EXPECT_THROW({ TokenBucketLimiter limiter(bad); }, std::invalid_argument);
}
