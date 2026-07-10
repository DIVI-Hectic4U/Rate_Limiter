#pragma once

#include <optional>
#include <string>
#include <unordered_map>

#include "../core/UserData.h"

class InMemoryStorage{

    public:
        bool contains(const std::string& clientId) const;
        UserData* get(const std::string& clientId);

        void insert(const std::string& clientId,
                    const UserData& userData);

    private:
        std::unordered_map<std::string,UserData> users_;
};