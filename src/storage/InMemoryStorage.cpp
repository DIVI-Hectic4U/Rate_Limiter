#include "../../include/storage/InMemoryStorage.h"

bool InMemoryStorage::contains(const std::string &clientId) const
{
    return users_.find(clientId) != users_.end();
}

UserData *InMemoryStorage::get(const std::string &clientId)
{

    auto it = users_.find(clientId);

    if (it == users_.end())
    {
        return nullptr;
    }

    return &(it->second);
}

void InMemoryStorage::insert(const std::string &clientId,
                             const UserData &userData)
{
    users_.emplace(clientId, userData);
}