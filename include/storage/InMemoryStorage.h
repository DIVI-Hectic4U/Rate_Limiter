#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <chrono>

// LEGACY: This storage abstraction is not currently used by any algorithm.
// Each limiter manages its own per-client unordered_map internally.
// Kept for potential future refactoring into a unified storage backend.

struct UserData
{
    int requestCount = 0;
    std::chrono::steady_clock::time_point windowStart;
};

class InMemoryStorage{

    public:
        bool contains(const std::string& clientId) const;
        UserData* get(const std::string& clientId);

        void insert(const std::string& clientId,
                    const UserData& userData);

    private:
        std::unordered_map<std::string,UserData> users_;
};